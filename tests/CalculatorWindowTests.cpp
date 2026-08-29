#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTest>

#include "../src/ui/CalculatorWindow.h"

class CalculatorWindowTests : public QObject
{
    Q_OBJECT

private slots:
    void acceptsACompleteExpressionFromThePhysicalKeyboard();
    void insertsDigitsAndOperatorsWithButtons();
    void calculatesExpressionWithButton();
    void insertsTextAtTheCursorAndReplacesASelection();
    void insertsFunctionsWithExpectedCursorPlacement();
    void scientificLabelsInsertParserSyntax();
    void powerAndSquareButtonsUseExistingGrammar();
    void insertsPiAtTheCursor();
    void insertsEAndGeneralLogarithmSyntax();
    void switchesAngleModesAndCalculatesInDegrees();
    void screenBackspaceMatchesLineEditBehavior();
    void switchesMutuallyExclusiveKeypadModesWithoutLosingInput();
    void clearsStaleMessageOnlyWhenTextChanges();
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

void CalculatorWindowTests::acceptsACompleteExpressionFromThePhysicalKeyboard()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");

    QVERIFY(input);
    QVERIFY(message);

    QTest::keyClicks(input, "sin(pi/2)+ln(1)");
    QTest::keyClick(input, Qt::Key_Return);

    QCOMPARE(input->text(), "sin(pi/2)+ln(1)");
    QCOMPARE(message->text(), "Result: 1");
}

void CalculatorWindowTests::insertsDigitsAndOperatorsWithButtons()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* twoButton = window.findChild<QPushButton*>("basic2Button");
    auto* addButton = window.findChild<QPushButton*>("basicAddButton");
    auto* threeButton = window.findChild<QPushButton*>("basic3Button");

    QVERIFY(input);
    QVERIFY(twoButton);
    QVERIFY(addButton);
    QVERIFY(threeButton);

    QTest::mouseClick(twoButton, Qt::LeftButton);
    QTest::mouseClick(addButton, Qt::LeftButton);
    QTest::mouseClick(threeButton, Qt::LeftButton);

    QCOMPARE(input->text(), "2+3");
    QCOMPARE(input->cursorPosition(), 3);
    QVERIFY(input->hasFocus());
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

void CalculatorWindowTests::insertsTextAtTheCursorAndReplacesASelection()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* addButton = window.findChild<QPushButton*>("basicAddButton");
    auto* nineButton = window.findChild<QPushButton*>("basic9Button");

    QVERIFY(input);
    QVERIFY(addButton);
    QVERIFY(nineButton);

    input->setText("23");
    input->setCursorPosition(1);
    QTest::mouseClick(addButton, Qt::LeftButton);
    QCOMPARE(input->text(), "2+3");
    QCOMPARE(input->cursorPosition(), 2);

    input->setText("123");
    input->setSelection(1, 1);
    QTest::mouseClick(nineButton, Qt::LeftButton);
    QCOMPARE(input->text(), "193");
    QCOMPARE(input->cursorPosition(), 2);
}

void CalculatorWindowTests::insertsFunctionsWithExpectedCursorPlacement()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");
    auto* sinButton = window.findChild<QPushButton*>("sinButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(sinButton);

    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QTest::mouseClick(sinButton, Qt::LeftButton);
    QCOMPARE(input->text(), "sin()");
    QCOMPARE(input->cursorPosition(), 4);

    input->setText("2+3");
    input->selectAll();
    QTest::mouseClick(sinButton, Qt::LeftButton);
    QCOMPARE(input->text(), "sin(2+3)");
    QCOMPARE(input->cursorPosition(), 8);
    QVERIFY(!input->hasSelectedText());
}

