#include "workergp.h"
#include <QDebug>
Q_DECLARE_METATYPE(GP::treeStruct);
Q_DECLARE_METATYPE(GP::basicInfo);
Q_DECLARE_METATYPE(GP::popInfo);

workerGP::workerGP(QObject *parent) : QObject(parent)
{
  qRegisterMetaType<GP::treeStruct>();
  qRegisterMetaType<GP::basicInfo>();
  qRegisterMetaType<GP::popInfo>();
  _working = false;
  _abort = false;
  engineType = 2; // local fitness evaluation
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

  if(engineType == 0 || engineType == 2) gp_engine = new GPonCPU(gp_parameters);
  if(engineType == 1) gp_engine = new FPI(gp_parameters);

  connect(gp_engine, SIGNAL(GP_send_run_progress(const int,const int)), this, SLOT(GP_received_run_progress(const int,const int)));
  connect(gp_engine, SIGNAL(GP_send_single_tree(GP::treeStruct)), this, SLOT(GP_received_single_tree(GP::treeStruct)));
  connect(gp_engine, SIGNAL(GP_send_basic_info(GP::basicInfo)), this, SLOT(GP_received_basic_info(GP::basicInfo)));
  connect(gp_engine, SIGNAL(GP_send_pop_info(GP::popInfo)), this, SLOT(GP_received_pop_info(GP::popInfo)));
  connect(this, SIGNAL(GP_pause_requested(bool)), gp_engine, SLOT(pauseRequested(bool)), Qt::DirectConnection);
  connect(this, SIGNAL(GP_stop_requested(bool)), gp_engine, SLOT(stopRequested(bool)), Qt::DirectConnection);
  // insert data to GP object
  gp_engine->engineType = engineType;
  gp_engine->insertData(data_matrix);
  gp_engine->dataPartitionType = dataPartitionType;
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

void workerGP::GP_received_run_progress(const int value, const int run)
{
  emit sendRunProgress(value,run);
  //QCoreApplication::processEvents();
}

void workerGP::GP_received_single_tree(GP::treeStruct data)
{
  emit sendSingleTree(data);
}

void workerGP::GP_received_basic_info(GP::basicInfo info)
{
	emit sendBasicInfo(info);
}

void workerGP::GP_received_pop_info(GP::popInfo info)
{
	emit sendPopInfo(info);
}

void workerGP::requestedPause(bool value)
{
	emit GP_pause_requested(value);
	//qDebug()<<"Pause requested (worker level)";
}

void workerGP::requestedStop(bool value)
{
	emit GP_stop_requested(value);
}
