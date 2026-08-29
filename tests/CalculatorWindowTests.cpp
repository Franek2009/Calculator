#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QTest>
#include <QWidget>

#include <algorithm>

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
    void inverseTrigButtonsInsertParserSyntax();
    void inverseTrigCalculatesWithAngleModesAndAns();
    void inverseTrigErrorsAndHistoryPreserveDiagnosticsAndMode();
    void utilityButtonsInsertExpectedSyntax();
    void postfixOperatorsCalculateReportErrorsAndUseHistory();
    void powerAndSquareButtonsUseExistingGrammar();
    void insertsPiAtTheCursor();
    void insertsEAndGeneralLogarithmSyntax();
    void switchesAngleModesAndCalculatesInDegrees();
    void ansButtonsInsertTheAnswerSymbol();
    void successfulCalculationsUpdateAnsAndErrorsPreserveIt();
    void historyStoresOnlySuccessfulCalculationsAndRecallsTheirMode();
    void recalledHistoryExpressionsUseTheCurrentAns();
    void historyCanBeClearedWithoutClearingAns();
    void historyKeepsOnlyTheHundredNewestEntries();
    void historyShowsEmptyAndEnabledStates();
    void applicationShortcutsPreserveInputFocus();
    void longContentKeepsStableLayoutAndFullTooltips();
    void historyAdjustsMinimumWidthWithoutResizeDrift();
    void ansTooltipsTrackTheLastSuccessfulResult();
    void screenBackspaceMatchesLineEditBehavior();
    void switchesMutuallyExclusiveKeypadModesWithoutLosingInput();
    void scientificCategoriesSwitchWithoutChangingInputOrGeometry();
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
    auto* twoButton = window.findChild<QPushButton*>("keypad2Button");
    auto* addButton = window.findChild<QPushButton*>("addButton");
    auto* threeButton = window.findChild<QPushButton*>("keypad3Button");

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
    auto* addButton = window.findChild<QPushButton*>("addButton");
    auto* nineButton = window.findChild<QPushButton*>("keypad9Button");

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
    auto* logsCategory = window.findChild<QPushButton*>("logsCategoryButton");
    auto* powersCategory = window.findChild<QPushButton*>("powersCategoryButton");
    auto* moreCategory = window.findChild<QPushButton*>("moreCategoryButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(sqrtButton);
    QVERIFY(logarithmButton);
    QVERIFY(absoluteValueButton);
    QVERIFY(logsCategory);
    QVERIFY(powersCategory);
    QVERIFY(moreCategory);

    QCOMPARE(sqrtButton->text(), QStringLiteral("√x"));
    QCOMPARE(logarithmButton->text(), QStringLiteral("log₁₀"));
    QCOMPARE(absoluteValueButton->text(), QStringLiteral("|x|"));

    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QTest::mouseClick(powersCategory, Qt::LeftButton);

    QTest::mouseClick(sqrtButton, Qt::LeftButton);
    QCOMPARE(input->text(), "sqrt()");
    QCOMPARE(input->cursorPosition(), 5);

    input->setText("2+3");
    input->selectAll();
    QTest::mouseClick(sqrtButton, Qt::LeftButton);
    QCOMPARE(input->text(), "sqrt(2+3)");
    QCOMPARE(input->cursorPosition(), 9);

    input->clear();
    QTest::mouseClick(logsCategory, Qt::LeftButton);
    QTest::mouseClick(logarithmButton, Qt::LeftButton);
    QCOMPARE(input->text(), "log10()");
    QCOMPARE(input->cursorPosition(), 6);

    input->clear();
    QTest::mouseClick(moreCategory, Qt::LeftButton);
    QTest::mouseClick(absoluteValueButton, Qt::LeftButton);
    QCOMPARE(input->text(), "abs()");
    QCOMPARE(input->cursorPosition(), 4);

    input->setText("-2");
    input->selectAll();
    QTest::mouseClick(absoluteValueButton, Qt::LeftButton);
    QCOMPARE(input->text(), "abs(-2)");
    QCOMPARE(input->cursorPosition(), 7);
}

