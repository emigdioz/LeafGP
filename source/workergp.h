#ifndef WORKERGP_H
#define WORKERGP_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <QTime>
#include <QCoreApplication>
#include "GP.h"
#include "CPU.h"
#include "GPU.h"
#include "params.h"

class workerGP : public QObject
{
  Q_OBJECT
public:
  explicit workerGP(QObject *parent = 0);
  void abort();
  void suspend();
  void resume();
  void requestWork();
  GP* gp_engine;
  Params parameters;

signals:
  void workRequested();
  void workFinished();
  void finished();

  void sendProgress1(int value);

public slots:
  void doWork();
  void resumeRequested();

private:
  bool _abort;
  bool _working;
  bool toSuspend;
  QMutex mutex;
  QWaitCondition waitCondition;

  void delay(int millisecondsToWait);
};

#endif // WORKERGP_H
