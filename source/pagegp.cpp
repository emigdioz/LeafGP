#include "mainwidget.h"
#include "ui_mainwidget.h"

void MainWidget::requestAlgorithmStart()
{
	workerAlgorithm->abort();
	threadGP->wait();
	workerAlgorithm->requestWork();
}

void MainWidget::on_startButton_clicked()
{
	ui->qualityPlot->graph(0)->clearData();
	ui->qualityPlot->graph(1)->clearData();
	ui->sizePlot->graph(0)->clearData();
	// clear data vectors and preallocate space
	userExperiment.nodeEvaluations.clear();
	userExperiment.objectiveEvaluations.clear();
	userExperiment.averageSize.clear();
	userExperiment.averageTrainingFitness.clear();
	userExperiment.averageTestingFitness.clear();
	userExperiment.medianTestingFitness.clear();
	userExperiment.medianTrainingFitness.clear();
	userExperiment.population.clear();
	gpExperiment::popData emptyPop;

	for(int i = 0;i < workerAlgorithm->gp_parameters.m_number_of_runs;i++)
		userExperiment.population.push_back(emptyPop);

//	userExperiment.nodeEvaluations.fill(-1,userExperiment.gpParams.m_number_of_runs);
//	userExperiment.objectiveEvaluations.fill(-1,userExperiment.gpParams.m_number_of_runs);
//	userExperiment.averageSize.fill(-1,userExperiment.gpParams.m_number_of_runs);
//	userExperiment.averageTrainingFitness.fill(-1,userExperiment.gpParams.m_number_of_runs);
//	userExperiment.averageTestingFitness.fill(-1,userExperiment.gpParams.m_number_of_runs);
//	userExperiment.medianTrainingFitness.fill(-1,userExperiment.gpParams.m_number_of_runs);
//	userExperiment.medianTestingFitness.fill(-1,userExperiment.gpParams.m_number_of_runs);
//	//userExperiment.population.fill(userExperiment.gpParams.m_number_of_runs);
//	userExperiment.bestSize.fill(-1,userExperiment.gpParams.m_number_of_runs);
//	userExperiment.bestTrainingFitness.fill(-1,userExperiment.gpParams.m_number_of_runs);
//	userExperiment.bestTestingFitness.fill(-1,userExperiment.gpParams.m_number_of_runs);
	// Copy input data to worker local matrix
	workerAlgorithm->data_matrix = input_data_matrix;
	requestAlgorithmStart();
	showStartedTime();
	startedDateTime = QDateTime::currentDateTime();
	timerGP->start(1000);
}

void MainWidget::receivedRunProgress(int value, int run)
{	
	int max_runs = workerAlgorithm->gp_parameters.m_number_of_runs;
	if(max_runs > 1)
	{
		int total_progress = ((1000.0/max_runs) * run) + ((float)value/max_runs);
		ui->circularProgress->setProgress1(total_progress);
		ui->circularProgress->setProgress2(value);
	}
	else
		ui->circularProgress->setProgress1(value);

	ui->label_82->setText("Run " + QString::number(run + 1) + "/" + QString::number(max_runs));
}

QString MainWidget::secondsToDHMS(qint64 duration)
{
    QString res;
    int seconds = (int)(duration % 60);
    duration /= 60;
    int minutes = (int)(duration % 60);
    duration /= 60;
    int hours = (int)(duration % 24);
    int days = (int)(duration / 24);
    if ((hours == 0) && (days == 0) && (minutes == 0))
        return res.sprintf("%02d secs", seconds);
    if ((hours == 0) && (days == 0))
        return res.sprintf("%02d mins %02d secs", minutes, seconds);
    if (days == 0)
        return res.sprintf("%02d hrs %02d mins %02d secs", hours, minutes, seconds);
    return res.sprintf("%dd days %02d hrs %02d mins %02d secs", days, hours, minutes, seconds);
}