void CalculatorWindowTests::inverseTrigButtonsInsertParserSyntax()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");
    auto* arcSineButton = window.findChild<QPushButton*>("arcSineButton");
    auto* arcCosineButton = window.findChild<QPushButton*>("arcCosineButton");
    auto* arcTangentButton = window.findChild<QPushButton*>("arcTangentButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(arcSineButton);
    QVERIFY(arcCosineButton);
    QVERIFY(arcTangentButton);
    QCOMPARE(arcSineButton->text(), QStringLiteral("sin⁻¹"));
    QCOMPARE(arcCosineButton->text(), QStringLiteral("cos⁻¹"));
    QCOMPARE(arcTangentButton->text(), QStringLiteral("tan⁻¹"));

    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QTest::mouseClick(arcSineButton, Qt::LeftButton);
    QCOMPARE(input->text(), "asin()");
    QCOMPARE(input->cursorPosition(), 5);

    input->setText("0.5");
    input->selectAll();
    QTest::mouseClick(arcCosineButton, Qt::LeftButton);
    QCOMPARE(input->text(), "acos(0.5)");
    QCOMPARE(input->cursorPosition(), 9);

    input->setText("1");
    input->selectAll();
    QTest::mouseClick(arcTangentButton, Qt::LeftButton);
    QCOMPARE(input->text(), "atan(1)");
    QCOMPARE(input->cursorPosition(), 7);
    QVERIFY(input->hasFocus());
}

void CalculatorWindowTests::inverseTrigCalculatesWithAngleModesAndAns()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");
    auto* degreesButton = window.findChild<QPushButton*>("degreesButton");
    auto* radiansButton = window.findChild<QPushButton*>("radiansButton");

    QVERIFY(input);
    QVERIFY(message);
    QVERIFY(degreesButton);
    QVERIFY(radiansButton);

    QTest::mouseClick(degreesButton, Qt::LeftButton);
    input->setText("asin(1)");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Result: 90");

    input->setText("1");
    QTest::keyClick(input, Qt::Key_Return);
    input->setText("asin(Ans)");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Result: 90");

    QTest::mouseClick(radiansButton, Qt::LeftButton);
    input->setText("asin(1)");
    QTest::keyClick(input, Qt::Key_Return);
    const double radians = message->text().mid(QString("Result: ").size()).toDouble();
    QVERIFY(std::abs(radians - std::numbers::pi / 2.0) < 1e-12);
}

void CalculatorWindowTests::inverseTrigErrorsAndHistoryPreserveDiagnosticsAndMode()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");
    auto* degreesButton = window.findChild<QPushButton*>("degreesButton");
    auto* radiansButton = window.findChild<QPushButton*>("radiansButton");
    auto* historyButton = window.findChild<QPushButton*>("historyButton");
    auto* history = window.findChild<QListWidget*>("historyList");

    QVERIFY(input);
    QVERIFY(message);
    QVERIFY(degreesButton);
    QVERIFY(radiansButton);
    QVERIFY(historyButton);
    QVERIFY(history);

    input->setText("asin(2)");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(),
             "Evaluation error at position 1: Arc sine argument must be between -1 and 1");
    QCOMPARE(input->selectionStart(), 0);
    QCOMPARE(input->selectedText(), "a");
    QCOMPARE(history->count(), 0);

    QTest::mouseClick(degreesButton, Qt::LeftButton);
    input->setText("asin(1)");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(history->count(), 1);
    QVERIFY(history->item(0)->text().startsWith("asin(1)\n= 90 · DEG"));

    QTest::mouseClick(radiansButton, Qt::LeftButton);
    QTest::mouseClick(historyButton, Qt::LeftButton);
    const QRect itemRect = history->visualItemRect(history->item(0));
    QTest::mouseClick(history->viewport(), Qt::LeftButton, Qt::NoModifier,
                      itemRect.center());
    QCOMPARE(input->text(), "asin(1)");
    QVERIFY(degreesButton->isChecked());
    QVERIFY(!radiansButton->isChecked());
    QVERIFY(message->text().isEmpty());
    QVERIFY(input->hasFocus());
}