void CalculatorWindowTests::scientificLabelsInsertParserSyntax()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");
    auto* sqrtButton = window.findChild<QPushButton*>("sqrtButton");
    auto* logarithmButton = window.findChild<QPushButton*>("log10Button");
    auto* absoluteValueButton = window.findChild<QPushButton*>("absButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(sqrtButton);
    QVERIFY(logarithmButton);
    QVERIFY(absoluteValueButton);

    QCOMPARE(sqrtButton->text(), QStringLiteral("√x"));
    QCOMPARE(logarithmButton->text(), QStringLiteral("log₁₀"));
    QCOMPARE(absoluteValueButton->text(), QStringLiteral("|x|"));

    QTest::mouseClick(functionsMode, Qt::LeftButton);

    QTest::mouseClick(sqrtButton, Qt::LeftButton);
    QCOMPARE(input->text(), "sqrt()");
    QCOMPARE(input->cursorPosition(), 5);

    input->setText("2+3");
    input->selectAll();
    QTest::mouseClick(sqrtButton, Qt::LeftButton);
    QCOMPARE(input->text(), "sqrt(2+3)");
    QCOMPARE(input->cursorPosition(), 9);

    input->clear();
    QTest::mouseClick(logarithmButton, Qt::LeftButton);
    QCOMPARE(input->text(), "log10()");
    QCOMPARE(input->cursorPosition(), 6);

    input->clear();
    QTest::mouseClick(absoluteValueButton, Qt::LeftButton);
    QCOMPARE(input->text(), "abs()");
    QCOMPARE(input->cursorPosition(), 4);

    input->setText("-2");
    input->selectAll();
    QTest::mouseClick(absoluteValueButton, Qt::LeftButton);
    QCOMPARE(input->text(), "abs(-2)");
    QCOMPARE(input->cursorPosition(), 7);
}

void CalculatorWindowTests::powerAndSquareButtonsUseExistingGrammar()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");
    auto* basicPowerButton = window.findChild<QPushButton*>("basicPowerButton");
    auto* functionsPowerButton = window.findChild<QPushButton*>("functionsPowerButton");
    auto* squareButton = window.findChild<QPushButton*>("squareButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(basicPowerButton);
    QVERIFY(functionsPowerButton);
    QVERIFY(squareButton);

    QCOMPARE(basicPowerButton->text(), QStringLiteral("xʸ"));
    QCOMPARE(functionsPowerButton->text(), QStringLiteral("xʸ"));
    QCOMPARE(squareButton->text(), QStringLiteral("x²"));

    input->setText("2");
    QTest::mouseClick(basicPowerButton, Qt::LeftButton);
    QCOMPARE(input->text(), "2^");
    QCOMPARE(input->cursorPosition(), 2);

    input->clear();
    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QTest::mouseClick(functionsPowerButton, Qt::LeftButton);
    QCOMPARE(input->text(), "^");
    QCOMPARE(input->cursorPosition(), 1);

    input->setText("5");
    QTest::mouseClick(squareButton, Qt::LeftButton);
    QCOMPARE(input->text(), "5^2");
    QCOMPARE(input->cursorPosition(), 3);

    input->setText("2+3");
    input->selectAll();
    QTest::mouseClick(squareButton, Qt::LeftButton);
    QCOMPARE(input->text(), "(2+3)^2");
    QCOMPARE(input->cursorPosition(), 7);
    QVERIFY(!input->hasSelectedText());
}

void CalculatorWindowTests::insertsPiAtTheCursor()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");
    auto* piButton = window.findChild<QPushButton*>("piButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(piButton);
    QCOMPARE(piButton->text(), QStringLiteral("π"));

    input->setText("2*");
    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QTest::mouseClick(piButton, Qt::LeftButton);

    QCOMPARE(input->text(), "2*pi");
    QCOMPARE(input->cursorPosition(), 4);
}

void CalculatorWindowTests::insertsEAndGeneralLogarithmSyntax()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");
    auto* eButton = window.findChild<QPushButton*>("eButton");
    auto* logButton = window.findChild<QPushButton*>("logButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(eButton);
    QVERIFY(logButton);
    QCOMPARE(eButton->text(), "e");
    QCOMPARE(logButton->text(), "log");

    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QTest::mouseClick(eButton, Qt::LeftButton);
    QCOMPARE(input->text(), "e");
    QCOMPARE(input->cursorPosition(), 1);

    input->clear();
    QTest::mouseClick(logButton, Qt::LeftButton);
    QCOMPARE(input->text(), "log(, )");
    QCOMPARE(input->cursorPosition(), 4);

    input->setText("8");
    input->selectAll();
    QTest::mouseClick(logButton, Qt::LeftButton);
    QCOMPARE(input->text(), "log(, 8)");
    QCOMPARE(input->cursorPosition(), 4);
    QVERIFY(!input->hasSelectedText());
}

