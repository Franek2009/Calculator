#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTest>

#include "../src/ui/CalculatorWindow.h"

class CalculatorWindowTests : public QObject
{
    Q_OBJECT

private slots:
    void calculatesExpressionWithEnter();
    void calculatesExpressionWithButton();
    void selectsAnInExpressionError();
    void placesCursorAtEndForEofError();
    void clearsInputAndMessageWithButtonAndEscape();
};

namespace
{
    void showWindow(CalculatorUI::CalculatorWindow& window)
    {
        window.show();
        QTest::qWait(10);
    }
}

void CalculatorWindowTests::calculatesExpressionWithEnter()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");

    QVERIFY(input);
    QVERIFY(message);

    input->setText("2+3*4");
    QTest::keyClick(input, Qt::Key_Return);

    QCOMPARE(message->text(), "Result: 14");
}

void CalculatorWindowTests::calculatesExpressionWithButton()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* calculateButton = window.findChild<QPushButton*>("calculateButton");
    auto* message = window.findChild<QLabel*>("messageLabel");

    QVERIFY(input);
    QVERIFY(calculateButton);
    QVERIFY(message);

    input->setText("sqrt(9)^2");
    QTest::mouseClick(calculateButton, Qt::LeftButton);

    QCOMPARE(message->text(), "Result: 9");
}

void CalculatorWindowTests::selectsAnInExpressionError()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");

    QVERIFY(input);
    QVERIFY(message);

    input->setText("2**3");
    QTest::keyClick(input, Qt::Key_Return);

    QCOMPARE(message->text(), "Syntax error at position 3: Expected expression");
    QCOMPARE(input->selectionStart(), 2);
    QCOMPARE(input->selectedText(), "*");
}

void CalculatorWindowTests::placesCursorAtEndForEofError()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");

    QVERIFY(input);
    QVERIFY(message);

    input->setText("2+");
    QTest::keyClick(input, Qt::Key_Return);

    QCOMPARE(message->text(), "Syntax error at position 3: Expected expression");
    QCOMPARE(input->cursorPosition(), 2);
    QVERIFY(!input->hasSelectedText());
}

void CalculatorWindowTests::clearsInputAndMessageWithButtonAndEscape()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* clearButton = window.findChild<QPushButton*>("clearButton");
    auto* message = window.findChild<QLabel*>("messageLabel");

    QVERIFY(input);
    QVERIFY(clearButton);
    QVERIFY(message);

    input->setText("2+2");
    QTest::keyClick(input, Qt::Key_Return);
    QTest::mouseClick(clearButton, Qt::LeftButton);

    QVERIFY(input->text().isEmpty());
    QVERIFY(message->text().isEmpty());
    QVERIFY(input->hasFocus());

    input->setText("2+2");
    QTest::keyClick(input, Qt::Key_Return);
    QTest::keyClick(input, Qt::Key_Escape);

    QVERIFY(input->text().isEmpty());
    QVERIFY(message->text().isEmpty());
    QVERIFY(input->hasFocus());
}

QTEST_MAIN(CalculatorWindowTests)

#include "CalculatorWindowTests.moc"
