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
  Params gp_parameters;
  std::vector<std::vector<float> > data_matrix;

signals:
  void workRequested();
  void workFinished();
  void finished();

  void sendProgress1(int value);
  void sendRunProgress(int value);

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

private slots:
  void GP_received_run_progress(const int value);

};

#endif // WORKERGP_H
