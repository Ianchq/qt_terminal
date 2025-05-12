#include "terminaltextedit.h"
#include <QScrollBar>
#include <QDir>
#include <QMouseEvent>

TerminalTextEdit::TerminalTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    setUndoRedoEnabled(false);
    setAcceptRichText(false);
    setCursorWidth(2);

    createHistoryFileIfNeeded();
    loadHistory();
    insertPrompt();
}

void TerminalTextEdit::insertPrompt()
{
    moveCursor(QTextCursor::End);
    QTextCursor cursor = textCursor();

    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    QString line = cursor.selectedText();

    if (line.trimmed().isEmpty()) {
        cursor.movePosition(QTextCursor::End);
        setTextCursor(cursor);
        insertPlainText(prompt);
    }

    ensureCursorVisible();
}

void TerminalTextEdit::scrollToBottom()
{
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

QString TerminalTextEdit::getCurrentCommand() const
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    QString line = cursor.selectedText();
    if (line.startsWith(prompt))
        return line.mid(prompt.length()).trimmed();
    return QString();
}

void TerminalTextEdit::keyPressEvent(QKeyEvent *event)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);

    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QString currentLine = getCurrentCommand();
        insertPlainText("\n");

        if (!currentLine.isEmpty()) {
            commandHistory.append(currentLine);
            historyIndex = commandHistory.size();
            saveHistory();
            emit commandEntered(currentLine);
        } else {
            insertPrompt();
        }
        scrollToBottom();
    }
    else if (event->key() == Qt::Key_Up) {
        if (historyIndex > 0) {
            historyIndex--;
            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            cursor.insertText(prompt + commandHistory.value(historyIndex));
        }
    }
    else if (event->key() == Qt::Key_Down) {
        if (historyIndex < commandHistory.size() - 1) {
            historyIndex++;
            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            cursor.insertText(prompt + commandHistory.value(historyIndex));
        }
        else {
            historyIndex = commandHistory.size();
            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            cursor.insertText(prompt);
        }
    }
    else if (event->key() == Qt::Key_Backspace) {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
        QString line = cursor.selectedText();
        if (line.length() <= prompt.length()) {
            return;
        }
        QTextEdit::keyPressEvent(event);
    }
    else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_C) {
        emit interruptRequested();
        return;
    }
    else if (event->key() == Qt::Key_Tab) {
        event->accept();
        handleTabCompletion();
    }
    else {
        QTextEdit::keyPressEvent(event);
    }
}

void TerminalTextEdit::loadHistory()
{
    QFile file(HistoryFileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                commandHistory.append(line);
            }
        }
        file.close();
    }
    historyIndex = commandHistory.size();
}

void TerminalTextEdit::saveHistory()
{
    while (commandHistory.size() > MaxHistorySize) {
        commandHistory.removeFirst();
    }

    QFile file(HistoryFileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&file);
        for (const QString &cmd : commandHistory) {
            out << cmd << "\n";
        }
        out.flush();
        file.flush();
        file.close();
    }
}

void TerminalTextEdit::createHistoryFileIfNeeded()
{
    QFile file(HistoryFileName);
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.close();
        }
    }
}

void TerminalTextEdit::mousePressEvent(QMouseEvent *event)
{
    selectingText = true;
    QTextEdit::mousePressEvent(event);
}

void TerminalTextEdit::mouseReleaseEvent(QMouseEvent *event)
{
    selectingText = false;
    QTextEdit::mouseReleaseEvent(event);
}

void TerminalTextEdit::handleTabCompletion()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    QString line = cursor.selectedText();

    QString currentInput = line.mid(prompt.length());
    QStringList parts = currentInput.split(' ', Qt::SkipEmptyParts);

    QString prefix;
    if (parts.isEmpty()) {
        prefix = "";
    } else {
        prefix = parts.last();
    }

    QStringList matches = findCompletions(prefix);
    if (matches.size() == 1) {
        parts.removeLast();
        parts.append(matches.first());
        QString completedLine = parts.join(' ');
        cursor.insertText(prompt + completedLine);

        if (completedLine != getCurrentCommand()) {
            commandHistory.append(completedLine);
            historyIndex = commandHistory.size();
            saveHistory();
        }
    } else if (matches.size() > 1) {
        append("\n" + matches.join("  "));
        appendPrompt();
    }
}


QStringList TerminalTextEdit::findCompletions(const QString &prefix)
{
    QStringList matches;

    QString pathEnv = qgetenv("PATH");
    QStringList paths = pathEnv.split(':');
    for (const QString &dir : paths) {
        QDir d(dir);
        QStringList files = d.entryList(QDir::Files | QDir::Executable);
        for (const QString &file : files) {
            if (file.startsWith(prefix) && !matches.contains(file))
                matches.append(file);
        }
    }

    QDir currentDir = QDir::current();
    QStringList localFiles = currentDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (const QString &file : localFiles) {
        if (file.startsWith(prefix) && !matches.contains(file))
            matches.append(file);
    }

    matches.sort();
    return matches;
}

void TerminalTextEdit::appendPrompt()
{
    append(prompt);
}
