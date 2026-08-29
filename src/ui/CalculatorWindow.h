#ifndef CALCULATOR_WINDOW_H
#define CALCULATOR_WINDOW_H

#include <cstddef>

#include <QMainWindow>

class QLabel;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QString;
class QWidget;

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
        void clearMessage();
        void insertText(const QString& text);
        void insertFunction(const QString& functionName);
        void backspace();
        void switchToBasicMode();
        void switchToFunctionsMode();
        void showCalculationError(const Calculator::CalculatorError& error);
        void setMessage(const QString& message, const QString& type);
        QPushButton* createKeyButton(const QString& label,
                                     const QString& objectName,
                                     const QString& role,
                                     QWidget* parent);
        QWidget* createBasicKeypad(QWidget* parent);
        QWidget* createFunctionsKeypad(QWidget* parent);
        static int stringIndexForByteOffset(const QString& text, std::size_t byteOffset);

        QLineEdit* expressionInput;
        QLabel* messageLabel;
        QStackedWidget* keypadStack;
        QPushButton* basicModeButton;
        QPushButton* functionsModeButton;
    };
}

#endif
