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

void fileContainer::insertSettings(gpExperiment data)
{
  section settings;
  settings.name = "settings";
  settings.experiment = data;
  settings.type = SECTION_SETTINGS;
  insertSection(settings);
}

void fileContainer::insertPopulation(gpExperiment data)
{
  section population;
  population.name = "population";
  population.experiment = data;
  population.type = SECTION_POPULATION;
  insertSection(population);
}

void fileContainer::insertSummary(gpExperiment data)
{
  section summary;
  summary.name = "summary";
  summary.experiment = data;
  summary.type = SECTION_SUMMARY;
  insertSection(summary);
}

QDataStream &operator<<(QDataStream &stream, const section &data) {
  stream << data.name;
  stream << data.type;
  switch(data.type)
  {
    case SECTION_DATASET:
      stream << data.dataset;
      break;

    case SECTION_SETTINGS:
      {
        // gp parameters
        stream << QString::fromStdString(data.experiment.gpParams.m_primitives);
        stream << data.experiment.gpParams.m_population_size;
        stream << data.experiment.gpParams.m_number_of_generations;
        stream << qlonglong(data.experiment.gpParams.m_seed);
        stream << data.experiment.gpParams.m_tournament_size;
        stream << data.experiment.gpParams.m_elitism_size;
        stream << data.experiment.gpParams.m_maximum_tree_size;
        stream << data.experiment.gpParams.m_minimum_tree_size;
        stream << data.experiment.gpParams.m_crossover_probability;
        stream << data.experiment.gpParams.m_mutation_probability;
        stream << data.experiment.gpParams.m_clone_probability;
        stream << data.experiment.gpParams.m_number_of_runs;
        stream << data.experiment.gpParams.m_trainingRatio;
        // missing couple of parameters (device, cpu_cores, max_local_size)
        break;
      }
    case SECTION_POPULATION:
      {
        // save first runs, generations and individuals number
        stream << data.experiment.gpParams.m_number_of_runs;
        stream << data.experiment.gpParams.m_number_of_generations;
        stream << data.experiment.gpParams.m_population_size;
        for(int run = 0;run < data.experiment.gpParams.m_number_of_runs;run++)
        {
          for(int gen = 0;gen < data.experiment.gpParams.m_number_of_generations;gen++)
          {
            for(int ind = 0;ind < data.experiment.gpParams.m_population_size;ind++)
            {
              stream << data.experiment.population.at(run).tree.at(gen).at(ind).name;
              stream << data.experiment.population.at(run).tree.at(gen).at(ind).arity;
              stream << data.experiment.population.at(run).tree.at(gen).at(ind).subTreeSize;
              stream << data.experiment.population.at(run).tree.at(gen).at(ind).posX;
              stream << data.experiment.population.at(run).tree.at(gen).at(ind).posY;
              stream << data.experiment.population.at(run).tree.at(gen).at(ind).index;
              stream << data.experiment.population.at(run).tree.at(gen).at(ind).syntaxPrefix;
              stream << data.experiment.population.at(run).parents[0].at(gen).at(ind);
              stream << data.experiment.population.at(run).parents[1].at(gen).at(ind);
              stream << data.experiment.population.at(run).size.at(gen).at(ind);
              stream << data.experiment.population.at(run).operatorType.at(gen).at(ind);
              stream << data.experiment.population.at(run).normalizedFitness.at(gen).at(ind);
              stream << data.experiment.population.at(run).realFitness.at(gen).at(ind);
              stream << data.experiment.population.at(run).id.at(gen).at(ind);
            }
          }
          stream << data.experiment.population.at(run).bestNormalizedTrainingFitness;
          stream << data.experiment.population.at(run).bestRealTrainingFitness;
          stream << data.experiment.population.at(run).bestSize;
          stream << data.experiment.population.at(run).bestNormalizedTestingFitness;
          stream << data.experiment.population.at(run).bestRealTestingFitness;
          stream << data.experiment.population.at(run).avgSize;
          stream << data.experiment.population.at(run).expectedOutput;
          stream << data.experiment.population.at(run).actualOutput;
        }
        break;
      }
    case SECTION_SUMMARY:
      {
        stream << data.experiment.dataX;
        stream << data.experiment.dataY;
        stream << data.experiment.trainingPartitionSize;
        stream << data.experiment.testingPartitionSize;
        // missing node evaluations and objective evaluations
        stream << data.experiment.filename;
        stream << data.experiment.averageSize;
        stream << data.experiment.averageTrainingFitness;
        stream << data.experiment.averageTestingFitness;
        stream << data.experiment.medianTrainingFitness;
        stream << data.experiment.medianTestingFitness;
        break;
      }
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
    case SECTION_SETTINGS:
      {
        QString tPrimitives;
        qlonglong tSeed;
        stream >> tPrimitives;
        data.experiment.gpParams.m_primitives = tPrimitives.toStdString();
        stream >> data.experiment.gpParams.m_population_size;
        stream >> data.experiment.gpParams.m_number_of_generations;
        stream >> tSeed;
        data.experiment.gpParams.m_seed = (long)tSeed;
        stream >> data.experiment.gpParams.m_tournament_size;
        stream >> data.experiment.gpParams.m_elitism_size;
        stream >> data.experiment.gpParams.m_maximum_tree_size;
        stream >> data.experiment.gpParams.m_minimum_tree_size;
        stream >> data.experiment.gpParams.m_crossover_probability;
        stream >> data.experiment.gpParams.m_mutation_probability;
        stream >> data.experiment.gpParams.m_clone_probability;
        stream >> data.experiment.gpParams.m_number_of_runs;
        stream >> data.experiment.gpParams.m_trainingRatio;
        break;
      }
    case SECTION_POPULATION:
      {
        int number_of_runs;
        int number_of_generations;
        int number_of_individuals;
        stream >> number_of_runs;
        stream >> number_of_generations;
        stream >> number_of_individuals;
        data.experiment.population.clear();
        gpExperiment::popData popTemp;
        for(int run = 0;run < number_of_runs;run++)
        {
          QVector<QVector<GP::treeStruct> > popTrees;
          QVector<QVector<int> > popParents[2];
          QVector<QVector<int> > popSize;
          QVector<QVector<int> > popOperator;
          QVector<QVector<float> > popNormalized;
          QVector<QVector<float> > popReal;
          QVector<QVector<int> > popId;
          popTrees.clear();
          popParents[0].clear();
          popParents[1].clear();
          popSize.clear();
          popOperator.clear();
          popNormalized.clear();
          popReal.clear();
          popId.clear();
          for(int gen = 0;gen < number_of_generations;gen++)
          {
            QVector<GP::treeStruct> indsTrees;
            QVector<int> indsParents[2];
            QVector<int> indsSize;
            QVector<int> indsOperator;
            QVector<float> indsNormalized;
            QVector<float> indsReal;
            QVector<int> indsId;
            indsTrees.clear();
            indsParents[0].clear();
            indsParents[1].clear();
            indsSize.clear();
            indsOperator.clear();
            indsNormalized.clear();
            indsReal.clear();
            indsId.clear();
            for(int ind = 0;ind < number_of_individuals;ind++)
            {
              GP::treeStruct treeTemp;
              int parentsTemp[2];
              int sizeTemp;
              int operatorTemp;
              float normalizedTemp;
              float realTemp;
              int idTemp;
              stream >> treeTemp.name;
              stream >> treeTemp.arity;
              stream >> treeTemp.subTreeSize;
              stream >> treeTemp.posX;
              stream >> treeTemp.posY;
              stream >> treeTemp.index;
              stream >> treeTemp.syntaxPrefix;
              stream >> parentsTemp[0];
              stream >> parentsTemp[1];
              stream >> sizeTemp;
              stream >> operatorTemp;
              stream >> normalizedTemp;
              stream >> realTemp;
              stream >> idTemp;
              indsTrees.push_back(treeTemp);
              indsParents[0].push_back(parentsTemp[0]);
              indsParents[1].push_back(parentsTemp[1]);
              indsSize.push_back(sizeTemp);
              indsOperator.push_back(operatorTemp);
              indsNormalized.push_back(normalizedTemp);
              indsReal.push_back(realTemp);
              indsId.push_back(idTemp);
            }
            popTrees.push_back(indsTrees);
            popParents[0].push_back(indsParents[0]);
            popParents[1].push_back(indsParents[1]);
            popSize.push_back(indsSize);
            popOperator.push_back(indsOperator);
            popNormalized.push_back(indsNormalized);
            popReal.push_back(indsReal);
            popId.push_back(indsId);
          }
          stream >> popTemp.bestNormalizedTrainingFitness;
          stream >> popTemp.bestRealTrainingFitness;
          stream >> popTemp.bestSize;
          stream >> popTemp.bestNormalizedTestingFitness;
          stream >> popTemp.bestRealTestingFitness;
          stream >> popTemp.avgSize;
          stream >> popTemp.expectedOutput;
          stream >> popTemp.actualOutput;
          popTemp.tree = popTrees;
          popTemp.parents[0] = popParents[0];
          popTemp.parents[1] = popParents[1];
          popTemp.size = popSize;
          popTemp.operatorType = popOperator;
          popTemp.normalizedFitness = popNormalized;
          popTemp.realFitness = popReal;
          popTemp.id = popId;
          data.experiment.population.push_back(popTemp);
        }
        break;
      }
    case SECTION_SUMMARY:
      {
        stream >> data.experiment.dataX;
        stream >> data.experiment.dataY;
        stream >> data.experiment.trainingPartitionSize;
        stream >> data.experiment.testingPartitionSize;
        stream >> data.experiment.filename;
        stream >> data.experiment.averageSize;
        stream >> data.experiment.averageTrainingFitness;
        stream >> data.experiment.averageTestingFitness;
        stream >> data.experiment.medianTrainingFitness;
        stream >> data.experiment.medianTestingFitness;
        break;
      }
  }
  return stream;
}