void MainWidget::showElapsedTime()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    qint64 seconds = startedDateTime.secsTo(currentDateTime);

    ui->label_86->setText(secondsToDHMS(seconds));
    ui->label_86->update();
}

void MainWidget::showStartedTime()
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm");
    ui->label_84->setText(text);
}

void MainWidget::algorithmFinished()
{
  timerGP->stop();
  if(workerAlgorithm->gp_parameters.m_number_of_runs > 1)
    drawBoxplots();
}

void MainWidget::drawCorrelationPlotGP(QVector<double> actualY, QVector<double> expectedY)
{
	ui->correlationGPPlot->show();
	QVector<double> xReg,yReg;
	double a,b,coeffReg,pearson,minX,maxX,minY,maxY;
	QVector<double> YTickVector;
	QVector<QString> YTickLabels;
	QVector<double> XTickVector;
	QVector<QString> XTickLabels;

	QCPScatterStyle correlationScatter;
	correlationScatter.setShape(QCPScatterStyle::ssCircle);
	correlationScatter.setPen(Qt::NoPen);
	correlationScatter.setBrush(QColor(255,174,0,128));
	correlationScatter.setSize(6);
	xReg.clear();
	yReg.clear();
	minX = maxX = expectedY.at(0);
	minY = maxY = actualY.at(0);
	for(int i = 0;i < actualY.size();i++)
	{
		if(expectedY.at(i) < minX) minX = expectedY.at(i);
		if(expectedY.at(i) > maxX) maxX = expectedY.at(i);
		if(actualY.at(i) < minY) minY = actualY.at(i);
		if(actualY.at(i) > maxY) maxY = actualY.at(i);
	}
	double marginX = (maxX - minX) * 0.1;
	double marginY = (maxY - minY) * 0.1;

	xReg.push_back(minX);
	xReg.push_back(maxX);
	yReg.push_back(minY);
	yReg.push_back(maxY);
	XTickVector.push_back(minX);
	XTickVector.push_back(maxX);
	XTickLabels.push_back(QString::number(minX,'f',2));
	XTickLabels.push_back(QString::number(maxX,'f',2));
	YTickVector.push_back(minY);
	YTickVector.push_back(maxY);
	YTickLabels.push_back(QString::number(minY,'f',2));
	YTickLabels.push_back(QString::number(maxY,'f',2));

	// compute Pearson coefficient
	pearson = basicStats.computePearson(actualY,expectedY);
	ui->label_99->setText("Correlation performance (ρ = " + QString::number(pearson,'f',4) + ")");

	QPen pen(QColor(Qt::white),2);
	QPen pen2(QColor(188,95,211,255),1);

	ui->correlationGPPlot->clearPlottables();
	ui->correlationGPPlot->addGraph();
	ui->correlationGPPlot->addGraph();
	ui->correlationGPPlot->xAxis->setRange(minX - marginX,maxX + marginX);
	ui->correlationGPPlot->yAxis->setRange(minY - marginY,maxY + marginY);
	ui->correlationGPPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
	ui->correlationGPPlot->graph(0)->setScatterStyle(correlationScatter);
	ui->correlationGPPlot->graph(1)->setPen(pen2);

	ui->correlationGPPlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->correlationGPPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
	ui->correlationGPPlot->xAxis->setBasePen(pen);
	ui->correlationGPPlot->xAxis->setLabel("Expected");
	ui->correlationGPPlot->xAxis->setLabelFont(QFont("Roboto",11,QFont::Light));
	ui->correlationGPPlot->xAxis->setLabelColor(QColor(45,65,102,255));
	ui->correlationGPPlot->xAxis->setAutoTicks(false);
	ui->correlationGPPlot->xAxis->setAutoTickLabels(false);
	ui->correlationGPPlot->xAxis->setTicks(true);
	ui->correlationGPPlot->xAxis->setTickLabels(true);
	ui->correlationGPPlot->xAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
	ui->correlationGPPlot->xAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->correlationGPPlot->xAxis->setTickVector(XTickVector);
	ui->correlationGPPlot->xAxis->setTickVectorLabels(XTickLabels);
	ui->correlationGPPlot->xAxis->setTickPen(QColor(255, 255, 255, 255));

	ui->correlationGPPlot->yAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->correlationGPPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
	ui->correlationGPPlot->yAxis->setBasePen(pen);
	ui->correlationGPPlot->yAxis->setLabel("Actual");
	ui->correlationGPPlot->yAxis->setLabelFont(QFont("Roboto",11,QFont::Light));
	ui->correlationGPPlot->yAxis->setLabelColor(QColor(45,65,102,255));
	ui->correlationGPPlot->yAxis->setAutoTicks(false);
	ui->correlationGPPlot->yAxis->setAutoTickLabels(false);
	ui->correlationGPPlot->yAxis->setTicks(true);
	ui->correlationGPPlot->yAxis->setTickLabels(true);
	ui->correlationGPPlot->yAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
	ui->correlationGPPlot->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->correlationGPPlot->yAxis->setTickVector(YTickVector);
	ui->correlationGPPlot->yAxis->setTickVectorLabels(YTickLabels);
	ui->correlationGPPlot->yAxis->setTickPen(QColor(255, 255, 255, 255));

	ui->correlationGPPlot->graph(0)->clearData();
	ui->correlationGPPlot->graph(0)->setData(expectedY,actualY);
	ui->correlationGPPlot->graph(1)->clearData();
	ui->correlationGPPlot->graph(1)->setData(xReg,yReg);
	ui->correlationGPPlot->replot();

}

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

