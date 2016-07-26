#include "workergp.h"
#include <QDebug>

workerGP::workerGP(QObject *parent) : QObject(parent)
{
  _working = false;
  _abort = false;

  // ************** Temporal, forced to GPU here, it should initialize based on user options
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

  // ************** Temporal, forced to GPU here, it should initialize based on user options
  gp_parameters.Initialize();
  gp_engine = new FPI(gp_parameters);

  connect(gp_engine, SIGNAL(GP_send_run_progress(const int)), this, SLOT(GP_received_run_progress(const int)));

  // insert data to GP object

  gp_engine->insertDataTraining(data_matrix);
  gp_engine->Run();

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

void workerGP::GP_received_run_progress(const int value)
{
  emit sendRunProgress(value);
  //QCoreApplication::processEvents();
}
