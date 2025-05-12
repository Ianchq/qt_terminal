#ifndef TERMINALTEXTEDIT_H
#define TERMINALTEXTEDIT_H

#include <QTextEdit>

class TerminalTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit TerminalTextEdit(QWidget *parent = nullptr);

signals:
    void commandEntered(const QString &command);
    void interruptRequested();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void showCustomContextMenu(const QPoint &pos);

private:
    void insertPrompt();
    void scrollToBottom();
    QString getCurrentCommand() const;
    void loadHistory();
    void saveHistory();
    void createHistoryFileIfNeeded();
    QStringList findCompletions(const QString &prefix);
    void handleTabCompletion();
    void appendPrompt();

    QStringList commandHistory;
    int historyIndex = 0;
    const QString prompt = ">>> ";
    const int MaxHistorySize = 100;
    const QString HistoryFileName = "history.txt";
    bool selectingText = false;
};

#endif // TERMINALTEXTEDIT_H
