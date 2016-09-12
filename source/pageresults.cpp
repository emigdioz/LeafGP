/*****************************************************************************
 * pageresults
 *
 * Created: 8/23/2016 2016 by emigdio
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

#include "mainwidget.h"
#include "ui_mainwidget.h"

void MainWidget::drawBoxplots()
{
  math_stats localTool;
  double Q1train, Q2train, Q3train, maxtrain, mintrain,
      Q1test, Q2test, Q3test, maxtest, mintest,
      Q1avgsize, Q2avgsize, Q3avgsize, maxavgsize, minavgsize,
      Q1bestsize, Q2bestsize, Q3bestsize, maxbestsize, minbestsize;
  double min1, max1,min2,max2;
  QVector<double> outliersBestTrain;
  QVector<double> outliersBestTest;
  std::vector<double> bestTrain;
  std::vector<double> bestTest;
  QVector<double> outliersAvgSize;
  QVector<double> outliersBestSize;
  std::vector<double> averageSize;
  std::vector<double> betterSize;
  for(int i = 0;i < workerAlgorithm->gp_parameters.m_number_of_runs;i++)
  {
    bestTrain.push_back(userExperiment.population.at(i).bestRealTrainingFitness.at(workerAlgorithm->gp_parameters.m_number_of_generations-1));
    bestTest.push_back(userExperiment.population.at(i).bestRealTestingFitness.at(workerAlgorithm->gp_parameters.m_number_of_generations-1));
    averageSize.push_back(userExperiment.averageSize.at(i));
    betterSize.push_back(userExperiment.population.at(i).bestSize.at(workerAlgorithm->gp_parameters.m_number_of_generations-1));
  }
  localTool.computeQuartiles(bestTrain,Q1train, Q2train, Q3train, mintrain, maxtrain, outliersBestTrain);
  localTool.computeQuartiles(bestTest,Q1test, Q2test, Q3test, mintest, maxtest, outliersBestTest);
  localTool.computeQuartiles(averageSize,Q1avgsize, Q2avgsize, Q3avgsize, minavgsize, maxavgsize, outliersAvgSize);
  localTool.computeQuartiles(betterSize,Q1bestsize, Q2bestsize, Q3bestsize, minbestsize, maxbestsize, outliersBestSize);
  if (mintrain < mintest)
      min1 = mintrain;
  else
      min1 = mintest;
  if (maxtrain > maxtest)
      max1 = maxtrain;
  else
      max1 = maxtest;
  if (minavgsize < minbestsize)
      min2 = minavgsize;
  else
      min2 = minbestsize;
  if (maxavgsize > maxbestsize)
      max2 = maxavgsize;
  else
      max2 = maxbestsize;
  bestTrainFitness->setKey(1);
  bestTrainFitness->setMinimum(mintrain);
  bestTrainFitness->setLowerQuartile(Q1train);
  bestTrainFitness->setMedian(Q2train);
  bestTrainFitness->setUpperQuartile(Q3train);
  bestTrainFitness->setMaximum(maxtrain);
  bestTrainFitness->setOutliers(outliersBestTrain);
  bestTestFitness->setKey(2);
  bestTestFitness->setMinimum(mintest);
  bestTestFitness->setLowerQuartile(Q1test);
  bestTestFitness->setMedian(Q2test);
  bestTestFitness->setUpperQuartile(Q3test);
  bestTestFitness->setMaximum(maxtest);
  bestTestFitness->setOutliers(outliersBestTest);
  avgSize->setKey(1);
  avgSize->setMinimum(minavgsize);
  avgSize->setLowerQuartile(Q1avgsize);
  avgSize->setMedian(Q2avgsize);
  avgSize->setUpperQuartile(Q3avgsize);
  avgSize->setMaximum(maxavgsize);
  avgSize->setOutliers(outliersAvgSize);
  bestSize->setKey(2);
  bestSize->setMinimum(minbestsize);
  bestSize->setLowerQuartile(Q1bestsize);
  bestSize->setMedian(Q2bestsize);
  bestSize->setUpperQuartile(Q3bestsize);
  bestSize->setMaximum(maxbestsize);
  bestSize->setOutliers(outliersBestSize);
  ui->boxplotQuality->yAxis->setRange(min1 - ((max1 - min1) * 0.2), max1 + ((max1 - min1) * 0.2));
  ui->boxplotQuality->replot();
  ui->boxplotSize->yAxis->setRange(min2 - ((max2 - min2) * 0.2), max2 + ((max2 - min2) * 0.2));
  ui->boxplotSize->replot();
}

void MainWidget::setupBoxplots()
{
  bestTrainFitness = new QCPStatisticalBox(ui->boxplotQuality->xAxis, ui->boxplotQuality->yAxis);
  bestTestFitness = new QCPStatisticalBox(ui->boxplotQuality->xAxis, ui->boxplotQuality->yAxis);
  avgSize = new QCPStatisticalBox(ui->boxplotSize->xAxis, ui->boxplotSize->yAxis);
  bestSize = new QCPStatisticalBox(ui->boxplotSize->xAxis, ui->boxplotSize->yAxis);

  ui->boxplotSize->addPlottable(bestTrainFitness);
  ui->boxplotSize->addPlottable(bestTestFitness);

  QPen pen(QColor(232, 236, 242, 255));
  pen.setWidth(2);
  QColor green1(148, 204, 20, 100);
  QColor orange1(255, 174, 0, 100);
  QColor green2(148, 204, 20, 255);
  QColor orange2(255, 174, 0, 255);
  QColor purple1(188, 95, 211, 100);
  QColor red1(255, 85, 85, 100);
  QColor purple2(188, 95, 211, 255);
  QColor red2(255, 85, 85, 255);
  QBrush boxBrush1(green1);
  QBrush boxBrush2(orange1);
  QPen pen1(green2);
  QPen pen2(orange2);
  pen1.setWidth(2);
  pen2.setWidth(2);
  QCPScatterStyle box1Style;
  QCPScatterStyle box2Style;
  box1Style.setBrush(boxBrush1);
  box1Style.setPen(Qt::NoPen);
  box1Style.setShape(QCPScatterStyle::ssCircle);
  box2Style.setBrush(boxBrush2);
  box2Style.setPen(Qt::NoPen);
  box2Style.setShape(QCPScatterStyle::ssCircle);
  avgSize->setBrush(boxBrush1);
  avgSize->setPen(pen1);
  avgSize->setOutlierStyle(box1Style);
  avgSize->setMedianPen(pen1);
  avgSize->setWhiskerPen(pen1);
  avgSize->setWhiskerBarPen(pen1);
  bestSize->setBrush(boxBrush2);
  bestSize->setPen(pen2);
  bestSize->setOutlierStyle(box2Style);
  bestSize->setMedianPen(pen2);
  bestSize->setWhiskerPen(pen2);
  bestSize->setWhiskerBarPen(pen2);
  ui->boxplotSize->axisRect()->setBackground(QColor(255, 255, 255, 255));

  ui->boxplotSize->xAxis->setSubTickCount(0);
  ui->boxplotSize->xAxis->setTickLength(0, 4);
  ui->boxplotSize->xAxis->setTickLabelRotation(20);
  ui->boxplotSize->xAxis->setAutoTicks(false);
  ui->boxplotSize->xAxis->setAutoTickLabels(false);
  ui->boxplotSize->xAxis->setTickVector(QVector<double>() << 1 << 2);
  ui->boxplotSize->xAxis->setTickVectorLabels(QVector<QString>() << "Average" << "Best");
  ui->boxplotSize->xAxis->scaleRange(0.6, 0.5);
  ui->boxplotSize->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  ui->boxplotSize->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
  ui->boxplotSize->xAxis->setBasePen(QColor(255, 255, 255, 255));
  ui->boxplotSize->xAxis->setTicks(true);
  ui->boxplotSize->xAxis->setTickLabelFont(QFont("Roboto",10,QFont::Light));
  ui->boxplotSize->xAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
  ui->boxplotSize->xAxis->setTickLabelColor(QColor(45,65,102,255));
  ui->boxplotSize->xAxis->setLabelColor(QColor(45,65,102,255));

	ui->boxplotSize->yAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->boxplotSize->yAxis->grid()->setPen(pen);
	ui->boxplotSize->yAxis->setTickLengthIn(0);
	ui->boxplotSize->yAxis->setSubTickLengthIn(0);
	ui->boxplotSize->yAxis->setTickLabels(true);
	ui->boxplotSize->yAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->boxplotSize->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->boxplotSize->yAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->boxplotSize->yAxis->setLabelColor(QColor(45,65,102,255));
	ui->boxplotSize->yAxis->setLabel("Size");
	ui->boxplotSize->yAxis->setRange(0, 7);

  ui->boxplotQuality->addPlottable(bestTrainFitness);
  ui->boxplotQuality->addPlottable(bestTestFitness);
  QBrush boxBrush3(purple1);
  QBrush boxBrush4(red1);
  QPen pen3(purple2);
  QPen pen4(red2);
  pen3.setWidth(2);
  pen4.setWidth(2);
  QCPScatterStyle box3Style;
  QCPScatterStyle box4Style;
  box3Style.setBrush(boxBrush3);
  box3Style.setPen(Qt::NoPen);
  box3Style.setShape(QCPScatterStyle::ssCircle);
  box4Style.setBrush(boxBrush4);
  box4Style.setPen(Qt::NoPen);
  box4Style.setShape(QCPScatterStyle::ssCircle);

  bestTrainFitness->setBrush(boxBrush3);
  bestTrainFitness->setPen(pen3);
  bestTrainFitness->setOutlierStyle(box3Style);
  bestTrainFitness->setMedianPen(pen3);
  bestTrainFitness->setWhiskerPen(pen3);
  bestTrainFitness->setWhiskerBarPen(pen3);
  bestTestFitness->setBrush(boxBrush4);
  bestTestFitness->setPen(pen4);
  bestTestFitness->setOutlierStyle(box4Style);
  bestTestFitness->setMedianPen(pen4);
  bestTestFitness->setWhiskerPen(pen4);
  bestTestFitness->setWhiskerBarPen(pen4);
  ui->boxplotQuality->axisRect()->setBackground(QColor(255, 255, 255, 255));

  ui->boxplotQuality->xAxis->setSubTickCount(0);
  ui->boxplotQuality->xAxis->setTickLength(0, 4);
  ui->boxplotQuality->xAxis->setTickLabelRotation(20);
  ui->boxplotQuality->xAxis->setAutoTicks(false);
  ui->boxplotQuality->xAxis->setAutoTickLabels(false);
  ui->boxplotQuality->xAxis->setTickVector(QVector<double>() << 1 << 2);
  ui->boxplotQuality->xAxis->setTickVectorLabels(QVector<QString>() << "Training" << "Testing");
  ui->boxplotQuality->xAxis->scaleRange(0.6, 0.5);
  ui->boxplotQuality->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  ui->boxplotQuality->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
  ui->boxplotQuality->xAxis->setBasePen(QColor(255, 255, 255, 255));
  ui->boxplotQuality->xAxis->setTicks(true);
  ui->boxplotQuality->xAxis->setTickLabelFont(QFont("Roboto",10,QFont::Light));
  ui->boxplotQuality->xAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
  ui->boxplotQuality->xAxis->setTickLabelColor(QColor(45,65,102,255));
  ui->boxplotQuality->xAxis->setLabelColor(QColor(45,65,102,255));

	ui->boxplotQuality->yAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->boxplotQuality->yAxis->grid()->setPen(pen);
	ui->boxplotQuality->yAxis->setTickLengthIn(0);
	ui->boxplotQuality->yAxis->setSubTickLengthIn(0);
	ui->boxplotQuality->yAxis->setTickLabels(true);
	ui->boxplotQuality->yAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->boxplotQuality->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->boxplotQuality->yAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->boxplotQuality->yAxis->setLabelColor(QColor(45,65,102,255));
	ui->boxplotQuality->yAxis->setLabel("Fitness (RMSE)");
	ui->boxplotQuality->yAxis->setRange(0, 7);
}

void MainWidget::populateRunsSelectionList()
{
	modelRunsList = new QStringListModel(this);
	QStringList listRuns;
	for(int i = 0;i < workerAlgorithm->gp_parameters.m_number_of_runs;i++)
	{
		listRuns.push_back("Run " + QString::number(i + 1));
	}
	modelRunsList->setStringList(listRuns);
	ui->runSelectionView->setModel(modelRunsList);
}

void MainWidget::setupQualityRunPlot()
{
	ui->plotQualityRun->show();
	ui->plotQualityRun->addGraph();
	ui->plotQualityRun->addGraph();
	ui->plotQualityRun->xAxis->setRange(1,workerAlgorithm->gp_parameters.m_number_of_generations);
	ui->plotQualityRun->graph(0)->setName("Best solution error for training data");
	ui->plotQualityRun->graph(0)->setPen(QPen(QColor(255,174,0,255),2));
	ui->plotQualityRun->graph(1)->setName("Best solution error for testing data");
	ui->plotQualityRun->graph(1)->setPen(QPen(QColor(75,143,185,255),2));

	ui->plotQualityRun->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
	ui->plotQualityRun->axisRect()->setBackground(QColor(255, 255, 255, 255));
	ui->plotQualityRun->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->plotQualityRun->xAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->plotQualityRun->yAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->plotQualityRun->xAxis->setTicks(true);
	ui->plotQualityRun->xAxis->setLabel("Generation");
	ui->plotQualityRun->xAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->plotQualityRun->xAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->plotQualityRun->xAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->plotQualityRun->xAxis->setLabelColor(QColor(45,65,102,255));
	ui->plotQualityRun->legend->setVisible(true);
	ui->plotQualityRun->legend->setFont(QFont("Roboto",10,QFont::Light));
	ui->plotQualityRun->legend->setTextColor(QColor(45,65,102,255));

	QPen pen(QColor(232, 236, 242, 255));
	pen.setWidth(2);
	ui->plotQualityRun->yAxis->grid()->setPen(pen);
	ui->plotQualityRun->yAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->plotQualityRun->yAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->plotQualityRun->xAxis->setTickPen(pen);
	ui->plotQualityRun->xAxis->setSubTickPen(QColor(232, 236, 242, 255));
	ui->plotQualityRun->yAxis->setTickLengthIn(0);
	ui->plotQualityRun->yAxis->setSubTickLengthIn(0);
	ui->plotQualityRun->yAxis->setTickLabels(true);
	ui->plotQualityRun->yAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->plotQualityRun->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->plotQualityRun->yAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->plotQualityRun->yAxis->setLabelColor(QColor(45,65,102,255));
	ui->plotQualityRun->yAxis->setLabel("RMSE");
	ui->plotQualityRun->graph(0)->clearData();
	ui->plotQualityRun->graph(1)->clearData();
}

void MainWidget::setupSizeRunPlot()
{
	ui->plotSizeRun->show();
	ui->plotSizeRun->addGraph();
	ui->plotSizeRun->xAxis->setRange(1,workerAlgorithm->gp_parameters.m_number_of_generations);
	ui->plotSizeRun->graph(0)->setName("Solution sizes average");
	ui->plotSizeRun->graph(0)->setPen(QPen(QColor(148,204,20,255),2));
	ui->plotSizeRun->graph(0)->setBrush(QBrush(QColor(148,204,20,20)));

	ui->plotSizeRun->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
	ui->plotSizeRun->axisRect()->setBackground(QColor(255, 255, 255, 255));
	ui->plotSizeRun->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->plotSizeRun->xAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->plotSizeRun->yAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->plotSizeRun->xAxis->setTicks(true);
	ui->plotSizeRun->xAxis->setLabel("Generation");
	ui->plotSizeRun->xAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->plotSizeRun->xAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->plotSizeRun->xAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->plotSizeRun->xAxis->setLabelColor(QColor(45,65,102,255));
	ui->plotSizeRun->legend->setVisible(true);
	ui->plotSizeRun->legend->setFont(QFont("Roboto",10,QFont::Light));
	ui->plotSizeRun->legend->setTextColor(QColor(45,65,102,255));

	QPen pen(QColor(232, 236, 242, 255));
	pen.setWidth(2);
	ui->plotSizeRun->yAxis->grid()->setPen(pen);
	ui->plotSizeRun->yAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->plotSizeRun->yAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->plotSizeRun->xAxis->setTickPen(pen);
	ui->plotSizeRun->xAxis->setSubTickPen(QColor(232, 236, 242, 255));
	ui->plotSizeRun->yAxis->setTickLengthIn(0);
	ui->plotSizeRun->yAxis->setSubTickLengthIn(0);
	ui->plotSizeRun->yAxis->setTickLabels(true);
	ui->plotSizeRun->yAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->plotSizeRun->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->plotSizeRun->yAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->plotSizeRun->yAxis->setLabelColor(QColor(45,65,102,255));
	ui->plotSizeRun->yAxis->setLabel("Size");
	ui->plotSizeRun->graph(0)->clearData();
}

void MainWidget::on_runSelectionView_clicked(const QModelIndex &index)
{
  float yMin,yMax = 0,yMax2 = 0;
  indexRun = index.row();

  QVector<double> x(workerAlgorithm->gp_parameters.m_number_of_generations);
  QVector<double> y1(workerAlgorithm->gp_parameters.m_number_of_generations);
  QVector<double> y2(workerAlgorithm->gp_parameters.m_number_of_generations);
  QVector<double> y3(workerAlgorithm->gp_parameters.m_number_of_generations);
  ui->populationMapRun->clearPopulation();
  ui->plotQualityRun->graph(0)->clearData();
  ui->plotQualityRun->graph(1)->clearData();
  ui->plotSizeRun->graph(0)->clearData();
  for(int i = 0;i < workerAlgorithm->gp_parameters.m_number_of_generations;i++)
  {
    x[i] = i + 1;
    y1[i] = userExperiment.population[indexRun].bestRealTrainingFitness.at(i);
    y2[i] = userExperiment.population[indexRun].bestRealTestingFitness.at(i);
    y3[i] = userExperiment.population[indexRun].avgSize.at(i);
    if(y1[i] > yMax) yMax = y1[i];
    if(y2[i] > yMax) yMax = y2[i];
    if(i == 0)
      yMin = y1[i];
    else
    {
      if(y1[i] < yMin) yMin = y1[i];
      if(y2[i] < yMin) yMin = y2[i];
    }
    if(y3[i] > yMax2) yMax2 = y3[i];
  }
  ui->plotQualityRun->graph(0)->setData(x,y1);
  ui->plotQualityRun->graph(1)->setData(x,y2);
  ui->plotQualityRun->yAxis->setRange(yMin - (0.1 * yMin),yMax + (0.1 * yMax));
  ui->plotQualityRun->replot();
  ui->plotSizeRun->graph(0)->setData(x,y3);
  ui->plotSizeRun->yAxis->setRange(0,yMax2 + (0.1 * yMax2));
  ui->plotSizeRun->replot();

  ui->populationMapRun->addData(userExperiment.population[indexRun]);

  ui->label_145->setText(QString::number(userExperiment.population[indexRun].bestNormalizedTrainingFitness.at(workerAlgorithm->gp_parameters.m_number_of_generations-1)));
  ui->label_154->setText(QString::number(userExperiment.population[indexRun].bestNormalizedTestingFitness.at(workerAlgorithm->gp_parameters.m_number_of_generations-1)));
  ui->label_147->setText(QString::number(userExperiment.averageTrainingFitness.at(indexRun)));
  ui->label_149->setText(QString::number(userExperiment.population[indexRun].bestSize.at(workerAlgorithm->gp_parameters.m_number_of_generations-1)));
  ui->label_151->setText(QString::number(userExperiment.averageSize.at(indexRun)));
}

void MainWidget::individualMapSelected(int &ind, int &gen, int &id)
{
  int unorderedIndex = ind;
  for(int i = 0;i < workerAlgorithm->gp_parameters.m_population_size;i++)
  {
    if(userExperiment.population[indexRun].id.at(gen).at(i) == id)
    {
      unorderedIndex = i;
      qDebug()<<"Found index "<<i;
      break;
    }
  }
  selectedTree = userExperiment.population[indexRun].tree.at(gen).at(unorderedIndex);
  int nLeaves = 0;
  for (int i = 0; i < selectedTree.name.size(); i++) {
      nLeaves = countLeaves(i, nLeaves);
  }
  positionLeaves(0,0,nLeaves);
  positionParents(0,0);
  ui->treePopRun->setData(selectedTree.posX,
                      selectedTree.posY,
                      selectedTree.name,
                      selectedTree.arity);
  ui->treePopRun->setLinkColor(QColor(255,174,0,255));
  ui->treePopRun->setNodeColor(QColor(255,174,0,255));
  ui->treePopRun->setNodeHoverColor(QColor(148,204,20,255));
  ui->label_139->setText(userExperiment.population[indexRun].tree.at(gen).at(unorderedIndex).syntaxPrefix);
  ui->label_152->setText("Solution error (RMSE) = " + QString::number(userExperiment.population[indexRun].realFitness.at(gen).at(unorderedIndex)));
}

void MainWidget::setupPerformancePlot()
{
	for(int i = 0;i < performancePlotSlices;i++)
		ui->performanceEPlot->addGraph();

	QPen pen(QColor(232, 236, 242, 255));
	QPen pen2(QColor(255,174,0,128),2);

	ui->performanceEPlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->performanceEPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
	ui->performanceEPlot->xAxis->setBasePen(pen);
	ui->performanceEPlot->xAxis->setLabel("Generations");
	ui->performanceEPlot->xAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->performanceEPlot->xAxis->setLabelColor(QColor(45,65,102,255));
	ui->performanceEPlot->xAxis->setSubTickPen(QColor(232, 236, 242, 255));
	ui->performanceEPlot->xAxis->setTickPen(pen);
	ui->performanceEPlot->xAxis->setTicks(true);
	ui->performanceEPlot->xAxis->setTickLabels(true);
	ui->performanceEPlot->xAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->performanceEPlot->xAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->performanceEPlot->xAxis->setTickPen(QColor(255, 255, 255, 255));

	QPen pen3(QColor(232, 236, 242, 255));
	pen3.setWidth(2);
	ui->performanceEPlot->yAxis->grid()->setPen(pen3);
	ui->performanceEPlot->yAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->performanceEPlot->yAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->performanceEPlot->yAxis->setBasePen(pen);
	ui->performanceEPlot->yAxis->setTickLengthIn(0);
	ui->performanceEPlot->yAxis->setSubTickLengthIn(0);
	ui->performanceEPlot->yAxis->setTickLabels(true);
	ui->performanceEPlot->yAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->performanceEPlot->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->performanceEPlot->yAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->performanceEPlot->yAxis->setLabelColor(QColor(45,65,102,255));
	ui->performanceEPlot->yAxis->setLabel("Size");

	ui->performanceEPlot->yAxis->setTickPen(QColor(255, 255, 255, 255));
}

void MainWidget::drawPerformancePlot()
{
	QColor sliceColor;
	double X,Y;
	float startSlice,endSlice;
	int maxY = 0;
	float maxFitness = 0;
	float alphaValue = (128 * exp(-(float)workerAlgorithm->gp_parameters.m_number_of_runs/5));
	QVector<QCPScatterStyle> qqScatter;
	QCPScatterStyle shape;
	for(int i = 0;i < performancePlotSlices;i++)
		ui->performanceEPlot->graph(i)->clearData();
	for(int i = 0;i < performancePlotSlices;i++)
	{
		sliceColor = QColor::fromHslF((float)i/performancePlotSlices, 0.95, 0.5);
		sliceColor.setAlpha(alphaValue);
		shape.setShape(QCPScatterStyle::ssCircle);
		shape.setPen(Qt::NoPen);
		shape.setBrush(sliceColor);
		shape.setSize(6);
		qqScatter.push_back(shape);
		ui->performanceEPlot->graph(i)->setLineStyle(QCPGraph::lsNone);
		ui->performanceEPlot->graph(i)->setScatterStyle(qqScatter[i]);
	}
	for(int iRun = 0;iRun < workerAlgorithm->gp_parameters.m_number_of_runs;iRun++)
	{
		for(int iGen = 0;iGen < workerAlgorithm->gp_parameters.m_number_of_generations;iGen++)
		{
			for(int iInd = 0;iInd < workerAlgorithm->gp_parameters.m_population_size;iInd++)
			{
				if(userExperiment.population[iRun].normalizedFitness.at(iGen).at(iInd) > maxFitness)
					maxFitness = userExperiment.population[iRun].normalizedFitness.at(iGen).at(iInd);
			}
		}
	}

	for(int iRun = 0;iRun < workerAlgorithm->gp_parameters.m_number_of_runs;iRun++)
	{
		for(int iGen = 0;iGen < workerAlgorithm->gp_parameters.m_number_of_generations;iGen++)
		{
			for(int iInd = 0;iInd < workerAlgorithm->gp_parameters.m_population_size;iInd++)
			{
				for(int iSlice = 0;iSlice < performancePlotSlices;iSlice++)
				{
					startSlice = ((float)iSlice/performancePlotSlices)*maxFitness;
					endSlice = ((float)(iSlice + 1)/performancePlotSlices)*maxFitness;
					if(userExperiment.population[iRun].normalizedFitness.at(iGen).at(iInd) > startSlice &&
						 userExperiment.population[iRun].normalizedFitness.at(iGen).at(iInd) <= endSlice)
					{
						X = iGen;
						Y = userExperiment.population[iRun].size.at(iGen).at(iInd);
						ui->performanceEPlot->graph(iSlice)->addData(X,Y);
					}
				}
				if(userExperiment.population[iRun].size.at(iGen).at(iInd) > maxY)
					maxY = userExperiment.population[iRun].size.at(iGen).at(iInd);
			}
		}
	}
	ui->performanceEPlot->xAxis->setRange(1,workerAlgorithm->gp_parameters.m_number_of_generations);
	ui->performanceEPlot->yAxis->setRange(0,maxY + (0.1 * maxY));
	ui->performanceEPlot->replot();
}

void MainWidget::on_pushButton_9_clicked()
{
  QPrinter printer(QPrinter::HighResolution); //create your QPrinter (don't need to be high resolution, anyway)
  printer.setPageSize(QPrinter::Letter);
  printer.setOrientation(QPrinter::Portrait);
  printer.setPageMargins (15,15,15,15,QPrinter::Millimeter);
  printer.setFullPage(false);
  printer.setOutputFileName("output.pdf");
  printer.setOutputFormat(QPrinter::PdfFormat); //you can use native format of system usin QPrinter::NativeFormat

  int width = printer.pageRect().width();
  int height = printer.pageRect().height();
  QImage image(":/icons/resources/images/leafgplogo.png");
  int logoWidth = image.width();
  int logoHeight = image.height();
  float ratioLogo = (float)logoHeight/logoWidth;
  QRectF source(image.rect());
  QRectF target(width/20,200,width/5,(width/5)*ratioLogo);

  QPainter painter(&printer); // create a painter which will paint 'on printer'.
  painter.fillRect(0,0,width,height/10,QBrush(QColor(45,65,102,255)));
  painter.drawImage(target,image,source);
  painter.setFont(QFont("Roboto",18,QFont::Light));
  painter.setPen(QPen(Qt::white));
  painter.drawText(width/2,1000,"Report");
  painter.end();
  qDebug()<<width<<" x "<<height;
}

void MainWidget::on_pushButton_12_clicked()
{
  QFileDialog fileDialog;
  fileDialog.setDefaultSuffix("lgp");
  projectFilename = fileDialog.getSaveFileName(this,tr("Browse..."), "", tr("LeafGP project files (*.lgp)"));
  if(!projectFilename.isEmpty())
  {
    QFileInfo info(projectFilename);
    if(info.completeSuffix().isEmpty())
      projectFilename = projectFilename + ".lgp";
    if(info.completeSuffix().compare("lgp") != 0)
      projectFilename = info.path() + "/" + info.baseName() + ".lgp";
    ui->lineEdit_9->setText(projectFilename);
  }
}

void MainWidget::on_pushButton_13_clicked()
{
  QMessageBox msgBox;
  //validDataProject = true;
  if(validDataProject)
  {
    if(!projectFilename.isEmpty())
    {
      if(ui->checkBox_11->isChecked() || ui->checkBox_12->isChecked() || ui->checkBox_13->isChecked() || ui->checkBox_14->isChecked())
      {
        projectFile.listSections.clear();
        projectFile.insertHeader(0xA0B0C0D0);
        projectFile.insertVersion(501);
        if(ui->checkBox_11->isChecked())
          projectFile.insertPopulation(userExperiment);
        if(ui->checkBox_12->isChecked())
          populateDataForContainer();
        if(ui->checkBox_13->isChecked())
          projectFile.insertSettings(userExperiment);
        if(ui->checkBox_14->isChecked())
          projectFile.insertSummary(userExperiment);
        projectFile.write(projectFilename);
      }
      else
      {
        msgBox.setText("At least one element has to be selected.");
        msgBox.exec();
      }
    }
    else
    {
      msgBox.setText("Please input a file name.");
      msgBox.exec();
    }
  }
  else
  {
    msgBox.setText("No data has been processed.");
    msgBox.exec();
  }
}