void CalculatorWindowTests::switchesAngleModesAndCalculatesInDegrees()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");
    auto* radiansButton = window.findChild<QPushButton*>("radiansButton");
    auto* degreesButton = window.findChild<QPushButton*>("degreesButton");

    QVERIFY(input);
    QVERIFY(message);
    QVERIFY(radiansButton);
    QVERIFY(degreesButton);
    QVERIFY(radiansButton->isChecked());
    QVERIFY(!degreesButton->isChecked());

    input->setText("sin(90)");
    QTest::keyClick(input, Qt::Key_Return);
    QVERIFY(message->text() != "Result: 1");
    const QString expression = input->text();

    QTest::mouseClick(degreesButton, Qt::LeftButton);
    QVERIFY(!radiansButton->isChecked());
    QVERIFY(degreesButton->isChecked());
    QCOMPARE(input->text(), expression);
    QVERIFY(message->text().isEmpty());

    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Result: 1");

    QTest::mouseClick(radiansButton, Qt::LeftButton);
    QVERIFY(radiansButton->isChecked());
    QVERIFY(!degreesButton->isChecked());
    QCOMPARE(input->text(), expression);
    QVERIFY(message->text().isEmpty());
    QVERIFY(input->hasFocus());
}

void CalculatorWindowTests::screenBackspaceMatchesLineEditBehavior()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* backspaceButton = window.findChild<QPushButton*>("basicBackspaceButton");

    QVERIFY(input);
    QVERIFY(backspaceButton);

    input->setText("123");
    input->setCursorPosition(2);
    QTest::mouseClick(backspaceButton, Qt::LeftButton);
    QCOMPARE(input->text(), "13");
    QCOMPARE(input->cursorPosition(), 1);

    input->setText("123");
    input->setSelection(0, 2);
    QTest::mouseClick(backspaceButton, Qt::LeftButton);
    QCOMPARE(input->text(), "3");
    QCOMPARE(input->cursorPosition(), 0);
}

void CalculatorWindowTests::switchesMutuallyExclusiveKeypadModesWithoutLosingInput()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* stack = window.findChild<QStackedWidget*>("keypadStack");
    auto* basicMode = window.findChild<QPushButton*>("basicModeButton");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");

    QVERIFY(input);
    QVERIFY(stack);
    QVERIFY(basicMode);
    QVERIFY(functionsMode);

    input->setText("2+2");
    QVERIFY(basicMode->isChecked());
    QVERIFY(!functionsMode->isChecked());
    QCOMPARE(stack->currentIndex(), 0);

    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QVERIFY(!basicMode->isChecked());
    QVERIFY(functionsMode->isChecked());
    QCOMPARE(stack->currentIndex(), 1);
    QCOMPARE(input->text(), "2+2");

    QTest::mouseClick(basicMode, Qt::LeftButton);
    QVERIFY(basicMode->isChecked());
    QVERIFY(!functionsMode->isChecked());
    QCOMPARE(stack->currentIndex(), 0);
    QCOMPARE(input->text(), "2+2");
    QVERIFY(input->hasFocus());
}

void CalculatorWindowTests::clearsStaleMessageOnlyWhenTextChanges()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");

    QVERIFY(input);
    QVERIFY(message);

    input->setText("2+2");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Result: 4");

    QTest::keyClick(input, Qt::Key_Left);
    QCOMPARE(message->text(), "Result: 4");
    input->setSelection(0, 1);
    QCOMPARE(message->text(), "Result: 4");

    input->setCursorPosition(input->text().size());
    QTest::keyClicks(input, "+1");
    QVERIFY(message->text().isEmpty());
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
