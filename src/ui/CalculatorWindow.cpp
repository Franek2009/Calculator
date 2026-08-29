#include "CalculatorWindow.h"

#include "../core/math/CalculatorError.h"
#include "../core/math/Evaluator.h"
#include "../core/math/Lexer.h"
#include "../core/math/Parser.h"

#include <QByteArray>
#include <QButtonGroup>
#include <QFontMetrics>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QList>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QShortcut>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>

#include <algorithm>
#include <exception>
#include <string>
#include <tuple>

namespace CalculatorUI
{
    namespace
    {
        constexpr int historyPanelWidth = 280;
        constexpr int historyEntryLimit = 100;
        constexpr int calculatorMinimumWidth = 600;
        constexpr int calculatorMaximumWidth = 960;
        constexpr int calculatorMinimumHeight = 560;
        constexpr int keypadButtonMaximumHeight = 72;
        constexpr int scientificCategoryHeight = 36;
        constexpr int scientificPageHeight = 56;
        constexpr int expressionRole = Qt::UserRole;
        constexpr int resultRole = Qt::UserRole + 1;
        constexpr int angleModeRole = Qt::UserRole + 2;
        constexpr int resultTextRole = Qt::UserRole + 3;

        class ElidedLabel final : public QLabel
        {
        public:
            explicit ElidedLabel(QWidget* parent = nullptr)
                : QLabel(parent)
            {
            }

        protected:
            void paintEvent(QPaintEvent*) override
            {
                QPainter painter(this);
                painter.setPen(palette().color(foregroundRole()));
                painter.setFont(font());
                const QRect textRect = contentsRect();
                const QString elided = fontMetrics().elidedText(
                    text(), Qt::ElideRight, textRect.width()
                );
                painter.drawText(textRect, alignment(), elided);
            }
        };

        class HistoryItemDelegate final : public QStyledItemDelegate
        {
        public:
            using QStyledItemDelegate::QStyledItemDelegate;

            QSize sizeHint(const QStyleOptionViewItem& option,
                           const QModelIndex&) const override
            {
                Q_UNUSED(option);
                return {0, 62};
            }

            void paint(QPainter* painter,
                       const QStyleOptionViewItem& option,
                       const QModelIndex& index) const override
            {
                painter->save();

                QColor background = QColor("#10151c");
                if (option.state & QStyle::State_Selected)
                {
                    background = QColor("#234d7d");
                }
                else if (option.state & QStyle::State_MouseOver)
                {
                    background = QColor("#202b38");
                }
                painter->fillRect(option.rect, background);

                const QRect content = option.rect.adjusted(10, 6, -10, -6);
                QFont expressionFont = option.font;
                expressionFont.setWeight(QFont::Medium);
                painter->setFont(expressionFont);
                painter->setPen(QColor("#edf2f8"));
                painter->drawText(
                    content.left(), content.top(), content.width(), 22,
                    Qt::AlignLeft | Qt::AlignVCenter,
                    QFontMetrics(expressionFont).elidedText(
                        index.data(expressionRole).toString(),
                        Qt::ElideRight,
                        content.width()
                    )
                );

                const QString mode = static_cast<Calculator::AngleMode>(
                    index.data(angleModeRole).toInt()
                ) == Calculator::AngleMode::Radians ? "RAD" : "DEG";
                QFont detailFont = option.font;
                detailFont.setPointSizeF(std::max(8.0, detailFont.pointSizeF() - 1.0));
                painter->setFont(detailFont);
                const QFontMetrics details(detailFont);
                const int badgeWidth = details.horizontalAdvance(mode) + 14;
                const QRect badgeRect(
                    content.right() - badgeWidth + 1,
                    content.bottom() - 19,
                    badgeWidth,
                    18
                );
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor("#27384c"));
                painter->drawRoundedRect(badgeRect, 6, 6);
                painter->setPen(QColor("#b9d9ff"));
                painter->drawText(badgeRect, Qt::AlignCenter, mode);

