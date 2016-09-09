/*****************************************************************************
 * filecontainer.h
 *
 * Created: 9/8/2016 2016 by emigdio
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
#ifndef FILECONTAINER_H
#define FILECONTAINER_H

#include <QFile>
#include <QDataStream>
#include <QString>
#include <QList>
#include <QVector>
#include "gpexperiment.h"

#define SECTION_DATASET    0
#define SECTION_SETTINGS   1
#define SECTION_EXPERIMENT 2

class section
{
public:
  QString name;
  int type;
  QVector<QVector< float> > dataset;
  gpExperiment experiment;
};


class fileContainer
{  
public:
  fileContainer();

  quint32 header;
  qint32 version;

  QFile file;
  QDataStream outBuffer;
  QList<section> listSections;

  void insertHeader(quint32 value);
  void insertVersion(qint32 value);
  void insertDataset(QVector<QVector<float> > data);
  void insertExperiment(gpExperiment data);
  void write(QString filename);
  void read(QString filename);
  quint32 extractHeader(void);
  qint32 extractVersion(void);

  void insertSection(section data);
  void extractSections(QList<section> &data);
};

QDataStream &operator<<(QDataStream &stream, const section &data);
QDataStream &operator>>(QDataStream &stream, section &data);

Q_DECLARE_METATYPE(section);

#endif // FILECONTAINER_H
