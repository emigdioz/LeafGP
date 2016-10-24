#include "mainwidget.h"
#include <QApplication>
#include <QSplashScreen>
#include <QTimer>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    //QThread::currentThread()->setPriority(QThread::HighPriority);
    QPixmap pixmap(":/icons/resources/images/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    //a.processEvents(QEventLoop::AllEvents);

    MainWidget w;
    QTimer::singleShot(2500,&splash,SLOT(close()));
    QTimer::singleShot(2500,&w,SLOT(show()));
    //w.show();
    //splash.finish(&w);
	return a.exec();
}