void CalculatorWindowTests::utilityButtonsInsertExpectedSyntax()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");
    auto* factorialButton = window.findChild<QPushButton*>("factorialButton");
    auto* percentageButton = window.findChild<QPushButton*>("percentageButton");
    auto* reciprocalButton = window.findChild<QPushButton*>("reciprocalButton");
    auto* cubeButton = window.findChild<QPushButton*>("cubeButton");
    auto* squareButton = window.findChild<QPushButton*>("squareButton");
    auto* powersCategory = window.findChild<QPushButton*>("powersCategoryButton");
    auto* moreCategory = window.findChild<QPushButton*>("moreCategoryButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(factorialButton);
    QVERIFY(percentageButton);
    QVERIFY(reciprocalButton);
    QVERIFY(cubeButton);
    QVERIFY(squareButton);
    QVERIFY(powersCategory);
    QVERIFY(moreCategory);
    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QTest::mouseClick(moreCategory, Qt::LeftButton);

    input->setText("5");
    input->setCursorPosition(1);
    QTest::mouseClick(factorialButton, Qt::LeftButton);
    QCOMPARE(input->text(), "5!");

    input->setText("2+3");
    input->selectAll();
    QTest::mouseClick(percentageButton, Qt::LeftButton);
    QCOMPARE(input->text(), "(2+3)%");

    input->clear();
    QTest::mouseClick(powersCategory, Qt::LeftButton);
    QTest::mouseClick(reciprocalButton, Qt::LeftButton);
    QCOMPARE(input->text(), "1/()");
    QCOMPARE(input->cursorPosition(), 3);
    input->setText("2+3");
    input->selectAll();
    QTest::mouseClick(reciprocalButton, Qt::LeftButton);
    QCOMPARE(input->text(), "1/(2+3)");
    QCOMPARE(input->cursorPosition(), 7);

    input->setText("2+3");
    input->selectAll();
    QTest::mouseClick(cubeButton, Qt::LeftButton);
    QCOMPARE(input->text(), "(2+3)^3");
    input->setText("2+3");
    input->selectAll();
    QTest::mouseClick(squareButton, Qt::LeftButton);
    QCOMPARE(input->text(), "(2+3)^2");
    QVERIFY(input->hasFocus());
}

void CalculatorWindowTests::postfixOperatorsCalculateReportErrorsAndUseHistory()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");
    auto* history = window.findChild<QListWidget*>("historyList");

    QVERIFY(input);
    QVERIFY(message);
    QVERIFY(history);

    QTest::keyClicks(input, "5!");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Result: 120");
    QCOMPARE(history->count(), 1);

    input->setText("Ans%");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Result: 1.2");
    QCOMPARE(history->count(), 2);
    QVERIFY(history->item(0)->text().startsWith("Ans%\n= 1.2"));

    input->setText("2.5!");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(),
             "Evaluation error at position 4: Factorial is only defined for non-negative integers");
    QCOMPARE(input->selectionStart(), 3);
    QCOMPARE(input->selectedText(), "!");
    QCOMPARE(history->count(), 2);

    input->setText("5!!");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(),
             "Syntax error at position 3: Double factorial is not supported");
    QCOMPARE(input->selectionStart(), 2);
    QCOMPARE(input->selectedText(), "!");
    QCOMPARE(history->count(), 2);

    input->setText("1/(Ans)");
    QTest::keyClick(input, Qt::Key_Return);
    QVERIFY(message->text().startsWith("Result: "));
    QCOMPARE(history->count(), 3);
    QVERIFY(input->hasFocus());
}

