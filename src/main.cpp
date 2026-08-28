#include <QApplication>

#include "ui/CalculatorWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CalculatorUI::CalculatorWindow window;
    window.show();

    return app.exec();
}