void MainWidget::setupQualityPlot()
{
	ui->qualityPlot->show();
	ui->qualityPlot->addGraph();
	ui->qualityPlot->addGraph();
	ui->qualityPlot->xAxis->setRange(1,workerAlgorithm->gp_parameters.m_number_of_generations);
	ui->qualityPlot->graph(0)->setName("Best solution error for training data");
	ui->qualityPlot->graph(0)->setPen(QPen(QColor(255,174,0,255),2));
	ui->qualityPlot->graph(1)->setName("Best solution error for testing data");
	ui->qualityPlot->graph(1)->setPen(QPen(QColor(75,143,185,255),2));

	ui->qualityPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
	ui->qualityPlot->axisRect()->setBackground(QColor(255, 255, 255, 255));
	ui->qualityPlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->qualityPlot->xAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->qualityPlot->yAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->qualityPlot->xAxis->setTicks(true);
	ui->qualityPlot->xAxis->setLabel("Generation");
	ui->qualityPlot->xAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->qualityPlot->xAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->qualityPlot->xAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->qualityPlot->xAxis->setLabelColor(QColor(45,65,102,255));
	ui->qualityPlot->legend->setVisible(true);
	ui->qualityPlot->legend->setFont(QFont("Roboto",10,QFont::Light));
	ui->qualityPlot->legend->setTextColor(QColor(45,65,102,255));

	QPen pen(QColor(232, 236, 242, 255));
	pen.setWidth(2);
	ui->qualityPlot->yAxis->grid()->setPen(pen);
	ui->qualityPlot->yAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->qualityPlot->yAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->qualityPlot->xAxis->setTickPen(pen);
	ui->qualityPlot->xAxis->setSubTickPen(QColor(232, 236, 242, 255));
	ui->qualityPlot->yAxis->setTickLengthIn(0);
	ui->qualityPlot->yAxis->setSubTickLengthIn(0);
	ui->qualityPlot->yAxis->setTickLabels(true);
	ui->qualityPlot->yAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->qualityPlot->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->qualityPlot->yAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->qualityPlot->yAxis->setLabelColor(QColor(45,65,102,255));
	ui->qualityPlot->yAxis->setLabel("RMSE");
	ui->qualityPlot->graph(0)->clearData();
	ui->qualityPlot->graph(1)->clearData();
}

