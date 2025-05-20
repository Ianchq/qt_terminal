#include <gtest/gtest.h>
#include <QApplication>
#include <QtTest/QSignalSpy>
#include "terminaltextedit.h"
#include <QKeyEvent>
#include <QTest>

class TerminalTextEditTest : public ::testing::Test {
protected:
    static QApplication *app;
    TerminalTextEdit *editor;

    static void SetUpTestSuite() {
        static int argc = 1;
        static char* argv[] = { (char*)"test_app", nullptr };
        app = new QApplication(argc, argv);
    }


    static void TearDownTestSuite() {
        delete app;
        app = nullptr;
    }

    void SetUp() override {
        editor = new TerminalTextEdit();
        editor->show();
    }

    void TearDown() override {
        delete editor;
        editor = nullptr;
    }
};


QApplication* TerminalTextEditTest::app = nullptr;

TEST_F(TerminalTextEditTest, PromptIsInsertedOnStartup) {
    QString text = editor->toPlainText();
    EXPECT_TRUE(text.endsWith("$ "));
}

TEST_F(TerminalTextEditTest, AppendOutputAppendsText) {
    editor->appendOutput("test output\n");
    QString content = editor->toPlainText();
    EXPECT_TRUE(content.contains("test output"));
}

TEST_F(TerminalTextEditTest, EnterCommandEmitsSignal) {
    QSignalSpy spy(editor, &TerminalTextEdit::commandEntered);
    editor->insertPlainText("echo hello");

    QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(editor, &enterEvent);

    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args[0].toString(), "echo hello");
}


TEST_F(TerminalTextEditTest, CommandIsHighlightedCorrectly) {
    editor->insertPlainText("clear");

    QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(editor, &enterEvent);

    editor->insertPlainText("cd ");
    editor->highlightCommand();

    SUCCEED();
}

