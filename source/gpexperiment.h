/*****************************************************************************
 * gpexperiment.h
 *
 * Created: 8/19/2016 2016 by emigdio
 *
 * Copyright 2016 emigdio. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#ifndef GPEXPERIMENT_H
#define GPEXPERIMENT_H

#include <QVector>
#include <QString>
#include "GP.h"
#include "params.h"

class gpExperiment
{
public:

  gpExperiment();

  QVector<QVector<int> >    instanceIndexes;
  QVector<QVector<float> >  dataX;
  QVector<float>            dataY;
  Params                    gpParams;
  int                       trainingPartitionSize;
  int                       testingPartitionSize;
  QVector<long>             nodeEvaluations;
  QVector<long>             objectiveEvaluations;
  QString                   filename;
  qlonglong                 filesize;
  QString                   fileformat;
  int                       benchmarkIndex;
  int                       nFeatures;
  int                       nSamples;
  QVector<float>            averageSize;
  QVector<float>            averageTrainingFitness;
  QVector<float>            averageTestingFitness;
  QVector<float>            medianTrainingFitness;
  QVector<float>            medianTestingFitness;

  typedef struct
  {
    QVector<QVector<GP::treeStruct> > tree;
    QVector<QVector<int> > parents[2];
    QVector<QVector<int> > size;
    QVector<QVector<int> > operatorType;
    QVector<QVector<float> > normalizedFitness;
    QVector<QVector<float> > realFitness;
    QVector<QVector<int> > id;
    QVector<float> bestNormalizedTrainingFitness;
    QVector<float> bestRealTrainingFitness;
    QVector<float> bestSize;
    QVector<float> bestNormalizedTestingFitness;
    QVector<float> bestRealTestingFitness;
    QVector<float> avgSize;
    QVector<double> expectedOutput;
    QVector<double> actualOutput;
  } popData;

    QVector<popData>          population;
};

#endif // GPEXPERIMENT_H
