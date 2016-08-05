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
  int engineType;
  int dataPartitionType;
  int trainingRatio;

signals:
  void workRequested();
  void workFinished();
  void finished();

  void sendRunProgress(int value, int run);
  void sendSingleTree(GP::treeStruct data);
  void sendBasicInfo(GP::basicInfo info);

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
  void GP_received_run_progress(const int value, const int run);
  void GP_received_single_tree(GP::treeStruct data);
  void GP_received_basic_info(GP::basicInfo info);

};

#endif // WORKERGP_H
