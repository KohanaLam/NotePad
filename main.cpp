#include "NotePad.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	// ¼ÇÊÂ±¾
	QApplication app(argc, argv);
	NotePad window;
	window.show();
	return app.exec();
}