void CalculatorWindowTests::powerAndSquareButtonsUseExistingGrammar()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");
    auto* powerButton = window.findChild<QPushButton*>("powerButton");
    auto* squareButton = window.findChild<QPushButton*>("squareButton");
    auto* powersCategory = window.findChild<QPushButton*>("powersCategoryButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(powerButton);
    QVERIFY(squareButton);
    QVERIFY(powersCategory);

    QCOMPARE(powerButton->text(), QStringLiteral("xʸ"));
    QCOMPARE(squareButton->text(), QStringLiteral("x²"));

    input->setText("2");
    QTest::mouseClick(powerButton, Qt::LeftButton);
    QCOMPARE(input->text(), "2^");
    QCOMPARE(input->cursorPosition(), 2);

    input->clear();
    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QTest::mouseClick(powerButton, Qt::LeftButton);
    QCOMPARE(input->text(), "^");
    QCOMPARE(input->cursorPosition(), 1);

    QTest::mouseClick(powersCategory, Qt::LeftButton);
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
    auto* moreCategory = window.findChild<QPushButton*>("moreCategoryButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(piButton);
    QVERIFY(moreCategory);
    QCOMPARE(piButton->text(), QStringLiteral("π"));

    input->setText("2*");
    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QTest::mouseClick(moreCategory, Qt::LeftButton);
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
    auto* logsCategory = window.findChild<QPushButton*>("logsCategoryButton");
    auto* moreCategory = window.findChild<QPushButton*>("moreCategoryButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(eButton);
    QVERIFY(logButton);
    QVERIFY(logsCategory);
    QVERIFY(moreCategory);
    QCOMPARE(eButton->text(), "e");
    QCOMPARE(logButton->text(), "log");

    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QTest::mouseClick(moreCategory, Qt::LeftButton);
    QTest::mouseClick(eButton, Qt::LeftButton);
    QCOMPARE(input->text(), "e");
    QCOMPARE(input->cursorPosition(), 1);

    input->clear();
    QTest::mouseClick(logsCategory, Qt::LeftButton);
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

void CalculatorWindowTests::ansButtonsInsertTheAnswerSymbol()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");
    auto* ansButton = window.findChild<QPushButton*>("ansButton");

    QVERIFY(input);
    QVERIFY(functionsMode);
    QVERIFY(ansButton);

    QTest::mouseClick(ansButton, Qt::LeftButton);
    QCOMPARE(input->text(), "Ans");
    QCOMPARE(input->cursorPosition(), 3);

    input->setText("2*3");
    input->setSelection(2, 1);
    QTest::mouseClick(ansButton, Qt::LeftButton);
    QCOMPARE(input->text(), "2*Ans");
    QCOMPARE(input->cursorPosition(), 5);

    input->clear();
    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QTest::mouseClick(ansButton, Qt::LeftButton);
    QCOMPARE(input->text(), "Ans");
    QCOMPARE(input->cursorPosition(), 3);
}

void CalculatorWindowTests::successfulCalculationsUpdateAnsAndErrorsPreserveIt()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");
    auto* history = window.findChild<QListWidget*>("historyList");
    auto* degreesButton = window.findChild<QPushButton*>("degreesButton");

    QVERIFY(input);
    QVERIFY(message);
    QVERIFY(history);
    QVERIFY(degreesButton);

    input->setText("Ans");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Evaluation error at position 1: Ans is not available");
    QCOMPARE(input->selectionStart(), 0);
    QCOMPARE(input->selectedText(), "A");
    QCOMPARE(history->count(), 0);

    input->setText("2+3");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Result: 5");

    input->setText("Ans*4");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Result: 20");

    input->setText("1/0");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(history->count(), 2);

    input->setText("2@3");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(history->count(), 2);

    input->setText("2+");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(history->count(), 2);

    QTest::mouseClick(degreesButton, Qt::LeftButton);
    input->setText("Ans+1");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Result: 21");
    QCOMPARE(history->count(), 3);
}

void CalculatorWindowTests::historyStoresOnlySuccessfulCalculationsAndRecallsTheirMode()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");
    auto* historyButton = window.findChild<QPushButton*>("historyButton");
    auto* history = window.findChild<QListWidget*>("historyList");
    auto* radiansButton = window.findChild<QPushButton*>("radiansButton");
    auto* degreesButton = window.findChild<QPushButton*>("degreesButton");

    QVERIFY(input);
    QVERIFY(message);
    QVERIFY(historyButton);
    QVERIFY(history);
    QVERIFY(radiansButton);
    QVERIFY(degreesButton);
    QVERIFY(!history->isVisible());

    QTest::mouseClick(degreesButton, Qt::LeftButton);
    input->setText("sin(90)");
    QTest::keyClick(input, Qt::Key_Return);

    QTest::mouseClick(radiansButton, Qt::LeftButton);
    input->setText("2+3");
    QTest::keyClick(input, Qt::Key_Return);

    input->setText("2+");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(history->count(), 2);
    QVERIFY(history->item(0)->text().startsWith("2+3\n= 5 · RAD"));
    QVERIFY(history->item(1)->text().startsWith("sin(90)\n= 1 · DEG"));

    const QString currentExpression = input->text();
    QTest::mouseClick(historyButton, Qt::LeftButton);
    QVERIFY(historyButton->isChecked());
    QVERIFY(history->isVisible());
    QCOMPARE(input->text(), currentExpression);
    QVERIFY(input->hasFocus());

    const QRect itemRect = history->visualItemRect(history->item(1));
    QTest::mouseClick(history->viewport(), Qt::LeftButton, Qt::NoModifier,
                      itemRect.center());
    QCOMPARE(input->text(), "sin(90)");
    QVERIFY(degreesButton->isChecked());
    QVERIFY(!radiansButton->isChecked());
    QVERIFY(message->text().isEmpty());
    QVERIFY(input->hasFocus());

    QTest::mouseClick(historyButton, Qt::LeftButton);
    QVERIFY(!historyButton->isChecked());
    QVERIFY(!history->isVisible());
    QCOMPARE(input->text(), "sin(90)");
}

