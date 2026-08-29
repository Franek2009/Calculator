#include "CalculatorWindow.h"

#include "../core/math/CalculatorError.h"
#include "../core/math/Evaluator.h"
#include "../core/math/Lexer.h"
#include "../core/math/Parser.h"

#include <QByteArray>
#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QShortcut>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QStyle>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <exception>
#include <string>

namespace CalculatorUI
{
    namespace
    {
        constexpr auto windowStyle = R"(
            QMainWindow, QWidget#centralWidget {
                background-color: #10151c;
                color: #e7edf5;
            }
            QFrame#displayPanel {
                background-color: #151c24;
                border: 1px solid #303a47;
                border-radius: 12px;
            }
            QLineEdit#expressionInput {
                min-height: 54px;
                padding: 4px 8px;
                border: none;
                background: transparent;
                color: #f4f7fb;
                font-size: 24px;
                selection-background-color: #2469bd;
            }
            QLabel#messageLabel {
                min-height: 24px;
                padding: 0 8px 4px 8px;
                color: #bdc8d6;
                font-size: 14px;
            }
            QLabel#messageLabel[messageType="error"] {
                color: #ff8f94;
            }
            QPushButton {
                min-height: 48px;
                border: 1px solid #303a47;
                border-radius: 8px;
                background-color: #202832;
                color: #edf2f8;
                font-size: 16px;
            }
            QPushButton:hover {
                background-color: #293440;
                border-color: #415064;
            }
            QPushButton:pressed {
                background-color: #18202a;
            }
            QPushButton[buttonRole="function"] {
                background-color: #223044;
                color: #dcecff;
            }
            QPushButton[buttonRole="operator"] {
                color: #b9d9ff;
            }
            QPushButton[buttonRole="clear"] {
                color: #ffb8bb;
            }
            QPushButton[buttonRole="calculate"] {
                background-color: #1764c0;
                border-color: #2477d8;
                color: white;
                font-weight: 600;
            }
            QPushButton[buttonRole="calculate"]:hover {
                background-color: #2073d2;
            }
            QPushButton[buttonRole="mode"] {
                min-height: 34px;
                background-color: #181f28;
                color: #aeb9c7;
                font-size: 14px;
            }
            QPushButton[buttonRole="mode"]:checked {
                background-color: #1764c0;
                border-color: #2477d8;
                color: white;
                font-weight: 600;
            }
        )";
    }

    CalculatorWindow::CalculatorWindow(QWidget* parent)
        : QMainWindow(parent)
    {
        setWindowTitle("Calculator");
        setMinimumSize(600, 560);
        resize(680, 620);
        setStyleSheet(windowStyle);

        auto* centralWidget = new QWidget(this);
        centralWidget->setObjectName("centralWidget");
        auto* layout = new QVBoxLayout(centralWidget);
        layout->setContentsMargins(20, 20, 20, 20);
        layout->setSpacing(12);

        auto* displayPanel = new QFrame(centralWidget);
        displayPanel->setObjectName("displayPanel");
        auto* displayLayout = new QVBoxLayout(displayPanel);
        displayLayout->setContentsMargins(10, 8, 10, 8);
        displayLayout->setSpacing(2);

        expressionInput = new QLineEdit(displayPanel);
        expressionInput->setPlaceholderText("Enter an expression, e.g. sin(pi/2) + ln(10)");
        expressionInput->setObjectName("expressionInput");

        messageLabel = new QLabel(displayPanel);
        messageLabel->setWordWrap(true);
        messageLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        messageLabel->setObjectName("messageLabel");
        messageLabel->setProperty("messageType", "none");

        displayLayout->addWidget(expressionInput);
        displayLayout->addWidget(messageLabel);

        auto* modeLayout = new QHBoxLayout;
        modeLayout->setSpacing(6);

        basicModeButton = createKeyButton("Basic", "basicModeButton", "mode", centralWidget);
        functionsModeButton = createKeyButton("Functions", "functionsModeButton", "mode",
                                              centralWidget);
        basicModeButton->setCheckable(true);
        functionsModeButton->setCheckable(true);

        auto* modeGroup = new QButtonGroup(this);
        modeGroup->setExclusive(true);
        modeGroup->addButton(basicModeButton);
        modeGroup->addButton(functionsModeButton);
        basicModeButton->setChecked(true);

        modeLayout->addWidget(basicModeButton);
        modeLayout->addWidget(functionsModeButton);
        modeLayout->addStretch();

        keypadStack = new QStackedWidget(centralWidget);
        keypadStack->setObjectName("keypadStack");
        keypadStack->addWidget(createBasicKeypad(keypadStack));
        keypadStack->addWidget(createFunctionsKeypad(keypadStack));
        keypadStack->setCurrentIndex(0);

        layout->addWidget(displayPanel);
        layout->addLayout(modeLayout);
        layout->addWidget(keypadStack, 1);

        setCentralWidget(centralWidget);

        connect(expressionInput, &QLineEdit::returnPressed, this,
                [this]() { calculateExpression(); });
        connect(expressionInput, &QLineEdit::textChanged, this,
                [this]() { clearMessage(); });
        connect(basicModeButton, &QPushButton::clicked, this,
                [this]() { switchToBasicMode(); });
        connect(functionsModeButton, &QPushButton::clicked, this,
                [this]() { switchToFunctionsMode(); });

        auto* clearShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
        connect(clearShortcut, &QShortcut::activated, this,
                [this]() { clearExpression(); });

        expressionInput->setFocus();
    }

    void CalculatorWindow::calculateExpression()
    {
        try
        {
            const std::string input = expressionInput->text().toStdString();
            Calculator::Lexer lexer(input);
            const auto tokens = lexer.tokenize();
            Calculator::Parser parser(tokens);
            const auto expression = parser.parse();
            Calculator::Evaluator evaluator;
            const double result = evaluator.evaluate(expression);

            setMessage("Result: " + QString::number(result, 'g', 15), "result");
        }
        catch (const Calculator::CalculatorError& error)
        {
            showCalculationError(error);
        }
        catch (const std::exception& error)
        {
            setMessage("Unexpected error: " + QString::fromStdString(error.what()), "error");
        }
    }

    void CalculatorWindow::clearExpression()
    {
        expressionInput->clear();
        clearMessage();
        expressionInput->setFocus();
    }

    void CalculatorWindow::clearMessage()
    {
        setMessage({}, "none");
    }

    void CalculatorWindow::insertText(const QString& text)
    {
        expressionInput->insert(text);
        expressionInput->setFocus();
    }

    void CalculatorWindow::insertFunction(const QString& functionName)
    {
        if (expressionInput->hasSelectedText())
        {
            expressionInput->insert(functionName + "(" + expressionInput->selectedText() + ")");
        }
        else
        {
            expressionInput->insert(functionName + "()");
            expressionInput->setCursorPosition(expressionInput->cursorPosition() - 1);
        }

        expressionInput->setFocus();
    }

    void CalculatorWindow::insertSquare()
    {
        if (expressionInput->hasSelectedText())
        {
            expressionInput->insert("(" + expressionInput->selectedText() + ")^2");
        }
        else
        {
            expressionInput->insert("^2");
        }

        expressionInput->setFocus();
    }

    void CalculatorWindow::backspace()
    {
        expressionInput->backspace();
        expressionInput->setFocus();
    }

    void CalculatorWindow::switchToBasicMode()
    {
        basicModeButton->setChecked(true);
        keypadStack->setCurrentIndex(0);
        expressionInput->setFocus();
    }

    void CalculatorWindow::switchToFunctionsMode()
    {
        functionsModeButton->setChecked(true);
        keypadStack->setCurrentIndex(1);
        expressionInput->setFocus();
    }

    void CalculatorWindow::showCalculationError(const Calculator::CalculatorError& error)
    {
        const QString input = expressionInput->text();
        const int index = stringIndexForByteOffset(input, error.position());

        expressionInput->setFocus();

        if (error.position() < static_cast<std::size_t>(input.toUtf8().size()))
        {
            int selectionLength = 1;

            if (input.at(index).isHighSurrogate() &&
                index + 1 < input.size() &&
                input.at(index + 1).isLowSurrogate())
            {
                selectionLength = 2;
            }

            expressionInput->setSelection(index, selectionLength);
        }
        else
        {
            expressionInput->setCursorPosition(input.size());
        }

        setMessage(QString::fromStdString(error.what()), "error");
    }

    void CalculatorWindow::setMessage(const QString& message, const QString& type)
    {
        messageLabel->setText(message);
        messageLabel->setProperty("messageType", type);
        messageLabel->style()->unpolish(messageLabel);
        messageLabel->style()->polish(messageLabel);
    }

    QPushButton* CalculatorWindow::createKeyButton(const QString& label,
                                                   const QString& objectName,
                                                   const QString& role,
                                                   QWidget* parent)
    {
        auto* button = new QPushButton(label, parent);
        button->setObjectName(objectName);
        button->setProperty("buttonRole", role);
        button->setFocusPolicy(Qt::NoFocus);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        return button;
    }

    QWidget* CalculatorWindow::createBasicKeypad(QWidget* parent)
    {
        auto* keypad = new QWidget(parent);
        keypad->setObjectName("basicKeypad");
        auto* grid = new QGridLayout(keypad);
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setSpacing(8);

        const auto addInsertButton = [this, keypad, grid](const QString& label,
                                                          const QString& text,
                                                          const QString& name,
                                                          const QString& role,
                                                          int row,
                                                          int column)
        {
            auto* button = createKeyButton(label, name, role, keypad);
            connect(button, &QPushButton::clicked, this,
                    [this, text]() { insertText(text); });
            grid->addWidget(button, row, column);
        };

        addInsertButton("7", "7", "basic7Button", "digit", 0, 0);
        addInsertButton("8", "8", "basic8Button", "digit", 0, 1);
        addInsertButton("9", "9", "basic9Button", "digit", 0, 2);
        addInsertButton("/", "/", "basicDivideButton", "operator", 0, 3);

        auto* backspaceButton = createKeyButton("Backspace", "basicBackspaceButton",
                                                "operator", keypad);
        connect(backspaceButton, &QPushButton::clicked, this, [this]() { backspace(); });
        grid->addWidget(backspaceButton, 0, 4);

        addInsertButton("4", "4", "basic4Button", "digit", 1, 0);
        addInsertButton("5", "5", "basic5Button", "digit", 1, 1);
        addInsertButton("6", "6", "basic6Button", "digit", 1, 2);
        addInsertButton("*", "*", "basicMultiplyButton", "operator", 1, 3);
        addInsertButton("(", "(", "basicLeftParenthesisButton", "operator", 1, 4);

        addInsertButton("1", "1", "basic1Button", "digit", 2, 0);
        addInsertButton("2", "2", "basic2Button", "digit", 2, 1);
        addInsertButton("3", "3", "basic3Button", "digit", 2, 2);
        addInsertButton("-", "-", "basicSubtractButton", "operator", 2, 3);
        addInsertButton(")", ")", "basicRightParenthesisButton", "operator", 2, 4);

        addInsertButton("0", "0", "basic0Button", "digit", 3, 0);
        addInsertButton(".", ".", "basicDecimalButton", "digit", 3, 1);
        addInsertButton(QStringLiteral("xʸ"), "^", "basicPowerButton", "operator", 3, 2);
        addInsertButton("+", "+", "basicAddButton", "operator", 3, 3);

        auto* clearButton = createKeyButton("Clear", "clearButton", "clear", keypad);
        connect(clearButton, &QPushButton::clicked, this, [this]() { clearExpression(); });
        grid->addWidget(clearButton, 4, 0, 1, 2); // Columns 0-1 of the five-column grid.

        auto* calculateButton = createKeyButton("=", "calculateButton", "calculate", keypad);
        connect(calculateButton, &QPushButton::clicked, this,
                [this]() { calculateExpression(); });
        grid->addWidget(calculateButton, 4, 2, 1, 3); // Columns 2-4 of the five-column grid.

        for (int column = 0; column < 5; ++column)
        {
            grid->setColumnStretch(column, 1);
        }

        return keypad;
    }

    QWidget* CalculatorWindow::createFunctionsKeypad(QWidget* parent)
    {
        auto* keypad = new QWidget(parent);
        keypad->setObjectName("functionsKeypad");
        auto* grid = new QGridLayout(keypad);
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setSpacing(8);

        const auto addInsertButton = [this, keypad, grid](const QString& label,
                                                          const QString& text,
                                                          const QString& name,
                                                          const QString& role,
                                                          int row,
                                                          int column)
        {
            auto* button = createKeyButton(label, name, role, keypad);
            connect(button, &QPushButton::clicked, this,
                    [this, text]() { insertText(text); });
            grid->addWidget(button, row, column);
        };
        const auto addFunctionButton = [this, keypad, grid](const QString& label,
                                                            const QString& functionName,
                                                            const QString& name,
                                                            int row,
                                                            int column)
        {
            auto* button = createKeyButton(label, name, "function", keypad);
            connect(button, &QPushButton::clicked, this,
                    [this, functionName]() { insertFunction(functionName); });
            grid->addWidget(button, row, column);
        };

        addFunctionButton("sin", "sin", "sinButton", 0, 0);
        addFunctionButton("cos", "cos", "cosButton", 0, 1);
        addFunctionButton("tan", "tan", "tanButton", 0, 2);
        addInsertButton("(", "(", "functionsLeftParenthesisButton", "operator", 0, 3);
        addInsertButton(")", ")", "functionsRightParenthesisButton", "operator", 0, 4);

        auto* backspaceButton = createKeyButton("Backspace", "functionsBackspaceButton",
                                                "operator", keypad);
        connect(backspaceButton, &QPushButton::clicked, this, [this]() { backspace(); });
        grid->addWidget(backspaceButton, 0, 5, 1, 2);

        addFunctionButton(QStringLiteral("|x|"), "abs", "absButton", 1, 0);
        addFunctionButton("ln", "ln", "lnButton", 1, 1);
        addFunctionButton(QStringLiteral("log₁₀"), "log10", "log10Button", 1, 2);
        addInsertButton("7", "7", "functions7Button", "digit", 1, 3);
        addInsertButton("8", "8", "functions8Button", "digit", 1, 4);
        addInsertButton("9", "9", "functions9Button", "digit", 1, 5);
        addInsertButton("/", "/", "functionsDivideButton", "operator", 1, 6);

        addFunctionButton(QStringLiteral("√x"), "sqrt", "sqrtButton", 2, 0);
        addInsertButton(QStringLiteral("π"), "pi", "piButton", "function", 2, 1);
        addInsertButton(QStringLiteral("xʸ"), "^", "functionsPowerButton", "operator", 2, 2);
        addInsertButton("4", "4", "functions4Button", "digit", 2, 3);
        addInsertButton("5", "5", "functions5Button", "digit", 2, 4);
        addInsertButton("6", "6", "functions6Button", "digit", 2, 5);
        addInsertButton("*", "*", "functionsMultiplyButton", "operator", 2, 6);

        auto* squareButton = createKeyButton(QStringLiteral("x²"), "squareButton", "function",
                                             keypad);
        connect(squareButton, &QPushButton::clicked, this, [this]() { insertSquare(); });
        grid->addWidget(squareButton, 3, 0);
        addInsertButton("0", "0", "functions0Button", "digit", 3, 1);
        addInsertButton(".", ".", "functionsDecimalButton", "digit", 3, 2);
        addInsertButton("1", "1", "functions1Button", "digit", 3, 3);
        addInsertButton("2", "2", "functions2Button", "digit", 3, 4);
        addInsertButton("3", "3", "functions3Button", "digit", 3, 5);
        addInsertButton("-", "-", "functionsSubtractButton", "operator", 3, 6);

        auto* clearButton = createKeyButton("Clear", "functionsClearButton", "clear", keypad);
        connect(clearButton, &QPushButton::clicked, this, [this]() { clearExpression(); });
        grid->addWidget(clearButton, 4, 0);

        auto* calculateButton = createKeyButton("=", "functionsCalculateButton", "calculate",
                                                keypad);
        connect(calculateButton, &QPushButton::clicked, this,
                [this]() { calculateExpression(); });
        grid->addWidget(calculateButton, 4, 1, 1, 5);
        addInsertButton("+", "+", "functionsAddButton", "operator", 4, 6);

        for (int column = 0; column < 7; ++column)
        {
            grid->setColumnStretch(column, 1);
        }

        return keypad;
    }

    int CalculatorWindow::stringIndexForByteOffset(const QString& text,
                                                    std::size_t byteOffset)
    {
        const QByteArray utf8 = text.toUtf8();
        const auto clampedOffset = std::min(byteOffset,
                                            static_cast<std::size_t>(utf8.size()));

        return QString::fromUtf8(utf8.constData(),
                                 static_cast<qsizetype>(clampedOffset)).size();
    }
}