void MainWidget::setupSizePlot()
{
	ui->sizePlot->show();
	ui->sizePlot->addGraph();
	ui->sizePlot->xAxis->setRange(1,workerAlgorithm->gp_parameters.m_number_of_generations);
	ui->sizePlot->graph(0)->setName("Solution sizes average");
	ui->sizePlot->graph(0)->setPen(QPen(QColor(148,204,20,255),2));
	ui->sizePlot->graph(0)->setBrush(QBrush(QColor(148,204,20,20)));

	ui->sizePlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
	ui->sizePlot->axisRect()->setBackground(QColor(255, 255, 255, 255));
	ui->sizePlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->sizePlot->xAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->sizePlot->yAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->sizePlot->xAxis->setTicks(true);
	ui->sizePlot->xAxis->setLabel("Generation");
	ui->sizePlot->xAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->sizePlot->xAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->sizePlot->xAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->sizePlot->xAxis->setLabelColor(QColor(45,65,102,255));
	ui->sizePlot->legend->setVisible(true);
	ui->sizePlot->legend->setFont(QFont("Roboto",10,QFont::Light));
	ui->sizePlot->legend->setTextColor(QColor(45,65,102,255));

	QPen pen(QColor(232, 236, 242, 255));
	pen.setWidth(2);
	ui->sizePlot->yAxis->grid()->setPen(pen);
	ui->sizePlot->yAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->sizePlot->yAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->sizePlot->xAxis->setTickPen(pen);
	ui->sizePlot->xAxis->setSubTickPen(QColor(232, 236, 242, 255));
	ui->sizePlot->yAxis->setTickLengthIn(0);
	ui->sizePlot->yAxis->setSubTickLengthIn(0);
	ui->sizePlot->yAxis->setTickLabels(true);
	ui->sizePlot->yAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->sizePlot->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->sizePlot->yAxis->setLabelFont(QFont("Roboto",10,QFont::Light));
	ui->sizePlot->yAxis->setLabelColor(QColor(45,65,102,255));
	ui->sizePlot->yAxis->setLabel("Size");
	ui->sizePlot->graph(0)->clearData();
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

void MainWidget::receivedSingleTree(GP::treeStruct data)
{
  selectedTree = data;
  int nLeaves = 0;
  for (int i = 0; i < selectedTree.name.size(); i++) {
      nLeaves = countLeaves(i, nLeaves);
  }
  positionLeaves(0,0,nLeaves);
  positionParents(0,0);
  ui->expressTreeWidget->setData(selectedTree.posX,
                      selectedTree.posY,
                      selectedTree.name,
                      selectedTree.arity);
  ui->expressTreeWidget->setLinkColor(QColor(255,174,0,255));
  ui->expressTreeWidget->setNodeColor(QColor(255,174,0,255));
  ui->expressTreeWidget->setNodeHoverColor(QColor(148,204,20,255));
  ui->label_103->setText(data.syntaxPrefix);
  //qDebug()<<nLeaves;
}

int MainWidget::countLeaves(int index, int count)
{
  if (selectedTree.subTreeSize[index] == 1) {
      selectedTree.index[index] = count;
      count += 1;
  }
  return count;
}

void MainWidget::positionLeaves(int index, int depth, int numberLeaves)
{
  if (selectedTree.subTreeSize[index] == 1) {
      selectedTree.posX[index] = (float)selectedTree.index[index] / (numberLeaves - 1);
      selectedTree.posY[index] = depth;
  }
  unsigned int j = index + 1;
  for (int i = 0; i < selectedTree.arity[index]; i++) {
      positionLeaves(j, depth + 1, numberLeaves);
      j += selectedTree.subTreeSize[j];
  }
}

void MainWidget::positionParents(int index, int depth)
{
  unsigned int j = index + 1;
  for (int i = 0; i < selectedTree.arity[index]; i++) {
      positionParents(j, depth + 1);
      j += selectedTree.subTreeSize[j];
  }
  if (selectedTree.subTreeSize[index] > 1) {
      float x = 0;
      int counter = 0;
      for (int k = 0; k < selectedTree.arity[index]; k++) {
          do // Search for childrens
          {
              if (selectedTree.posY[index + counter] == (depth + 1)) {
                  x += selectedTree.posX[index + counter];
                  counter += 1;
                  break;
              }
              counter += 1;
          } while ((index + counter) < selectedTree.name.size());
      }
      selectedTree.posX[index] = x / selectedTree.arity[index];
      selectedTree.posY[index] = depth;
  }
}

void MainWidget::receivedBasicInfo(GP::basicInfo info)
{
	float yMax = ((info.maxError - info.minError) * 0.1) + info.maxError;
	float yMin = info.minError - ((info.maxError - info.minError) * 0.1);
	float yMaxSize = (info.maxAvgSize * 0.1) + info.maxAvgSize;
	ui->label_88->setText(QString::number(info.currentGeneration));
	ui->label_92->setText(QString::number(info.currentNodesExecutions));
	ui->label_94->setText(QString::number(info.bestTrainError));
	ui->label_96->setText(QString::number(info.bestSize));
	ui->label_98->setText(QString::number(info.avgSize));
	drawCorrelationPlotGP(info.actual,info.expected);
	if(info.currentGeneration == 1)
	{
		ui->qualityPlot->graph(0)->clearData();
		ui->qualityPlot->graph(1)->clearData();
		ui->sizePlot->graph(0)->clearData();
	}
	ui->qualityPlot->graph(0)->addData(info.currentGeneration,info.bestTrainError);
	ui->qualityPlot->graph(1)->addData(info.currentGeneration,info.bestTestError);
	ui->qualityPlot->yAxis->setRange(yMin,yMax);
	ui->qualityPlot->xAxis->setRange(1,workerAlgorithm->gp_parameters.m_number_of_generations);
	ui->qualityPlot->replot();
	ui->sizePlot->graph(0)->addData(info.currentGeneration,info.avgSize);
	ui->sizePlot->yAxis->setRange(-1,yMaxSize);
	ui->sizePlot->xAxis->setRange(1,workerAlgorithm->gp_parameters.m_number_of_generations);
	ui->sizePlot->replot();
	if(info.currentGeneration == workerAlgorithm->gp_parameters.m_number_of_generations)
	{
		userExperiment.nodeEvaluations.push_back(info.currentNodesExecutions);
		userExperiment.averageSize.push_back(info.avgSize);
		userExperiment.averageTrainingFitness.push_back(info.avgTrainError);
		userExperiment.medianTrainingFitness.push_back(info.medianTrainError);
	}
}

void MainWidget::receivedPopInfo(GP::popInfo info)
{
	std::vector<int> temp = info.id;
	if(info.currentGen == 1)
	{
		ui->populationMap->clearPopulation();
	}
	ui->populationMap->addSingleGeneration(info);
	currPop.id.push_back(QVector<int>::fromStdVector(info.id));
	currPop.operatorType.push_back(QVector<int>::fromStdVector(info.operatorT));
	currPop.parents[0].push_back(QVector<int>::fromStdVector(info.parents[0]));
	currPop.parents[1].push_back(QVector<int>::fromStdVector(info.parents[1]));
	currPop.size.push_back(QVector<int>::fromStdVector(info.size));
	currPop.normalizedFitness.push_back(QVector<float>::fromStdVector(info.normFitness));
	currPop.realFitness.push_back(QVector<float>::fromStdVector(info.realFitness));
	currPop.tree.push_back(QVector<GP::treeStruct>::fromStdVector(info.trees));
	currPop.bestNormalizedTrainingFitness.push_back(info.bestNormalizedTrainingFitness);
	currPop.bestRealTrainingFitness.push_back(info.bestRealTrainingFitness);
	currPop.bestSize.push_back(info.bestSize);
	currPop.bestNormalizedTestingFitness.push_back(info.bestNormalizedTestingFitness);
	currPop.bestRealTestingFitness.push_back(info.bestRealTestingFitness);

	if(info.currentGen == workerAlgorithm->gp_parameters.m_number_of_generations)	
	{
		userExperiment.population[info.currentRun] = currPop;
		currPop.id.clear();
		currPop.operatorType.clear();
		currPop.parents[0].clear();
		currPop.parents[1].clear();
		currPop.size.clear();
		currPop.normalizedFitness.clear();
		currPop.realFitness.clear();
		currPop.tree.clear();
		currPop.bestNormalizedTrainingFitness.clear();
		currPop.bestRealTrainingFitness.clear();
		currPop.bestSize.clear();
		currPop.bestNormalizedTestingFitness.clear();
		currPop.bestRealTestingFitness.clear();
	}
}

QPixmap MainWidget::drawSmallCircle(QColor color,int size)
{
	QPixmap pixmap(size,size);
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(0,0,10,10,QBrush(QColor(246,248,250,255)));
	painter.setPen(Qt::NoPen);
	painter.setBrush(color);
	painter.drawEllipse(0,0,size,size);
	painter.end();
	return pixmap;
}

QPixmap MainWidget::drawGradient(int width, int height)
{
	QLinearGradient linearGradient(0,0,width,0);
	linearGradient.setColorAt(0, QColor::fromHslF(0,0.95,0.5));
	linearGradient.setColorAt(0.125, QColor::fromHslF(0.1,0.95,0.5));
	linearGradient.setColorAt(0.25, QColor::fromHslF(0.2,0.95,0.5));
	linearGradient.setColorAt(0.375, QColor::fromHslF(0.3,0.95,0.5));
	linearGradient.setColorAt(0.5, QColor::fromHslF(0.4,0.95,0.5));
	linearGradient.setColorAt(0.625, QColor::fromHslF(0.5,0.95,0.5));
	linearGradient.setColorAt(0.75, QColor::fromHslF(0.6,0.95,0.5));
	linearGradient.setColorAt(0.875, QColor::fromHslF(0.7,0.95,0.5));
	linearGradient.setColorAt(1, QColor::fromHslF(0.8,0.95,0.5));
	QPixmap pixmap(width,height);
	QPainter painter(&pixmap);
	painter.fillRect(0,0,width,height,linearGradient);
	painter.end();
	return pixmap;
}

void MainWidget::on_comboBox_7_currentIndexChanged(int index)
{
  switch (index) {
    case 0:
      ui->label_120->show();
      ui->label_122->show();
      ui->label_123->show();
      ui->label_124->show();
      ui->label_110->hide();
      ui->label_111->hide();
      ui->label_112->hide();
      ui->label_113->hide();
      ui->label_114->hide();
      ui->label_115->hide();
      ui->label_116->hide();
      ui->label_117->hide();
      ui->label_118->hide();
      ui->label_119->hide();
      ui->label_126->hide();
      ui->label_127->hide();
      ui->label_128->hide();
      ui->label_129->hide();
      break;
    case 1:
      ui->label_120->hide();
      ui->label_122->hide();
      ui->label_123->hide();
      ui->label_124->hide();
      ui->label_110->show();
      ui->label_111->show();
      ui->label_112->show();
      ui->label_113->show();
      ui->label_114->show();
      ui->label_115->show();
      ui->label_116->show();
      ui->label_117->show();
      ui->label_118->show();
      ui->label_119->show();
      ui->label_126->hide();
      ui->label_127->hide();
      ui->label_128->hide();
      ui->label_129->hide();
      break;
    case 2:
      ui->label_120->hide();
      ui->label_122->hide();
      ui->label_123->hide();
      ui->label_124->hide();
      ui->label_110->hide();
      ui->label_111->hide();
      ui->label_112->hide();
      ui->label_113->hide();
      ui->label_114->hide();
      ui->label_115->hide();
      ui->label_116->hide();
      ui->label_117->hide();
      ui->label_118->hide();
      ui->label_119->hide();
      ui->label_126->show();
      ui->label_127->show();
      ui->label_128->show();
      ui->label_129->show();
  }
}
