#include "CalculatorWindow.h"

#include "../core/math/CalculatorError.h"
#include "../core/math/Evaluator.h"
#include "../core/math/Lexer.h"
#include "../core/math/Parser.h"

#include <QByteArray>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QShortcut>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <exception>
#include <string>

namespace CalculatorUI
{
    CalculatorWindow::CalculatorWindow(QWidget* parent)
        : QMainWindow(parent)
    {
        setWindowTitle("Calculator");

        auto* centralWidget = new QWidget(this);
        auto* layout = new QVBoxLayout(centralWidget);

        expressionInput = new QLineEdit(centralWidget);
        expressionInput->setPlaceholderText("Example: sqrt(9)^2");
        expressionInput->setObjectName("expressionInput");

        auto* calculateButton = new QPushButton("Calculate", centralWidget);
        calculateButton->setObjectName("calculateButton");

        auto* clearButton = new QPushButton("Clear", centralWidget);
        clearButton->setObjectName("clearButton");

        messageLabel = new QLabel(centralWidget);
        messageLabel->setWordWrap(true);
        messageLabel->setObjectName("messageLabel");

        layout->addWidget(expressionInput);
        layout->addWidget(calculateButton);
        layout->addWidget(clearButton);
        layout->addWidget(messageLabel);

        setCentralWidget(centralWidget);
        resize(400, 160);

        connect(calculateButton, &QPushButton::clicked, this,
                [this]() { calculateExpression(); });
        connect(expressionInput, &QLineEdit::returnPressed, this,
                [this]() { calculateExpression(); });
        connect(clearButton, &QPushButton::clicked, this,
                [this]() { clearExpression(); });

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

            messageLabel->setText("Result: " + QString::number(result, 'g', 15));
        }
        catch (const Calculator::CalculatorError& error)
        {
            showCalculationError(error);
        }
        catch (const std::exception& error)
        {
            messageLabel->setText("Unexpected error: " +
                                  QString::fromStdString(error.what()));
        }
    }

    void CalculatorWindow::clearExpression()
    {
        expressionInput->clear();
        messageLabel->clear();
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

        messageLabel->setText(QString::fromStdString(error.what()));
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
