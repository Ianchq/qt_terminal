#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "terminaltextedit.h"
#include <QTextCursor>
#include <QDir>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    TerminalTextEdit *terminal = new TerminalTextEdit(this);
    setCentralWidget(terminal);

    terminal->append(">>> ");

    connect(terminal, &TerminalTextEdit::commandEntered, this, &MainWindow::onCommandEntered);

    commandMap["ls"] = [this](const QString &cmd) { runLsCommand(cmd); };
    commandMap["pwd"] = [this](const QString &cmd) { runPwdCommand(cmd); };
    commandMap["echo"] = [this](const QString &cmd) { runEchoCommand(cmd); };
    commandMap["cd"] = [this](const QString &cmd) { runCdCommand(cmd); };
    commandMap["clear"] = [this] (const QString &cmd) { runClearCommand(cmd); };
    // commandMap["cat"] = [this](const QString &cmd) { runLsCommand(cmd); };
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCommandEntered(const QString &command)
{
    executeCommand(command);
}

void MainWindow::executeCommand(const QString &command)
{
    QString cmdName = command.section(' ', 0, 0);

    auto it = commandMap.find(cmdName);
    if (it != commandMap.end()) {
        it->second(command);
    } else {
        printUnknownCommand(command);
    }
}

void MainWindow::runLsCommand(const QString &command)
{
    TerminalTextEdit *terminal = qobject_cast<TerminalTextEdit *>(centralWidget());

    QStringList parts = command.split(" ", Qt::SkipEmptyParts);
    QString path = ".";
    bool showHidden = false;

    for (const QString &part : parts.mid(1)) {
        if (part == "-a") {
            showHidden = true;
        } else {
            path = part;
        }
    }

    QDir dir(path);
    if (!dir.exists()) {
        terminal->append("ls: cannot access '" + path + "': No such directory");
        return;
    }

    QDir::Filters filters = QDir::NoDotAndDotDot | QDir::AllEntries;
    if (showHidden) {
        filters |= QDir::Hidden;
    }

    QStringList entries = dir.entryList(filters);
    terminal->append(entries.join("\n"));
}

// void MainWindow::runLsCommand(const QString &command)
// {
//     TerminalTextEdit *terminal = qobject_cast<TerminalTextEdit *>(centralWidget());

//     QStringList parts = command.split(" ", Qt::SkipEmptyParts);
//     QString commandName = parts[0];
//     QStringList arguments = parts.mid(1);

//     if (commandName == "ls" || commandName == "pwd" || commandName == "echo" || commandName == "cd" || commandName == "cat") {
//         QProcess *process = new QProcess(this);
//         process->setProgram(commandName);
//         process->setArguments(arguments);

//         connect(process, &QProcess::readyReadStandardOutput, [=]() {
//             QByteArray output = process->readAllStandardOutput();
//             terminal->append(output);
//         });

//         process->start();
//         if (!process->waitForStarted()) {
//             terminal->append("Error: Could not start command.");
//         }
//     } else {
//         terminal->append("Unknown command: " + command);
//     }
// }

void MainWindow::runPwdCommand(const QString &command)
{
    TerminalTextEdit *terminal = qobject_cast<TerminalTextEdit *>(centralWidget());

    QStringList parts = command.split(" ", Qt::SkipEmptyParts);
    if (parts.size() > 1) {
        terminal->append("pwd: too many arguments");
        return;
    }

    QString currentDir = QDir::currentPath();
    terminal->append(currentDir);
}

void MainWindow::runEchoCommand(const QString &command)
{
    TerminalTextEdit *terminal = qobject_cast<TerminalTextEdit *>(centralWidget());

    QStringList parts = command.split(" ");
    parts.removeAt(0);
    QString output = parts.join(" ");

    terminal->append(output);
}

void MainWindow::printUnknownCommand(const QString &command)
{
    TerminalTextEdit *terminal = qobject_cast<TerminalTextEdit *>(centralWidget());
    terminal->append("Unknown command: " + command);
}

void MainWindow::runCdCommand(const QString &command)
{
    TerminalTextEdit *terminal = qobject_cast<TerminalTextEdit *>(centralWidget());

    QStringList parts = command.split(" ", Qt::SkipEmptyParts);

    QString path;

    if (parts.size() < 2) {
        path = QDir::homePath();
    } else {
        path = parts[1];
        if (path == "~") {
            path = QDir::homePath();
        }
    }

    QDir dir;
    bool success = dir.cd(path);
    if (success) {
        QDir::setCurrent(dir.absolutePath());
        terminal->append("Changed directory to: " + dir.absolutePath());
    } else {
        terminal->append("cd: no such directory: " + path);
    }
}

void MainWindow::runClearCommand(const QString &command)
{
    TerminalTextEdit *terminal = qobject_cast<TerminalTextEdit *>(centralWidget());
    terminal->clear();
}