void CalculatorWindowTests::recalledHistoryExpressionsUseTheCurrentAns()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");
    auto* historyButton = window.findChild<QPushButton*>("historyButton");
    auto* history = window.findChild<QListWidget*>("historyList");

    QVERIFY(input);
    QVERIFY(message);
    QVERIFY(historyButton);
    QVERIFY(history);

    input->setText("5");
    QTest::keyClick(input, Qt::Key_Return);
    input->setText("Ans*4");
    QTest::keyClick(input, Qt::Key_Return);
    input->setText("2");
    QTest::keyClick(input, Qt::Key_Return);

    QTest::mouseClick(historyButton, Qt::LeftButton);
    QVERIFY(history->item(1)->text().startsWith("Ans*4"));
    const QRect itemRect = history->visualItemRect(history->item(1));
    QTest::mouseClick(history->viewport(), Qt::LeftButton, Qt::NoModifier,
                      itemRect.center());
    QCOMPARE(input->text(), "Ans*4");

    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Result: 8");
}

void CalculatorWindowTests::historyCanBeClearedWithoutClearingAns()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");
    auto* history = window.findChild<QListWidget*>("historyList");
    auto* historyButton = window.findChild<QPushButton*>("historyButton");
    auto* clearHistoryButton = window.findChild<QPushButton*>("clearHistoryButton");

    QVERIFY(input);
    QVERIFY(message);
    QVERIFY(history);
    QVERIFY(historyButton);
    QVERIFY(clearHistoryButton);

    input->setText("10");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(history->count(), 1);

    QTest::mouseClick(historyButton, Qt::LeftButton);
    QTest::mouseClick(clearHistoryButton, Qt::LeftButton);
    QCOMPARE(history->count(), 0);
    QVERIFY(input->hasFocus());

    input->setText("Ans+1");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->text(), "Result: 11");
}

