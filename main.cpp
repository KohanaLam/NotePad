#include "NotePad.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	NotePad window;
	window.show();
	return app.exec();
}