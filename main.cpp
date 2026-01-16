#include "NotePad.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	// 记事本
	QApplication app(argc, argv);
	// --- 加载 QSS 样式表 ---
	// ":/style.qss" 这里的冒号表示从资源系统加载，而不是从 C 盘加载
	QFile file(":/style.qss");
	if (file.open(QFile::ReadOnly)) {
		QString styleSheet = QLatin1String(file.readAll());
		app.setStyleSheet(styleSheet);
		file.close();
	}
	else {
		// 如果这里打印了，说明第二步没做对
		qDebug() << "Open failed!";
	}

	NotePad window;
	window.show();
	return app.exec();
}