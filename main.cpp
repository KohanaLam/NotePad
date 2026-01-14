#include "NotePad.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    NotePad window;
    window.resize(600,400);
    window.show();
    return app.exec();
}
