#include "mainwidget.h"
#include <QApplication>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <QTimer>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QThread::currentThread()->setPriority(QThread::HighPriority);
  QPixmap pixmap(":/icons/resources/images/splash.png");
  QSplashScreen splash(pixmap);
  splash.show();
  MainWidget w;
  w.move(QApplication::desktop()->screen()->rect().center() -
         w.rect().center());
  QTimer::singleShot(3000, &splash, SLOT(close()));
  QTimer::singleShot(3000, &w, SLOT(show()));
  return a.exec();
}
