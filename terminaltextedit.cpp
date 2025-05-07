#include "terminaltextedit.h"
#include <QKeyEvent>
#include <QTextCursor>

TerminalTextEdit::TerminalTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
}

void TerminalTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {

        QString currentLine = getCurrentPromptLine().trimmed();
        if (!currentLine.isEmpty()) {
            commandHistory.append(currentLine);
            historyIndex = commandHistory.size();
            emit commandEntered(currentLine);
        }
        append("\n>>> ");
    }
    else if (event->key() == Qt::Key_Up) {

        if (!commandHistory.isEmpty() && historyIndex > 0) {
            historyIndex--;
            setCurrentCommand(commandHistory[historyIndex]);
        }
        event->accept();
    }
    else if (event->key() == Qt::Key_Down) {

        if (!commandHistory.isEmpty() && historyIndex < commandHistory.size() - 1) {
            historyIndex++;
            setCurrentCommand(commandHistory[historyIndex]);
        } else if (historyIndex == commandHistory.size() - 1) {
            historyIndex++;
            setCurrentCommand("");
        }
        event->accept();
    }
    else {
        QTextEdit::keyPressEvent(event);
    }
}

    QString TerminalTextEdit::getCurrentPromptLine() const
    {
        QString allText = toPlainText();
        int lastPrompt = allText.lastIndexOf(">>> ");
        if (lastPrompt != -1) {
            return allText.mid(lastPrompt + 4);
        }
        return "";
    }

void TerminalTextEdit::setCurrentCommand(const QString &command)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);

    QString allText = toPlainText();
    int lastPrompt = allText.lastIndexOf(">>> ");
    if (lastPrompt != -1) {
        cursor.setPosition(lastPrompt + 4);
        cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        cursor.insertText(command);
    }
}
