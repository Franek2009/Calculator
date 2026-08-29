#ifndef CALCULATOR_WINDOW_H
#define CALCULATOR_WINDOW_H

#include <cstddef>
#include <optional>

#include <QMainWindow>

class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QFrame;
class QStackedWidget;
class QString;
class QWidget;

namespace Calculator
{
    enum class AngleMode;
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
        void insertLogarithm();
        void insertSquare();
        void backspace();
        void switchToBasicMode();
        void switchToFunctionsMode();
        void switchToRadians();
        void switchToDegrees();
        void toggleHistory(bool visible);
        void addHistoryEntry(const QString& expression,
                             double result,
                             Calculator::AngleMode mode);
        void recallHistoryEntry(QListWidgetItem* item);
        void clearHistory();
        void updateHistoryState();
        void updateAnswerTooltips();
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
        QPushButton* radiansButton;
        QPushButton* degreesButton;
        QPushButton* historyButton;
        QPushButton* clearHistoryButton;
        QPushButton* basicAnsButton;
        QPushButton* functionsAnsButton;
        QFrame* historyPanel;
        QListWidget* historyList;
        QLabel* historyEmptyLabel;
        Calculator::AngleMode angleMode;
        std::optional<double> lastAnswer;
    };
}

#endif
