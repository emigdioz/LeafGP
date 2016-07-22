#include "workergp.h"
#include <QDebug>

workerGP::workerGP(QObject *parent) : QObject(parent)
{
  _working = false;
  _abort = false;
}

void workerGP::abort()
{
  mutex.lock();
  if (_working) {
      _abort = true;
      qDebug() << "Request worker aborting in Thread " << thread()->currentThreadId();
  }
  mutex.unlock();
}

void workerGP::suspend()
{
  toSuspend = true;
}

void workerGP::resume()
{
  if (!toSuspend)
      return;
  toSuspend = false;
  mutex.lock();
  waitCondition.wakeOne();
  mutex.unlock();
}

void workerGP::requestWork()
{
  mutex.lock();
  _working = true;
  _abort = false;
  qDebug() << "Request worker start in Thread " << thread()->currentThreadId();
  mutex.unlock();

  emit workRequested();
}

void workerGP::doWork()
{
  qDebug() << "Starting worker process in Thread " << thread()->currentThreadId();
  mutex.lock();
  bool abort = _abort;
  mutex.unlock();
  if (abort) {
      qDebug() << "Aborting worker process in Thread " << thread()->currentThreadId();
  }

//  for(int i = 0;i < 1000;i++)
//  {
//    delay(10);
//    emit sendProgress1(i);
//  }
  parameters.Initialize();

  mutex.lock();
  _working = false;
  mutex.unlock();
  qDebug() << "Worker process finished in Thread " << thread()->currentThreadId();
  emit finished();
}

void workerGP::resumeRequested()
{
  qDebug() << "Resume requested";
  resume();
}

void workerGP::delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        //QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}