void CalculatorWindowTests::historyKeepsOnlyTheHundredNewestEntries()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* history = window.findChild<QListWidget*>("historyList");

    QVERIFY(input);
    QVERIFY(history);

    for (int value = 0; value < 101; ++value)
    {
        input->setText(QString::number(value));
        QTest::keyClick(input, Qt::Key_Return);
    }

    QCOMPARE(history->count(), 100);
    QVERIFY(history->item(0)->text().startsWith("100\n= 100"));
    QVERIFY(history->item(99)->text().startsWith("1\n= 1"));
}

void CalculatorWindowTests::historyShowsEmptyAndEnabledStates()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* historyButton = window.findChild<QPushButton*>("historyButton");
    auto* history = window.findChild<QListWidget*>("historyList");
    auto* emptyLabel = window.findChild<QLabel*>("historyEmptyLabel");
    auto* clearHistoryButton = window.findChild<QPushButton*>("clearHistoryButton");

    QVERIFY(input);
    QVERIFY(historyButton);
    QVERIFY(history);
    QVERIFY(emptyLabel);
    QVERIFY(clearHistoryButton);
    QVERIFY(!clearHistoryButton->isEnabled());

    QTest::mouseClick(historyButton, Qt::LeftButton);
    QVERIFY(emptyLabel->isVisible());
    QVERIFY(!history->isVisible());

    input->setText("2+3");
    QTest::keyClick(input, Qt::Key_Return);
    QVERIFY(!emptyLabel->isVisible());
    QVERIFY(history->isVisible());
    QVERIFY(clearHistoryButton->isEnabled());

    QTest::mouseClick(clearHistoryButton, Qt::LeftButton);
    QVERIFY(emptyLabel->isVisible());
    QVERIFY(!history->isVisible());
    QVERIFY(!clearHistoryButton->isEnabled());
    QVERIFY(input->hasFocus());
}

void CalculatorWindowTests::applicationShortcutsPreserveInputFocus()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* historyButton = window.findChild<QPushButton*>("historyButton");
    auto* history = window.findChild<QListWidget*>("historyList");

    QVERIFY(input);
    QVERIFY(historyButton);
    QVERIFY(history);

    input->setText("2+3");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(history->count(), 1);

    QTest::keyClick(input, Qt::Key_H, Qt::ControlModifier);
    QVERIFY(historyButton->isChecked());
    QVERIFY(history->isVisible());
    QVERIFY(input->hasFocus());

    QTest::keyClick(input, Qt::Key_L, Qt::ControlModifier);
    QVERIFY(input->text().isEmpty());
    QVERIFY(input->hasFocus());

    QTest::keyClick(input, Qt::Key_H, Qt::ControlModifier | Qt::ShiftModifier);
    QCOMPARE(history->count(), 0);
    QVERIFY(input->hasFocus());

    QTest::keyClick(input, Qt::Key_H, Qt::ControlModifier);
    QVERIFY(!historyButton->isChecked());
    QVERIFY(!history->isVisible());
    QVERIFY(input->hasFocus());
}

void CalculatorWindowTests::longContentKeepsStableLayoutAndFullTooltips()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* message = window.findChild<QLabel*>("messageLabel");
    auto* historyButton = window.findChild<QPushButton*>("historyButton");
    auto* history = window.findChild<QListWidget*>("historyList");

    QVERIFY(input);
    QVERIFY(message);
    QVERIFY(historyButton);
    QVERIFY(history);

    const int messageHeight = message->height();
    const QString longExpression = QString("1+").repeated(80) + "1";
    input->setText(longExpression);
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->height(), messageHeight);

    QTest::mouseClick(historyButton, Qt::LeftButton);
    QCOMPARE(history->horizontalScrollBarPolicy(), Qt::ScrollBarAlwaysOff);
    QVERIFY(history->item(0)->toolTip().contains(longExpression));
    const int longRowHeight = history->sizeHintForRow(0);

    input->setText("2");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(history->sizeHintForRow(0), longRowHeight);

    input->setText(QString("a").repeated(180));
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(message->height(), messageHeight);
    QCOMPARE(message->toolTip(), message->text());
    QVERIFY(message->text().size() > 100);
}

