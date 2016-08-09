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
