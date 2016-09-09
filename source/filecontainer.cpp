/*****************************************************************************
 * filecontainer.cpp
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
#include "filecontainer.h"
#include <QDataStream>

fileContainer::fileContainer()
{

}

void fileContainer::insertHeader(quint32 value)
{
  header = value;
}

void fileContainer::insertVersion(qint32 value)
{
  version = value;
}

void fileContainer::write(QString filename)
{
  file.setFileName(filename);
  file.open(QIODevice::WriteOnly);
  outBuffer.setDevice(&file);
  outBuffer.setVersion(QDataStream::Qt_5_7);
  outBuffer << header;
  outBuffer << version;
  outBuffer << listSections;
  file.close();
}

void fileContainer::read(QString filename)
{
  file.setFileName(filename);
  file.open(QIODevice::ReadOnly);
  //outBuffer.device()->reset();
  outBuffer.setDevice(&file);
  outBuffer >> header;
  outBuffer >> version;
  outBuffer >> listSections;
  file.close();
}

quint32 fileContainer::extractHeader()
{
  return header;
}

qint32 fileContainer::extractVersion()
{
  return version;
}

void fileContainer::insertSection(section data)
{
  // insert just unique sections
  bool unique = true;
  for(int i = 0;i < listSections.length();i++)
    if(listSections.at(i).type == data.type)
      unique = false;
  if(unique)
    listSections << data;
}

void fileContainer::extractSections(QList<section> &data)
{
  data = listSections;
}

void fileContainer::insertDataset(QVector<QVector<float> > data)
{
  section dataset;
  dataset.name = "dataset";
  dataset.dataset = data;
  dataset.type = SECTION_DATASET;
  insertSection(dataset);
}

void fileContainer::insertExperiment(gpExperiment data)
{
  section experiment;
  experiment.name = "experiment";
  experiment.experiment = data;
  experiment.type = SECTION_EXPERIMENT;
  insertSection(experiment);
}

QDataStream &operator<<(QDataStream &stream, const section &data) {
  stream << data.name;
  stream << data.type;
  switch(data.type)
  {
    case SECTION_DATASET:
      stream << data.dataset;
      break;

    case SECTION_EXPERIMENT:

      break;

  }
  return stream;
}

QDataStream &operator>>(QDataStream &stream, section &data) {
  stream >> data.name;
  stream >> data.type;
  switch(data.type)
  {
    case SECTION_DATASET:
      stream >> data.dataset;
      break;
    case SECTION_EXPERIMENT:
      break;
  }
  return stream;
}
