#include "CalculatorWindow.h"

#include "../core/math/CalculatorError.h"
#include "../core/math/Evaluator.h"
#include "../core/math/Lexer.h"
#include "../core/math/Parser.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

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

        auto* calculateButton = new QPushButton("Calculate", centralWidget);

        messageLabel = new QLabel(centralWidget);
        messageLabel->setWordWrap(true);

        layout->addWidget(expressionInput);
        layout->addWidget(calculateButton);
        layout->addWidget(messageLabel);

        setCentralWidget(centralWidget);
        resize(400, 160);

        connect(calculateButton, &QPushButton::clicked, this,
                [this]() { calculateExpression(); });
        connect(expressionInput, &QLineEdit::returnPressed, this,
                [this]() { calculateExpression(); });

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
            messageLabel->setText(QString::fromStdString(error.what()));
        }
        catch (const std::exception& error)
        {
            messageLabel->setText("Unexpected error: " +
                                  QString::fromStdString(error.what()));
        }
    }
}
