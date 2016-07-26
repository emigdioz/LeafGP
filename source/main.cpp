#include "mainwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QThread::currentThread()->setPriority(QThread::HighPriority);
	MainWidget w;
	w.show();

	return a.exec();
}