void CalculatorWindowTests::historyAdjustsMinimumWidthWithoutResizeDrift()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* historyButton = window.findChild<QPushButton*>("historyButton");
    auto* basicButton = window.findChild<QPushButton*>("keypad7Button");

    QVERIFY(input);
    QVERIFY(historyButton);
    QVERIFY(basicButton);
    QCOMPARE(window.minimumWidth(), 600);

    const int closedWidth = window.width();
    QTest::mouseClick(historyButton, Qt::LeftButton);
    QCOMPARE(window.minimumWidth(), 880);
    QTest::mouseClick(historyButton, Qt::LeftButton);
    QCOMPARE(window.minimumWidth(), 600);
    QCOMPARE(window.width(), closedWidth);

    window.resize(1400, 900);
    QTest::qWait(10);
    QVERIFY(basicButton->height() <= 72);
    QVERIFY(input->hasFocus());
}

void CalculatorWindowTests::ansTooltipsTrackTheLastSuccessfulResult()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* ansButton = window.findChild<QPushButton*>("ansButton");
    auto* degreesButton = window.findChild<QPushButton*>("degreesButton");
    auto* clearHistoryButton = window.findChild<QPushButton*>("clearHistoryButton");

    QVERIFY(input);
    QVERIFY(ansButton);
    QVERIFY(degreesButton);
    QVERIFY(clearHistoryButton);
    QCOMPARE(ansButton->toolTip(), "Ans is not available yet");

    input->setText("2+3");
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(ansButton->toolTip(), "Insert Ans (Ans = 5)");

    input->setText("1/0");
    QTest::keyClick(input, Qt::Key_Return);
    QTest::mouseClick(degreesButton, Qt::LeftButton);
    QTest::mouseClick(clearHistoryButton, Qt::LeftButton);
    QCOMPARE(ansButton->toolTip(), "Insert Ans (Ans = 5)");
    QVERIFY(input->hasFocus());
}

void CalculatorWindowTests::screenBackspaceMatchesLineEditBehavior()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* backspaceButton = window.findChild<QPushButton*>("backspaceButton");

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
    auto* shelf = window.findChild<QWidget*>("scientificShelf");
    auto* keypad = window.findChild<QWidget*>("sharedKeypad");
    auto* sevenButton = window.findChild<QPushButton*>("keypad7Button");
    auto* basicMode = window.findChild<QPushButton*>("basicModeButton");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");

    QVERIFY(input);
    QVERIFY(shelf);
    QVERIFY(keypad);
    QVERIFY(sevenButton);
    QVERIFY(basicMode);
    QVERIFY(functionsMode);
    QCOMPARE(window.findChildren<QPushButton*>("keypad7Button").size(), 1);

    input->setText("2+2");
    input->setSelection(2, 1);
    QVERIFY(basicMode->isChecked());
    QVERIFY(!functionsMode->isChecked());
    QVERIFY(!shelf->isVisible());
    QVERIFY(keypad->isVisible());

    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QVERIFY(!basicMode->isChecked());
    QVERIFY(functionsMode->isChecked());
    QVERIFY(shelf->isVisible());
    QVERIFY(keypad->isVisible());
    QCOMPARE(input->text(), "2+2");
    QCOMPARE(input->selectionStart(), 2);
    QCOMPARE(input->selectedText(), "2");

    input->setCursorPosition(input->text().size());
    QTest::mouseClick(sevenButton, Qt::LeftButton);
    QCOMPARE(input->text(), "2+27");

    QTest::mouseClick(basicMode, Qt::LeftButton);
    QVERIFY(basicMode->isChecked());
    QVERIFY(!functionsMode->isChecked());
    QVERIFY(!shelf->isVisible());
    QVERIFY(keypad->isVisible());
    QCOMPARE(input->text(), "2+27");
    QVERIFY(input->hasFocus());
}