                const int resultWidth = std::max(0, content.width() - badgeWidth - 10);
                painter->setPen(QColor("#aeb9c7"));
                painter->drawText(
                    content.left(), content.bottom() - 19, resultWidth, 18,
                    Qt::AlignLeft | Qt::AlignVCenter,
                    details.elidedText(
                        "= " + index.data(resultTextRole).toString(),
                        Qt::ElideRight,
                        resultWidth
                    )
                );

                painter->setPen(QColor("#27313d"));
                painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
                painter->restore();
            }
        };

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
            QFrame#historyPanel {
                background-color: #151c24;
                border-left: 1px solid #303a47;
            }
            QLabel#historyTitle {
                color: #e7edf5;
                font-size: 18px;
                font-weight: 600;
            }
            QLabel#historyEmptyLabel {
                color: #7f8b99;
                font-size: 14px;
            }
            QListWidget#historyList {
                border: 1px solid #303a47;
                border-radius: 8px;
                background-color: #10151c;
                color: #dce5ef;
                outline: none;
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
            QPushButton:disabled {
                background-color: #181f28;
                border-color: #27313d;
                color: #66717e;
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
        : QMainWindow(parent),
          angleMode(Calculator::AngleMode::Radians)
    {
        setWindowTitle("Calculator");
        setMinimumSize(calculatorMinimumWidth, calculatorMinimumHeight);
        resize(680, 620);
        setStyleSheet(windowStyle);

        auto* centralWidget = new QWidget(this);
        centralWidget->setObjectName("centralWidget");
        auto* mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        auto* calculatorPanel = new QWidget(centralWidget);
        calculatorPanel->setMinimumWidth(calculatorMinimumWidth);
        calculatorPanel->setMaximumWidth(calculatorMaximumWidth);
        auto* layout = new QVBoxLayout(calculatorPanel);
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
        expressionInput->setMinimumWidth(0);
        expressionInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        messageLabel = new ElidedLabel(displayPanel);
        messageLabel->setFixedHeight(28);
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
        historyButton = createKeyButton("History", "historyButton", "mode", centralWidget);
        historyButton->setToolTip("Show or hide history (Ctrl+H)");
        basicModeButton->setCheckable(true);
        functionsModeButton->setCheckable(true);
        historyButton->setCheckable(true);

        auto* modeGroup = new QButtonGroup(this);
        modeGroup->setExclusive(true);
        modeGroup->addButton(basicModeButton);
        modeGroup->addButton(functionsModeButton);
        basicModeButton->setChecked(true);

        radiansButton = createKeyButton("RAD", "radiansButton", "mode", centralWidget);
        degreesButton = createKeyButton("DEG", "degreesButton", "mode", centralWidget);
        radiansButton->setCheckable(true);
        degreesButton->setCheckable(true);

        auto* angleModeGroup = new QButtonGroup(this);
        angleModeGroup->setExclusive(true);
        angleModeGroup->addButton(radiansButton);
        angleModeGroup->addButton(degreesButton);
        radiansButton->setChecked(true);

        modeLayout->addWidget(basicModeButton);
        modeLayout->addWidget(functionsModeButton);
        modeLayout->addWidget(historyButton);
        modeLayout->addStretch();
        modeLayout->addWidget(radiansButton);
        modeLayout->addWidget(degreesButton);

        scientificShelf = createScientificShelf(calculatorPanel);
        scientificShelf->hide();
        auto* sharedKeypad = createSharedKeypad(calculatorPanel);

        layout->addWidget(displayPanel);
        layout->addLayout(modeLayout);
        layout->addWidget(scientificShelf);
        layout->addWidget(sharedKeypad, 1);

        historyPanel = new QFrame(centralWidget);
        historyPanel->setObjectName("historyPanel");
        historyPanel->setFixedWidth(historyPanelWidth);
        auto* historyLayout = new QVBoxLayout(historyPanel);
        historyLayout->setContentsMargins(14, 20, 14, 20);
        historyLayout->setSpacing(10);

        auto* historyTitle = new QLabel("History", historyPanel);
        historyTitle->setObjectName("historyTitle");
        clearHistoryButton = createKeyButton("Clear history", "clearHistoryButton",
                                              "clear", historyPanel);
        clearHistoryButton->setToolTip("Clear history (Ctrl+Shift+H)");
        clearHistoryButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        historyList = new QListWidget(historyPanel);
        historyList->setObjectName("historyList");
        historyList->setWordWrap(false);
        historyList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        historyList->setUniformItemSizes(true);
        historyList->setMouseTracking(true);
        historyList->setItemDelegate(new HistoryItemDelegate(historyList));

        historyEmptyLabel = new QLabel("No history yet", historyPanel);
        historyEmptyLabel->setObjectName("historyEmptyLabel");
        historyEmptyLabel->setAlignment(Qt::AlignCenter);
        historyEmptyLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        historyLayout->addWidget(historyTitle);
        historyLayout->addWidget(historyList, 1);
        historyLayout->addWidget(historyEmptyLabel, 1);
        historyLayout->addWidget(clearHistoryButton);
        historyPanel->hide();

        mainLayout->addStretch(1);
        mainLayout->addWidget(calculatorPanel, 1);
        mainLayout->addStretch(1);
        mainLayout->addWidget(historyPanel);

        setCentralWidget(centralWidget);

        connect(expressionInput, &QLineEdit::returnPressed, this,
                [this]() { calculateExpression(); });
        connect(expressionInput, &QLineEdit::textChanged, this,
                [this]() { clearMessage(); });
        connect(basicModeButton, &QPushButton::clicked, this,
                [this]() { switchToBasicMode(); });
        connect(functionsModeButton, &QPushButton::clicked, this,
                [this]() { switchToFunctionsMode(); });
        connect(radiansButton, &QPushButton::clicked, this,
                [this]() { switchToRadians(); });
        connect(degreesButton, &QPushButton::clicked, this,
                [this]() { switchToDegrees(); });
        connect(historyButton, &QPushButton::toggled, this,
                [this](bool visible) { toggleHistory(visible); });
        connect(clearHistoryButton, &QPushButton::clicked, this,
                [this]() { clearHistory(); });
        connect(historyList, &QListWidget::itemClicked, this,
                [this](QListWidgetItem* item) { recallHistoryEntry(item); });

        auto* clearShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
        connect(clearShortcut, &QShortcut::activated, this,
                [this]() { clearExpression(); });

        auto* historyShortcut = new QShortcut(QKeySequence("Ctrl+H"), this);
        connect(historyShortcut, &QShortcut::activated, this,
                [this]() { historyButton->toggle(); });

        auto* clearExpressionShortcut = new QShortcut(QKeySequence("Ctrl+L"), this);
        connect(clearExpressionShortcut, &QShortcut::activated, this,
                [this]() { clearExpression(); });

        auto* clearHistoryShortcut = new QShortcut(QKeySequence("Ctrl+Shift+H"), this);
        connect(clearHistoryShortcut, &QShortcut::activated, this,
                [this]()
                {
                    if (clearHistoryButton->isEnabled())
                    {
                        clearHistory();
                    }
                    else
                    {
                        expressionInput->setFocus();
                    }
                });

        updateHistoryState();
        updateAnswerTooltips();
        expressionInput->setFocus();
    }

    void CalculatorWindow::calculateExpression()
    {
        try
        {
            const QString originalExpression = expressionInput->text();
            const std::string input = originalExpression.toStdString();
            Calculator::Lexer lexer(input);
            const auto tokens = lexer.tokenize();
            Calculator::Parser parser(tokens);
            const auto expression = parser.parse();
            Calculator::Evaluator evaluator(Calculator::EvaluationContext{
                angleMode,
                lastAnswer
            });
            const double result = evaluator.evaluate(expression);

            lastAnswer = result;
            addHistoryEntry(originalExpression, result, angleMode);
            updateAnswerTooltips();
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

    void CalculatorWindow::insertLogarithm()
    {
        const bool hasSelection = expressionInput->hasSelectedText();
        const int insertionPosition = hasSelection
            ? expressionInput->selectionStart()
            : expressionInput->cursorPosition();
        const QString selectedValue = expressionInput->selectedText();

        expressionInput->insert(hasSelection
            ? "log(, " + selectedValue + ")"
            : "log(, )");
        expressionInput->setCursorPosition(insertionPosition + 4);
        expressionInput->setFocus();
    }

    void CalculatorWindow::insertPowerShortcut(int exponent)
    {
        const QString suffix = "^" + QString::number(exponent);
        if (expressionInput->hasSelectedText())
        {
            expressionInput->insert("(" + expressionInput->selectedText() + ")" + suffix);
        }
        else
        {
            expressionInput->insert(suffix);
        }

        expressionInput->setFocus();
    }

    void CalculatorWindow::insertPostfixOperator(const QString& text)
    {
        if (expressionInput->hasSelectedText())
        {
            expressionInput->insert("(" + expressionInput->selectedText() + ")" + text);
        }
        else
        {
            expressionInput->insert(text);
        }
        expressionInput->setFocus();
    }

    void CalculatorWindow::insertReciprocal()
    {
        const bool hasSelection = expressionInput->hasSelectedText();
        const int insertionPosition = hasSelection
            ? expressionInput->selectionStart()
            : expressionInput->cursorPosition();
        const QString selected = expressionInput->selectedText();
        expressionInput->insert(hasSelection ? "1/(" + selected + ")" : "1/()");
        expressionInput->setCursorPosition(
            hasSelection ? insertionPosition + selected.size() + 4 : insertionPosition + 3
        );
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
        scientificShelf->hide();
        expressionInput->setFocus();
    }

    void CalculatorWindow::switchToFunctionsMode()
    {
        functionsModeButton->setChecked(true);
        scientificShelf->show();
        expressionInput->setFocus();
    }

    void CalculatorWindow::switchScientificCategory(int index,
                                                     QPushButton* categoryButton)
    {
        categoryButton->setChecked(true);
        scientificStack->setCurrentIndex(index);
        expressionInput->setFocus();
    }

    void CalculatorWindow::switchToRadians()
    {
        radiansButton->setChecked(true);
        angleMode = Calculator::AngleMode::Radians;
        clearMessage();
        expressionInput->setFocus();
    }

    void CalculatorWindow::switchToDegrees()
    {
        degreesButton->setChecked(true);
        angleMode = Calculator::AngleMode::Degrees;
        clearMessage();
        expressionInput->setFocus();
    }

    void CalculatorWindow::toggleHistory(bool visible)
    {
        if (visible)
        {
            const int targetWidth = width() + historyPanelWidth;
            setMinimumWidth(calculatorMinimumWidth + historyPanelWidth);
            historyPanel->show();
            resize(targetWidth, height());
        }
        else
        {
            historyPanel->hide();
            setMinimumWidth(calculatorMinimumWidth);
            resize(std::max(calculatorMinimumWidth, width() - historyPanelWidth), height());
        }

        expressionInput->setFocus();
    }

    void CalculatorWindow::addHistoryEntry(const QString& expression,
                                           double result,
                                           Calculator::AngleMode mode)
    {
        const QString resultText = QString::number(result, 'g', 15);
        const QString modeText = mode == Calculator::AngleMode::Radians ? "RAD" : "DEG";
        auto* item = new QListWidgetItem(
            expression + "\n= " + resultText + " · " + modeText
        );
        item->setData(expressionRole, expression);
        item->setData(resultRole, result);
        item->setData(angleModeRole, static_cast<int>(mode));
        item->setData(resultTextRole, resultText);
        item->setToolTip(expression + "\n= " + resultText + " · " + modeText);
        historyList->insertItem(0, item);

        while (historyList->count() > historyEntryLimit)
        {
            delete historyList->takeItem(historyList->count() - 1);
        }

        updateHistoryState();
    }

    void CalculatorWindow::recallHistoryEntry(QListWidgetItem* item)
    {
        if (!item)
        {
            return;
        }

        expressionInput->setText(item->data(expressionRole).toString());
        const auto storedMode = static_cast<Calculator::AngleMode>(
            item->data(angleModeRole).toInt()
        );

        if (storedMode == Calculator::AngleMode::Radians)
        {
            switchToRadians();
        }
        else
        {
            switchToDegrees();
        }

        expressionInput->setCursorPosition(expressionInput->text().size());
        expressionInput->setFocus();
    }

    void CalculatorWindow::clearHistory()
    {
        historyList->clear();
        updateHistoryState();
        expressionInput->setFocus();
    }

    void CalculatorWindow::updateHistoryState()
    {
        const bool hasHistory = historyList->count() > 0;
        historyList->setVisible(hasHistory);
        historyEmptyLabel->setVisible(!hasHistory);
        clearHistoryButton->setEnabled(hasHistory);
    }

    void CalculatorWindow::updateAnswerTooltips()
    {
        const QString tooltip = lastAnswer
            ? "Insert Ans (Ans = " + QString::number(*lastAnswer, 'g', 15) + ")"
            : "Ans is not available yet";

        ansButton->setToolTip(tooltip);
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
        messageLabel->setToolTip(message);
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
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        button->setMaximumHeight(keypadButtonMaximumHeight);
        return button;
    }

    QWidget* CalculatorWindow::createSharedKeypad(QWidget* parent)
    {
        auto* keypad = new QWidget(parent);
        keypad->setObjectName("sharedKeypad");
        auto* grid = new QGridLayout(keypad);
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setSpacing(8);
        grid->setAlignment(Qt::AlignTop);

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
            return button;
        };

        addInsertButton("7", "7", "keypad7Button", "digit", 0, 0);
        addInsertButton("8", "8", "keypad8Button", "digit", 0, 1);
        addInsertButton("9", "9", "keypad9Button", "digit", 0, 2);
        addInsertButton("/", "/", "divideButton", "operator", 0, 3);

        auto* backspaceButton = createKeyButton("Backspace", "backspaceButton",
                                                "operator", keypad);
        connect(backspaceButton, &QPushButton::clicked, this, [this]() { backspace(); });
        grid->addWidget(backspaceButton, 0, 4);

        addInsertButton("4", "4", "keypad4Button", "digit", 1, 0);
        addInsertButton("5", "5", "keypad5Button", "digit", 1, 1);
        addInsertButton("6", "6", "keypad6Button", "digit", 1, 2);
        addInsertButton("*", "*", "multiplyButton", "operator", 1, 3);
        addInsertButton("(", "(", "leftParenthesisButton", "operator", 1, 4);

        addInsertButton("1", "1", "keypad1Button", "digit", 2, 0);
        addInsertButton("2", "2", "keypad2Button", "digit", 2, 1);
        addInsertButton("3", "3", "keypad3Button", "digit", 2, 2);
        addInsertButton("-", "-", "subtractButton", "operator", 2, 3);
        addInsertButton(")", ")", "rightParenthesisButton", "operator", 2, 4);

        addInsertButton("0", "0", "keypad0Button", "digit", 3, 0);
        addInsertButton(".", ".", "decimalButton", "digit", 3, 1);
        addInsertButton(QStringLiteral("xʸ"), "^", "powerButton", "operator", 3, 2);
        addInsertButton("+", "+", "addButton", "operator", 3, 3);
        ansButton = addInsertButton("Ans", "Ans", "ansButton", "function", 3, 4);

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

    QWidget* CalculatorWindow::createScientificShelf(QWidget* parent)
    {
        auto* shelf = new QWidget(parent);
        shelf->setObjectName("scientificShelf");
        shelf->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        auto* shelfLayout = new QVBoxLayout(shelf);
        shelfLayout->setContentsMargins(0, 0, 0, 0);
        shelfLayout->setSpacing(8);

        auto* categoryLayout = new QHBoxLayout;
        categoryLayout->setSpacing(6);
        auto* categoryGroup = new QButtonGroup(this);
        categoryGroup->setExclusive(true);
        const QStringList categoryLabels{"Trig", "Logs", "Powers", "More"};
        const QStringList categoryNames{"trigCategoryButton", "logsCategoryButton",
                                        "powersCategoryButton", "moreCategoryButton"};
        QList<QPushButton*> categoryButtons;
        for (int index = 0; index < categoryLabels.size(); ++index)
        {
            auto* button = createKeyButton(categoryLabels[index], categoryNames[index],
                                           "mode", shelf);
            button->setCheckable(true);
            button->setFixedHeight(scientificCategoryHeight);
            categoryGroup->addButton(button);
            categoryLayout->addWidget(button);
            categoryButtons.append(button);
            connect(button, &QPushButton::clicked, this,
                    [this, index, button]() { switchScientificCategory(index, button); });
        }

        scientificStack = new QStackedWidget(shelf);
        scientificStack->setObjectName("scientificStack");
        scientificStack->setFixedHeight(scientificPageHeight);

        const auto createPage = [this](const QString& name,
                                       const QList<std::tuple<QString, QString, QString>>& specs)
        {
            auto* page = new QWidget(scientificStack);
            page->setObjectName(name);
            auto* grid = new QGridLayout(page);
            grid->setContentsMargins(0, 0, 0, 0);
            grid->setSpacing(8);
            for (int index = 0; index < specs.size(); ++index)
            {
                const auto& [label, syntax, objectName] = specs[index];
                auto* button = createKeyButton(label, objectName, "function", page);
                if (syntax == "log")
                {
                    connect(button, &QPushButton::clicked, this,
                            [this]() { insertLogarithm(); });
                }
                else if (syntax == "!")
                {
                    connect(button, &QPushButton::clicked, this,
                            [this]() { insertPostfixOperator("!"); });
                }
                else if (syntax == "%")
                {
                    connect(button, &QPushButton::clicked, this,
                            [this]() { insertPostfixOperator("%"); });
                }
                else if (syntax == "1/x")
                {
                    connect(button, &QPushButton::clicked, this,
                            [this]() { insertReciprocal(); });
                }
                else if (syntax == "^2")
                {
                    connect(button, &QPushButton::clicked, this,
                            [this]() { insertPowerShortcut(2); });
                }
                else if (syntax == "^3")
                {
                    connect(button, &QPushButton::clicked, this,
                            [this]() { insertPowerShortcut(3); });
                }
                else if (syntax == "pi" || syntax == "e")
                {
                    connect(button, &QPushButton::clicked, this,
                            [this, syntax]() { insertText(syntax); });
                }
                else
                {
                    connect(button, &QPushButton::clicked, this,
                            [this, syntax]() { insertFunction(syntax); });
                }
                grid->addWidget(button, 0, index);
                grid->setColumnStretch(index, 1);
            }
            return page;
        };

        scientificStack->addWidget(createPage("trigPage", {
            {"sin", "sin", "sinButton"}, {"cos", "cos", "cosButton"},
            {"tan", "tan", "tanButton"}, {QStringLiteral("sin⁻¹"), "asin", "arcSineButton"},
            {QStringLiteral("cos⁻¹"), "acos", "arcCosineButton"},
            {QStringLiteral("tan⁻¹"), "atan", "arcTangentButton"}
        }));
        scientificStack->addWidget(createPage("logsPage", {
            {"ln", "ln", "lnButton"}, {QStringLiteral("log₁₀"), "log10", "log10Button"},
            {"log", "log", "logButton"}
        }));
        scientificStack->addWidget(createPage("powersPage", {
            {QStringLiteral("√x"), "sqrt", "sqrtButton"},
            {QStringLiteral("x²"), "^2", "squareButton"},
            {QStringLiteral("x³"), "^3", "cubeButton"}, {"1/x", "1/x", "reciprocalButton"}
        }));
        scientificStack->addWidget(createPage("morePage", {
            {QStringLiteral("|x|"), "abs", "absButton"}, {"!", "!", "factorialButton"},
            {"%", "%", "percentageButton"}, {QStringLiteral("π"), "pi", "piButton"},
            {"e", "e", "eButton"}
        }));

        categoryButtons.front()->setChecked(true);
        scientificStack->setCurrentIndex(0);
        shelfLayout->addLayout(categoryLayout);
        shelfLayout->addWidget(scientificStack);
        return shelf;
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
