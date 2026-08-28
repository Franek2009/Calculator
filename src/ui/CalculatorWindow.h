#ifndef CALCULATOR_WINDOW_H
#define CALCULATOR_WINDOW_H

#include <QMainWindow>

class QLabel;
class QLineEdit;

namespace CalculatorUI
{
    class CalculatorWindow : public QMainWindow
    {
    public:
        explicit CalculatorWindow(QWidget* parent = nullptr);

    private:
        void calculateExpression();

        QLineEdit* expressionInput;
        QLabel* messageLabel;
    };
}

#endif