void CalculatorWindowTests::scientificCategoriesSwitchWithoutChangingInputOrGeometry()
{
    CalculatorUI::CalculatorWindow window;
    showWindow(window);
    auto* input = window.findChild<QLineEdit*>("expressionInput");
    auto* shelf = window.findChild<QWidget*>("scientificShelf");
    auto* stack = window.findChild<QStackedWidget*>("scientificStack");
    auto* functionsMode = window.findChild<QPushButton*>("functionsModeButton");
    auto* basicMode = window.findChild<QPushButton*>("basicModeButton");
    auto* radiansButton = window.findChild<QPushButton*>("radiansButton");
    auto* trig = window.findChild<QPushButton*>("trigCategoryButton");
    auto* logs = window.findChild<QPushButton*>("logsCategoryButton");
    auto* powers = window.findChild<QPushButton*>("powersCategoryButton");
    auto* more = window.findChild<QPushButton*>("moreCategoryButton");
    auto* trigPage = window.findChild<QWidget*>("trigPage");
    auto* logsPage = window.findChild<QWidget*>("logsPage");
    auto* powersPage = window.findChild<QWidget*>("powersPage");
    auto* morePage = window.findChild<QWidget*>("morePage");

    QVERIFY(input);
    QVERIFY(shelf);
    QVERIFY(stack);
    QVERIFY(functionsMode);
    QVERIFY(basicMode);
    QVERIFY(radiansButton);
    QVERIFY(trig);
    QVERIFY(logs);
    QVERIFY(powers);
    QVERIFY(more);
    QVERIFY(trigPage);
    QVERIFY(logsPage);
    QVERIFY(powersPage);
    QVERIFY(morePage);
    QCOMPARE(trig->focusPolicy(), Qt::NoFocus);
    QCOMPARE(logs->focusPolicy(), Qt::NoFocus);
    QCOMPARE(powers->focusPolicy(), Qt::NoFocus);
    QCOMPARE(more->focusPolicy(), Qt::NoFocus);
    QVERIFY(trig->isChecked());
    QCOMPARE(stack->currentIndex(), 0);

    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QVERIFY(trigPage->isVisible());
    QVERIFY(!logsPage->isVisible());
    QVERIFY(!powersPage->isVisible());
    QVERIFY(!morePage->isVisible());
    const int shelfHeight = shelf->height();
    const int windowHeight = window.height();
    input->setText("12+34");
    input->setSelection(3, 2);

    const QList<QPushButton*> categories{logs, powers, more, trig};
    for (int index = 0; index < categories.size(); ++index)
    {
        QTest::mouseClick(categories[index], Qt::LeftButton);
        QCOMPARE(stack->currentIndex(), (index + 1) % 4);
        QCOMPARE(std::count_if(categories.cbegin(), categories.cend(),
                               [](const QPushButton* button) { return button->isChecked(); }), 1);
        QCOMPARE(input->text(), "12+34");
        QCOMPARE(input->selectionStart(), 3);
        QCOMPARE(input->selectedText(), "34");
        QCOMPARE(shelf->height(), shelfHeight);
        QCOMPARE(window.height(), windowHeight);
        QVERIFY(radiansButton->isChecked());
        QVERIFY(input->hasFocus());
    }

    QTest::mouseClick(more, Qt::LeftButton);
    input->setSelection(0, 2);
    QTest::mouseClick(basicMode, Qt::LeftButton);
    QVERIFY(!shelf->isVisible());
    QCOMPARE(input->selectionStart(), 0);
    QCOMPARE(input->selectedText(), "12");
    QTest::mouseClick(functionsMode, Qt::LeftButton);
    QVERIFY(shelf->isVisible());
    QVERIFY(more->isChecked());
    QCOMPARE(stack->currentIndex(), 3);
    QCOMPARE(input->selectionStart(), 0);
    QCOMPARE(input->selectedText(), "12");
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
