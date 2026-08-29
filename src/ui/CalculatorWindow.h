#ifndef CALCULATOR_WINDOW_H
#define CALCULATOR_WINDOW_H

#include <cstddef>

#include <QMainWindow>

class QLabel;
class QLineEdit;
class QString;

namespace Calculator
{
    class CalculatorError;
}

namespace CalculatorUI
{
    class CalculatorWindow : public QMainWindow
    {
    public:
        explicit CalculatorWindow(QWidget* parent = nullptr);

    private:
        void calculateExpression();
        void clearExpression();
        void showCalculationError(const Calculator::CalculatorError& error);
        static int stringIndexForByteOffset(const QString& text, std::size_t byteOffset);

        QLineEdit* expressionInput;
        QLabel* messageLabel;
    };
}

#endif
