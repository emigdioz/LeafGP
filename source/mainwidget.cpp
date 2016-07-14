#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QDebug>

MainWidget::MainWidget(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWidget)
{
	ui->setupUi(this);
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Lato-Light.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Lato-Medium.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Roboto-Light.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Roboto-Medium.ttf");
	ui->tableViewDataSummary->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetData, SIGNAL(currentRowChanged(int)), ui->stackedWidgetData, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetSetup, SIGNAL(currentRowChanged(int)), ui->stackedWidgetSetup, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetGP, SIGNAL(currentRowChanged(int)), ui->stackedWidgetGP, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetResults, SIGNAL(currentRowChanged(int)), ui->stackedWidgetResults, SLOT(setCurrentIndex(int)));
	connect(ui->toolBoxData, SIGNAL(currentChanged(int)), ui->stackedWidgetDataAdvanced, SLOT(setCurrentIndex(int)));
	connect(ui->toolBoxSetup, SIGNAL(currentChanged(int)), ui->stackedWidgetSetupAdvanced, SLOT(setCurrentIndex(int)));
	connect(ui->tableViewDataSummary, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showSelectionMenu(const QPoint&)));
	connect(ui->selectorWidget, SIGNAL(sendSelectedCoordinates(int,int)), this, SLOT(updateOtherPlots(int,int)));
	QSizePolicy sp_retain = ui->InfoBox->sizePolicy();
	sp_retain.setRetainSizeWhenHidden(true);
	ui->InfoBox->setSizePolicy(sp_retain);
	ui->InfoBox->hide();
	ui->label_30->hide();
	ui->previewPlot->hide();
	ui->label_47->hide();
	ui->pdfPlot->hide();
	ui->label_48->hide();
	ui->featurePlot->hide();
	ui->label_49->hide();
	ui->featureOutputPlot->hide();
	ui->page->setMinimumHeight(90);
	ui->frame_3->resize(150,ui->frame_3->height());
	ui->frame_8->resize(150,ui->frame_8->height());
	QPen pen(QColor(232,236,242,255), 2);
	ui->samplePlot->setGridPen(pen);
	pen.setColor(QColor(255,85,85,255));
	pen.setWidth(3);
	ui->samplePlot->setDataPen(pen);
	ui->samplePlot->setSizePolygon(0.8);
	ui->samplePlot->setLabelsFont(QFont("Roboto",9,QFont::Light));
	ui->samplePlot->setLabelsFontColor(QColor(45,65,102,255));
	ui->tableViewDataSummary->hide();
	tableSummarySelectionType = 1; // 1: Column wise, 2: row wise
	ui->label_50->hide();
	ui->samplePlot->hide();
	QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	policy.setHeightForWidth(true);
	ui->selectorWidget->setSizePolicy(policy);
	ui->selectorWidget->setNormalStateColor(QColor(232,236,242,255));
	ui->selectorWidget->setHoverStateColor(QColor(182,194,214,255));
	ui->selectorWidget->setDiagonalColor(QColor(246,248,250,255));
	ui->selectorWidget->setSelectedStateColor(QColor(148,204,20,255));
	ui->label_51->hide();
	ui->label_52->hide();
	ui->label_53->hide();
	ui->label_54->hide();
	ui->selectorWidget->hide();
	ui->correlationPlot->hide();
	ui->regressionPlot->hide();
	ui->qqPlot->hide();
	ui->treeFunctionsSource->sortByColumn(0,Qt::AscendingOrder);
}

MainWidget::~MainWidget()
{
	delete ui;
}

void MainWidget::on_loadButton_pressed()
{
	QString fileName = QFileDialog::getOpenFileName (this, "Open CSV file",
																	 QDir::currentPath(), "CSV (*.csv)");
	QFileInfo infoFile(fileName);

	if(inputData.readFile(fileName))
	{
		int nSamples,nFeatures;
		nSamples = inputData.getSamples();
		nFeatures = inputData.getFeatures();
		ui->label_20->setText("File name:");
		ui->label_21->setText(fileName.section("/",-1,-1));
		ui->label_22->setText("File size:");
		ui->label_23->setText(QString::number(infoFile.size()) + " bytes");
		ui->label_28->setText("File format:");
		ui->label_29->setText("CSV");
		ui->label_24->setText("Number of features:");
		ui->label_25->setText(QString::number(nFeatures));
		ui->label_26->setText("Number of samples:");
		ui->label_27->setText(QString::number(nSamples));
		ui->InfoBox->show();
		populatePreviewPlot(nSamples);
		ui->label_31->hide();
		setTableDataItemsAligment(Qt::AlignCenter);
		setTableHeaderLabels();
		ui->tableViewDataSummary->setModel(inputData.model);
		ui->tableViewDataSummary->show();
		ui->selectorWidget->setNumberVariables(nFeatures);
		ui->selectorWidget->setInitialSelection(0,0);
		ui->label_51->show();
		ui->label_52->show();
		ui->label_53->show();
		ui->label_54->show();
		ui->selectorWidget->show();
		ui->correlationPlot->show();
		ui->regressionPlot->show();
		ui->qqPlot->show();
		updateOtherPlots(0,0);
	}
}

void MainWidget::on_comboBox_activated(int index)
{
	if(inputData.readBenchmark(index))
	{
		int nSamples,nFeatures;
		nSamples = inputData.getSamples();
		nFeatures = inputData.getFeatures();
		ui->label_20->setText("Short name:");
		ui->label_21->setText(inputData.getBenchmarkName(index));
		ui->label_22->setText("Objective function:");
		ui->label_28->setText("Features data range:");
		ui->label_29->setText(inputData.getBenchmarkRange(index));
		ui->label_24->setText("Number of features:");
		ui->label_25->setText(QString::number(nFeatures));
		ui->label_26->setText("Number of samples:");
		ui->label_27->setText(QString::number(nSamples));

		QPixmap pix(130,40);
		pix.fill(QColor(246,248,250,255));
		QPainter painter;
		functionText.useSTIX();
		functionText.set_fontSize(12);
		functionText.parse(inputData.getBenchmarkFunctionLatex(index));
		painter.begin(&pix);
		functionText.set_fontColor(QColor(45,65,102,255));
		functionText.draw(painter, Qt::AlignVCenter, QRectF(0,0,pix.width(), pix.height()), false);
		painter.end();
		ui->label_23->setPixmap(pix);
		ui->InfoBox->show();
		populatePreviewPlot(nSamples);
		ui->label_31->hide();
		setTableDataItemsAligment(Qt::AlignCenter);
		setTableHeaderLabels();
		ui->tableViewDataSummary->setModel(inputData.model);
		ui->tableViewDataSummary->show();
		ui->selectorWidget->setNumberVariables(nFeatures);
		ui->selectorWidget->setInitialSelection(0,0);
		ui->label_51->show();
		ui->label_52->show();
		ui->label_53->show();
		ui->label_54->show();
		ui->selectorWidget->show();
		ui->correlationPlot->show();
		ui->regressionPlot->show();
		ui->qqPlot->show();
		updateOtherPlots(0,0);
	}
}

void MainWidget::setTableDataItemsAligment(Qt::AlignmentFlag aligment)
{
	int nRows = inputData.getSamples();
	int nCols = inputData.getFeatures() + 1;
	for(int j = 0;j < nRows; j++)
	{
		for(int i = 0;i < nCols; i++)
		{
			inputData.model->item(j,i)->setTextAlignment(aligment);
		}
	}
}

void MainWidget::setTableHeaderLabels()
{
	int nCols = inputData.getFeatures();
	for(int i = 0;i < nCols; i++)
	{
		inputData.model->setHeaderData(i,Qt::Horizontal, "X"+ QString::number(i+1));
	}
	inputData.model->setHeaderData(nCols,Qt::Horizontal, "Y");
}

void MainWidget::on_tableViewDataSummary_clicked(const QModelIndex &index)
{
	featurePDF = new KDE();
	int indexCol = index.column();
	int indexRow = index.row();
	int nRows = inputData.getSamples();
	int nCols = inputData.getFeatures();
	double temp;
	double meanFeature,
			medianFeature,
			stdFeature,
			varianceFeature,
			skewnessFeature,
			kurtosisFeature;
	double meanSample,
				medianSample,
				stdSample,
				varianceSample,
				skewnessSample,
				kurtosisSample;
	QVector <double> modesFeature;
	QVector <double> modesSample;
	QVector <double> normalizedSample;
	selectedFeature.clear();
	selectedSample.clear();
	normalizedSample.clear();
	if(tableSummarySelectionType == 1) // feature wise selection
	{
		ui->label_50->hide();  // hide star plot
		ui->samplePlot->hide();
		ui->label_47->show(); // show other plots
		ui->pdfPlot->show();
		ui->label_48->show();
		ui->featurePlot->show();
		ui->label_49->show();
		ui->featureOutputPlot->hide();

		for(int i = 0;i < nRows;i++)
		{
			selectedFeature.push_back(inputData.model->item(i,indexCol)->text().toDouble());
			featurePDF->add_data(inputData.model->item(i,indexCol)->text().toDouble());
		}
		meanFeature = basicStats.computeMean(selectedFeature);
		medianFeature = basicStats.computeMedian(selectedFeature);
		modesFeature = basicStats.computeMode(selectedFeature);
		stdFeature = basicStats.computeStdDev(selectedFeature);
		varianceFeature = basicStats.computeVariance(selectedFeature);
		skewnessFeature = basicStats.computeSkewness(selectedFeature);
		kurtosisFeature = basicStats.computeKurtosis(selectedFeature);
		ui->label_34->setText(QString::number(meanFeature));
		ui->label_36->setText(QString::number(medianFeature));
		ui->label_38->setText(QString::number(modesFeature.at(0)));
		ui->label_40->setText(QString::number(stdFeature));
		ui->label_42->setText(QString::number(varianceFeature));
		ui->label_44->setText(QString::number(skewnessFeature));
		ui->label_46->setText(QString::number(kurtosisFeature));
		drawPdfPlot(100);
		drawFeaturePlot();
		drawFeatureOutputPlot();
		if(indexCol < nCols)
		{
			ui->label_48->setText("Feature Output (X" + QString::number(indexCol+1) + ")");
			ui->label_49->setText("X" + QString::number(indexCol+1) + "/Y");
		}
		else
		{
			ui->label_48->setText("Y");
			ui->label_49->setText("Y/Y");
		}
	}
	else
	{
		ui->label_50->show();  // show star plot
		ui->samplePlot->show();
		ui->label_47->hide(); // hide other plots
		ui->pdfPlot->hide();
		ui->label_48->hide();
		ui->featurePlot->hide();
		ui->label_49->hide();
		ui->featureOutputPlot->hide();
		for(int i = 0;i < nCols;i++)
		{
			selectedSample.push_back(inputData.model->item(indexRow,i)->text().toDouble());
			temp = (inputData.model->item(indexRow,i)->text().toDouble() - inputData.minimum.at(i))/
					(inputData.maximum.at(i) - inputData.minimum.at(i));
			normalizedSample.push_back(temp);
		}
		meanSample = basicStats.computeMean(selectedSample);
		medianSample = basicStats.computeMedian(selectedSample);
		modesSample = basicStats.computeMode(selectedSample);
		stdSample = basicStats.computeStdDev(selectedSample);
		varianceSample = basicStats.computeVariance(selectedSample);
		skewnessSample = basicStats.computeSkewness(selectedSample);
		kurtosisSample = basicStats.computeKurtosis(selectedSample);
		ui->label_34->setText(QString::number(meanSample));
		ui->label_36->setText(QString::number(medianSample));
		ui->label_38->setText(QString::number(modesSample.at(0)));
		ui->label_40->setText(QString::number(stdSample));
		ui->label_42->setText(QString::number(varianceSample));
		ui->label_44->setText(QString::number(skewnessSample));
		ui->label_46->setText(QString::number(kurtosisSample));
		ui->samplePlot->setNumberVariables(nCols);
		ui->samplePlot->setData(normalizedSample);
		ui->label_50->setText("Sample #" + QString::number(indexRow+1));
	}
}

void MainWidget::populatePreviewPlot(int nSamples)
{
	ui->label_30->show();
	ui->previewPlot->show();
	ui->previewPlot->addGraph();
	ui->previewPlot->xAxis->setRange(1,nSamples);
	ui->previewPlot->yAxis->setRange(inputData.Ymin,inputData.Ymax);
	ui->previewPlot->graph(0)->setName("Data");
	ui->previewPlot->graph(0)->setPen(QColor(255,174,0,255));
	ui->previewPlot->graph(0)->setBrush(QBrush(QColor(255,174,0,20)));
	ui->previewPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
	ui->previewPlot->axisRect()->setBackground(QColor(255, 255, 255, 255));
	ui->previewPlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->previewPlot->xAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->previewPlot->yAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->previewPlot->xAxis->setTicks(false);

	QPen pen(QColor(232, 236, 242, 255));
	pen.setWidth(2);
	ui->previewPlot->yAxis->grid()->setPen(pen);
	ui->previewPlot->yAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->previewPlot->yAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->previewPlot->yAxis->setTickLengthIn(0);
	ui->previewPlot->yAxis->setSubTickLengthIn(0);
	ui->previewPlot->yAxis->setTickLabels(true);
	ui->previewPlot->yAxis->setTickLabelFont(QFont("Roboto",12,QFont::Light));
	ui->previewPlot->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->previewPlot->graph(0)->clearData();
	ui->previewPlot->graph(0)->setData(inputData.X,inputData.Y);
	ui->previewPlot->replot();
}

void MainWidget::drawPdfPlot(float nPoints)
{
	double minX = featurePDF->get_min(0);
	double maxX = featurePDF->get_max(0);
	double maxY = 0;
	double x_increment = (maxX - minX)/nPoints;
	QVector<double> X;
	QVector<double> Y;
	QVector<double> YTickVector;
	QVector<QString> YTickLabels;
	QVector<double> XTickVector;
	QVector<QString> XTickLabels;
	double temp;
	for(double x = minX; x <= maxX; x += x_increment)
	{
		X.push_back(x);
		temp = featurePDF->pdf(x);
		Y.push_back(temp);
		if(temp > maxY) maxY = temp;
	}
	XTickVector.push_back(maxX-(maxX-minX)/8);
	XTickVector.push_back(minX);
	XTickLabels.push_back(QString::number(maxX,'f',2));
	XTickLabels.push_back(QString::number(minX,'f',2));
	ui->label_47->show();
	ui->pdfPlot->show();
	ui->pdfPlot->addGraph();
	ui->pdfPlot->xAxis->setRange(minX,maxX);
	ui->pdfPlot->yAxis->setRange(0,maxY);
	QPen pen(QColor(148,204,20,255));
	pen.setWidth(2);
	ui->pdfPlot->graph(0)->setPen(pen);
	ui->pdfPlot->graph(0)->setBrush(QBrush(QColor(148,204,20,40)));
	ui->pdfPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
	ui->pdfPlot->axisRect()->setBackground(QColor(255, 255, 255, 255));
	ui->pdfPlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->pdfPlot->xAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->pdfPlot->yAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->pdfPlot->xAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->pdfPlot->xAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->pdfPlot->xAxis->setTickLengthIn(0);
	ui->pdfPlot->xAxis->setSubTickLengthIn(0);
	ui->pdfPlot->xAxis->setAutoTicks(false);
	ui->pdfPlot->xAxis->setAutoTickLabels(false);
	ui->pdfPlot->xAxis->setTicks(true);
	ui->pdfPlot->xAxis->setTickLabels(true);
	ui->pdfPlot->xAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
	ui->pdfPlot->xAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->pdfPlot->xAxis->setTickVector(XTickVector);
	ui->pdfPlot->xAxis->setTickVectorLabels(XTickLabels);

	YTickVector.push_back(maxY);
	YTickVector.push_back(0);
	YTickLabels.push_back(QString::number(maxY,'f',2));
	YTickLabels.push_back(QString::number(0));
	ui->pdfPlot->yAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->pdfPlot->yAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->pdfPlot->yAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->pdfPlot->yAxis->setTickLengthIn(0);
	ui->pdfPlot->yAxis->setSubTickLengthIn(0);
	ui->pdfPlot->yAxis->setAutoTicks(false);
	ui->pdfPlot->yAxis->setAutoTickLabels(false);
	ui->pdfPlot->yAxis->setTickLabels(true);
	ui->pdfPlot->yAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
	ui->pdfPlot->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->pdfPlot->yAxis->setTickVector(YTickVector);
	ui->pdfPlot->yAxis->setTickVectorLabels(YTickLabels);
	ui->pdfPlot->graph(0)->clearData();
	ui->pdfPlot->graph(0)->setData(X,Y);
	ui->pdfPlot->replot();
}

void MainWidget::drawFeaturePlot()
{
	QVector<double> X,Y;
	QVector<double> YTickVector;
	QVector<QString> YTickLabels;
	QVector<double> XTickVector;
	QVector<QString> XTickLabels;
	double min,max;
	int nRows = inputData.getSamples();
	for(int i = 0;i < nRows;i++)
	{
		X.push_back(i+1);
		Y.push_back(selectedFeature.at(i));
		if(i == 0) min = max = Y.at(0);
		else {
		  if(Y.at(i) < min) min = Y.at(i);
		  if(Y.at(i) > max) max = Y.at(i);
		}
	}
	XTickVector.push_back(nRows-(nRows-1)/8);
	XTickVector.push_back(1);
	XTickLabels.push_back(QString::number(nRows));
	XTickLabels.push_back(QString::number(1));
	ui->label_48->show();
	ui->featurePlot->show();
	ui->featurePlot->addGraph();
	ui->featurePlot->xAxis->setRange(1,nRows);
	ui->featurePlot->yAxis->setRange(min,max);
	QPen pen(QColor(188,95,211,255));
	pen.setWidth(1);
	ui->featurePlot->graph(0)->setPen(pen);
	ui->featurePlot->graph(0)->setBrush(QBrush(QColor(188,95,211,40)));
	ui->featurePlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
	ui->featurePlot->axisRect()->setBackground(QColor(255, 255, 255, 255));
	ui->featurePlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->featurePlot->xAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->featurePlot->yAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->featurePlot->xAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->featurePlot->xAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->featurePlot->xAxis->setTickLengthIn(0);
	ui->featurePlot->xAxis->setSubTickLengthIn(0);
	ui->featurePlot->xAxis->setAutoTicks(false);
	ui->featurePlot->xAxis->setAutoTickLabels(false);
	ui->featurePlot->xAxis->setTicks(true);
	ui->featurePlot->xAxis->setTickLabels(true);
	ui->featurePlot->xAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
	ui->featurePlot->xAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->featurePlot->xAxis->setTickVector(XTickVector);
	ui->featurePlot->xAxis->setTickVectorLabels(XTickLabels);

	YTickVector.push_back(max);
	YTickVector.push_back(min);
	YTickLabels.push_back(QString::number(max,'f',2));
	YTickLabels.push_back(QString::number(min,'f',2));
	ui->featurePlot->yAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->featurePlot->yAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->featurePlot->yAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->featurePlot->yAxis->setTickLengthIn(0);
	ui->featurePlot->yAxis->setSubTickLengthIn(0);
	ui->featurePlot->yAxis->setAutoTicks(false);
	ui->featurePlot->yAxis->setAutoTickLabels(false);
	ui->featurePlot->yAxis->setTickLabels(true);
	ui->featurePlot->yAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
	ui->featurePlot->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->featurePlot->yAxis->setTickVector(YTickVector);
	ui->featurePlot->yAxis->setTickVectorLabels(YTickLabels);
	ui->featurePlot->graph(0)->clearData();
	ui->featurePlot->graph(0)->setData(X,Y);
	ui->featurePlot->replot();
}

void MainWidget::drawFeatureOutputPlot()
{
	QVector<double> X,Y;
	QVector<double> YTickVector;
	QVector<QString> YTickLabels;
	QVector<double> XTickVector;
	QVector<QString> XTickLabels;
	double minX,maxX,minY,maxY;
	int nRows = inputData.getSamples();
	int nCols = inputData.getFeatures() + 1;
	for(int i = 0;i < nRows;i++)
	{
		X.push_back(selectedFeature.at(i));
		Y.push_back(inputData.model->item(i,nCols - 1)->text().toDouble());
		if(i == 0)
		{
			minX = maxX = X.at(0);
			minY = maxY = Y.at(0);
		}
		else
		{
			if(X.at(i) < minX) minX = X.at(i);
			if(X.at(i) > maxX) maxX = X.at(i);
			if(Y.at(i) < minY) minY = Y.at(i);
			if(Y.at(i) > maxY) maxY = Y.at(i);
		}
	}
	XTickVector.push_back(maxX-(maxX-minX)/8);
	XTickVector.push_back(minX);
	XTickLabels.push_back(QString::number(maxX,'f',2));
	XTickLabels.push_back(QString::number(minX,'f',2));
	ui->label_49->show();
	ui->featureOutputPlot->show();
	ui->featureOutputPlot->addGraph();
	ui->featureOutputPlot->xAxis->setRange(minX,maxX);
	ui->featureOutputPlot->yAxis->setRange(minY,maxY);
	QPen pen(QColor(255,85,85,255));
	pen.setWidth(1);
	ui->featureOutputPlot->graph(0)->setPen(pen);
	ui->featureOutputPlot->graph(0)->setBrush(QBrush(QColor(255,85,85,40)));
	ui->featureOutputPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
	ui->featureOutputPlot->axisRect()->setBackground(QColor(255, 255, 255, 255));
	ui->featureOutputPlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->featureOutputPlot->xAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->featureOutputPlot->yAxis->setBasePen(QColor(255, 255, 255, 255));
	ui->featureOutputPlot->xAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->featureOutputPlot->xAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->featureOutputPlot->xAxis->setTickLengthIn(0);
	ui->featureOutputPlot->xAxis->setSubTickLengthIn(0);
	ui->featureOutputPlot->xAxis->setAutoTicks(false);
	ui->featureOutputPlot->xAxis->setAutoTickLabels(false);
	ui->featureOutputPlot->xAxis->setTicks(true);
	ui->featureOutputPlot->xAxis->setTickLabels(true);
	ui->featureOutputPlot->xAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
	ui->featureOutputPlot->xAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->featureOutputPlot->xAxis->setTickVector(XTickVector);
	ui->featureOutputPlot->xAxis->setTickVectorLabels(XTickLabels);

	YTickVector.push_back(maxY);
	YTickVector.push_back(minY);
	YTickLabels.push_back(QString::number(maxY,'f',2));
	YTickLabels.push_back(QString::number(minY,'f',2));
	ui->featureOutputPlot->yAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->featureOutputPlot->yAxis->setTickPen(QColor(255, 255, 255, 255));
	ui->featureOutputPlot->yAxis->setSubTickPen(QColor(255, 255, 255, 255));
	ui->featureOutputPlot->yAxis->setTickLengthIn(0);
	ui->featureOutputPlot->yAxis->setSubTickLengthIn(0);
	ui->featureOutputPlot->yAxis->setAutoTicks(false);
	ui->featureOutputPlot->yAxis->setAutoTickLabels(false);
	ui->featureOutputPlot->yAxis->setTickLabels(true);
	ui->featureOutputPlot->yAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
	ui->featureOutputPlot->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->featureOutputPlot->yAxis->setTickVector(YTickVector);
	ui->featureOutputPlot->yAxis->setTickVectorLabels(YTickLabels);
	ui->featureOutputPlot->graph(0)->clearData();
	ui->featureOutputPlot->graph(0)->setData(X,Y);
	ui->featureOutputPlot->replot();
}

void MainWidget::showSelectionMenu(const QPoint &pos)
{
	QPoint globalPos = ui->tableViewDataSummary->mapToGlobal(pos);
	QMenu myMenu;
	if(tableSummarySelectionType == 1)
	{
		ui->actionSelect_by_feature->setIcon(QIcon(":/icons/resources/images/checkicon.png"));
		ui->actionSelect_by_sample->setIcon(QIcon());
	}
	else
	{
		ui->actionSelect_by_feature->setIcon(QIcon());
		ui->actionSelect_by_sample->setIcon(QIcon(":/icons/resources/images/checkicon.png"));
	}
	myMenu.addAction(ui->actionSelect_by_feature);
	myMenu.addAction(ui->actionSelect_by_sample);
	if(!ui->tableViewDataSummary->isHidden())
		myMenu.exec(mapToGlobal(globalPos));
}

void MainWidget::on_actionSelect_by_feature_triggered()
{
	ui->tableViewDataSummary->setSelectionBehavior(QAbstractItemView::SelectColumns);
	tableSummarySelectionType = 1;
}

void MainWidget::on_actionSelect_by_sample_triggered()
{
	ui->tableViewDataSummary->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableSummarySelectionType = 2;
}

void MainWidget::updateOtherPlots(const int x, const int y)
{
	QVector<double> X,Y;
	QVector<double> variable;
	QVector<double> xReg,yReg;
	double a,b,coeffReg,temp,pearson;
	int nRows = inputData.getSamples();
	double marginX = (inputData.maximum.at(x) - inputData.minimum.at(x)) * 0.1;
	double marginY = (inputData.maximum.at(y) - inputData.minimum.at(y)) * 0.1;
	QVector<double> YTickVector;
	QVector<QString> YTickLabels;
	QVector<double> XTickVector;
	QVector<QString> XTickLabels;

	if(x != y)
	{
		X.clear();
		Y.clear();
		xReg.clear();
		yReg.clear();
		for(int i = 0;i < nRows;i++)
		{
			X.push_back(inputData.model->item(i,x)->text().toDouble());
			Y.push_back(inputData.model->item(i,y)->text().toDouble());
		}
		// calculate linear regression
		if(basicStats.computeUnivariateLinearRegression(X,Y,b,a,coeffReg))
		{
			xReg.push_back(inputData.minimum.at(x));
			xReg.push_back(inputData.maximum.at(x));
			temp = a + b * inputData.minimum.at(x);
			yReg.push_back(temp);
			temp = a + b * inputData.maximum.at(x);
			yReg.push_back(temp);
		}
		// compute Pearson coefficient
		pearson = basicStats.computePearson(X,Y);

		XTickVector.push_back(inputData.minimum.at(x));
		XTickVector.push_back(inputData.maximum.at(x));
		XTickLabels.push_back(QString::number(inputData.minimum.at(x),'f',2));
		XTickLabels.push_back(QString::number(inputData.maximum.at(x),'f',2));
		YTickVector.push_back(inputData.minimum.at(y));
		YTickVector.push_back(inputData.maximum.at(y));
		YTickLabels.push_back(QString::number(inputData.minimum.at(y),'f',2));
		YTickLabels.push_back(QString::number(inputData.maximum.at(y),'f',2));

		ui->label_52->setText("Pearson Correlation ρ = " + QString::number(pearson,'f',4));
		QPen pen(QColor(Qt::white),2);
		QPen pen2(QColor(188,95,211,255),3);
		QCPScatterStyle correlationScatter;
		correlationScatter.setShape(QCPScatterStyle::ssCircle);
		correlationScatter.setPen(Qt::NoPen);
		correlationScatter.setBrush(QColor(255,174,0,128));
		correlationScatter.setSize(6);
		ui->correlationPlot->clearPlottables();
		ui->correlationPlot->addGraph();
		ui->correlationPlot->addGraph();
		ui->correlationPlot->xAxis->setRange(inputData.minimum.at(x) - marginX,inputData.maximum.at(x) + marginX);
		ui->correlationPlot->yAxis->setRange(inputData.minimum.at(y) - marginY,inputData.maximum.at(y) + marginY);
		ui->correlationPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
		ui->correlationPlot->graph(0)->setScatterStyle(correlationScatter);
		ui->correlationPlot->graph(1)->setPen(pen2);

		ui->correlationPlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
		ui->correlationPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
		ui->correlationPlot->xAxis->setBasePen(pen);
		ui->correlationPlot->xAxis->setLabel("X" + QString::number(x+1));
		ui->correlationPlot->xAxis->setLabelFont(QFont("Roboto",11,QFont::Light));
		ui->correlationPlot->xAxis->setLabelColor(QColor(45,65,102,255));
		ui->correlationPlot->xAxis->setAutoTicks(false);
		ui->correlationPlot->xAxis->setAutoTickLabels(false);
		ui->correlationPlot->xAxis->setTicks(true);
		ui->correlationPlot->xAxis->setTickLabels(true);
		ui->correlationPlot->xAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
		ui->correlationPlot->xAxis->setTickLabelColor(QColor(45,65,102,255));
		ui->correlationPlot->xAxis->setTickVector(XTickVector);
		ui->correlationPlot->xAxis->setTickVectorLabels(XTickLabels);
		ui->correlationPlot->xAxis->setTickPen(QColor(255, 255, 255, 255));

		ui->correlationPlot->yAxis->grid()->setPen(QColor(255, 255, 255, 255));
		ui->correlationPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
		ui->correlationPlot->yAxis->setBasePen(pen);
		ui->correlationPlot->yAxis->setLabel("X" + QString::number(y+1));
		ui->correlationPlot->yAxis->setLabelFont(QFont("Roboto",11,QFont::Light));
		ui->correlationPlot->yAxis->setLabelColor(QColor(45,65,102,255));
		ui->correlationPlot->yAxis->setAutoTicks(false);
		ui->correlationPlot->yAxis->setAutoTickLabels(false);
		ui->correlationPlot->yAxis->setTicks(true);
		ui->correlationPlot->yAxis->setTickLabels(true);
		ui->correlationPlot->yAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
		ui->correlationPlot->yAxis->setTickLabelColor(QColor(45,65,102,255));
		ui->correlationPlot->yAxis->setTickVector(YTickVector);
		ui->correlationPlot->yAxis->setTickVectorLabels(YTickLabels);
		ui->correlationPlot->yAxis->setTickPen(QColor(255, 255, 255, 255));

		ui->correlationPlot->graph(0)->clearData();
		ui->correlationPlot->graph(0)->setData(X,Y);
		ui->correlationPlot->graph(1)->clearData();
		ui->correlationPlot->graph(1)->setData(xReg,yReg);
		ui->correlationPlot->replot();
	}
	else
	{
		X.clear();
		Y.clear();
		variable.clear();
		int numberBins = 15;
		int maxY;
		for(int i = 0;i < nRows;i++)
		{
			variable.push_back(inputData.model->item(i,x)->text().toDouble());
		}
		ui->label_52->setText("Histogram for X" + QString::number(x + 1));
		basicStats.designHistogram(variable,numberBins,X,Y,maxY);

		ui->correlationPlot->clearPlottables();
		QCPBars *histogramPlot = new QCPBars(ui->correlationPlot->xAxis, ui->correlationPlot->yAxis);
		ui->correlationPlot->addPlottable(histogramPlot);
		//histogramPlot->setWidth(3/(double)X.size());
		histogramPlot->setPen(Qt::NoPen);
		histogramPlot->setData(X,Y);
		histogramPlot->setAntialiased(false);
		histogramPlot->setAntialiasedFill(false);
		histogramPlot->setBrush(QColor(255,174,0,128));
		histogramPlot->keyAxis()->setAutoTicks(false);
		histogramPlot->keyAxis()->setTickVector(X);
		histogramPlot->keyAxis()->setSubTickCount(0);
		//histogramPlot->keyAxis()->setRange(1,4);
		ui->correlationPlot->xAxis->setRange(-0.5,numberBins-0.5);
		ui->correlationPlot->yAxis->setRange(0,maxY);
		ui->correlationPlot->xAxis->setTicks(false);
		ui->correlationPlot->xAxis->setTickLabels(false);
		ui->correlationPlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
		ui->correlationPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
		ui->correlationPlot->xAxis->setBasePen(Qt::NoPen);
		ui->correlationPlot->xAxis->setLabel("");
		ui->correlationPlot->yAxis->setTicks(false);
		ui->correlationPlot->yAxis->setTickLabels(false);
		ui->correlationPlot->yAxis->grid()->setPen(QColor(255, 255, 255, 255));
		ui->correlationPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
		ui->correlationPlot->yAxis->setBasePen(Qt::NoPen);
		ui->correlationPlot->yAxis->setLabel("");
		ui->correlationPlot->replot();
	}
	drawRegressionPlot(x);
	drawQQPlot(x,y);
}

void MainWidget::drawRegressionPlot(int x)
{
	int i,j;
	QVector<double> X,Y;
	QVector<QVector<double> > data;
	QVector<double> output;
	QVector<double> coefficients;
	QVector<double> min,max;
	QVector<double> xReg,yReg;
	QVector<double> YTickVector;
	QVector<QString> YTickLabels;
	QVector<double> XTickVector;
	QVector<QString> XTickLabels;
	double out;
	int nRows = inputData.getSamples();
	int nCols = inputData.getFeatures();
	double marginX = (inputData.maximum.at(x) - inputData.minimum.at(x)) * 0.1;
	double marginY = (inputData.maximum.at(nCols) - inputData.minimum.at(nCols)) * 0.1;

	X.clear();
	Y.clear();
	min.clear();
	max.clear();
	xReg.clear();
	yReg.clear();
	for(i = 0;i < nRows;i++)
	{
		QVector<double> line;
		for(j = 0;j < nCols;j++)
			line.append(inputData.model->item(i,j)->text().toDouble());
		data.append(line);
		output.append(inputData.model->item(i,nCols)->text().toDouble());
		X.push_back(inputData.model->item(i,x)->text().toDouble());
		Y.push_back(inputData.model->item(i,nCols)->text().toDouble());
	}
	basicStats.computeMultipleLinearRegression(data,output,coefficients);
	for(int i = 0;i < nCols;i++)
	{
		min.push_back(inputData.minimum.at(i));
		max.push_back(inputData.maximum.at(i));
	}
	xReg.push_back(inputData.minimum.at(x));
	xReg.push_back(inputData.maximum.at(x));
	basicStats.processMultipleLinearRegression(min,out);
	yReg.push_back(out);
	basicStats.processMultipleLinearRegression(max,out);
	yReg.push_back(out);
	XTickVector.push_back(inputData.minimum.at(x));
	XTickVector.push_back(inputData.maximum.at(x));
	XTickLabels.push_back(QString::number(inputData.minimum.at(x),'f',2));
	XTickLabels.push_back(QString::number(inputData.maximum.at(x),'f',2));
	YTickVector.push_back(inputData.minimum.at(nCols));
	YTickVector.push_back(inputData.maximum.at(nCols));
	YTickLabels.push_back(QString::number(inputData.minimum.at(nCols),'f',2));
	YTickLabels.push_back(QString::number(inputData.maximum.at(nCols),'f',2));

	ui->label_55->setText("Coefficient " + QString::number(x + 1) + " = " + QString::number(coefficients.at(x),'f',4));
	QCPScatterStyle regressionScatter;
	regressionScatter.setShape(QCPScatterStyle::ssCircle);
	regressionScatter.setPen(Qt::NoPen);
	regressionScatter.setBrush(QColor(255,174,0,128));
	regressionScatter.setSize(6);
	QPen pen(QColor(Qt::white),2);
	QPen pen2(QColor(188,95,211,255),3);
	ui->regressionPlot->addGraph();
	ui->regressionPlot->addGraph();
	ui->regressionPlot->xAxis->setRange(inputData.minimum.at(x) - marginX,inputData.maximum.at(x) + marginX);
	ui->regressionPlot->yAxis->setRange(inputData.minimum.at(nCols) - marginY,inputData.maximum.at(nCols) + marginY);
	ui->regressionPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
	ui->regressionPlot->graph(0)->setScatterStyle(regressionScatter);
	ui->regressionPlot->graph(1)->setPen(pen2);

	ui->regressionPlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->regressionPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
	ui->regressionPlot->xAxis->setBasePen(pen);
	ui->regressionPlot->xAxis->setLabel("X" + QString::number(x+1));
	ui->regressionPlot->xAxis->setLabelFont(QFont("Roboto",11,QFont::Light));
	ui->regressionPlot->xAxis->setLabelColor(QColor(45,65,102,255));
	ui->regressionPlot->xAxis->setAutoTicks(false);
	ui->regressionPlot->xAxis->setAutoTickLabels(false);
	ui->regressionPlot->xAxis->setTicks(true);
	ui->regressionPlot->xAxis->setTickLabels(true);
	ui->regressionPlot->xAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
	ui->regressionPlot->xAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->regressionPlot->xAxis->setTickVector(XTickVector);
	ui->regressionPlot->xAxis->setTickVectorLabels(XTickLabels);
	ui->regressionPlot->xAxis->setTickPen(QColor(255, 255, 255, 255));

	ui->regressionPlot->yAxis->grid()->setPen(QColor(255, 255, 255, 255));
	ui->regressionPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
	ui->regressionPlot->yAxis->setBasePen(pen);
	ui->regressionPlot->yAxis->setLabel("Y");
	ui->regressionPlot->yAxis->setLabelFont(QFont("Roboto",11,QFont::Light));
	ui->regressionPlot->yAxis->setLabelColor(QColor(45,65,102,255));
	ui->regressionPlot->yAxis->setAutoTicks(false);
	ui->regressionPlot->yAxis->setAutoTickLabels(false);
	ui->regressionPlot->yAxis->setTicks(true);
	ui->regressionPlot->yAxis->setTickLabels(true);
	ui->regressionPlot->yAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
	ui->regressionPlot->yAxis->setTickLabelColor(QColor(45,65,102,255));
	ui->regressionPlot->yAxis->setTickVector(YTickVector);
	ui->regressionPlot->yAxis->setTickVectorLabels(YTickLabels);
	ui->regressionPlot->yAxis->setTickPen(QColor(255, 255, 255, 255));

	ui->regressionPlot->graph(0)->clearData();
	ui->regressionPlot->graph(0)->setData(X,Y);
	ui->regressionPlot->graph(1)->clearData();
	ui->regressionPlot->graph(1)->setData(xReg,yReg);
	ui->regressionPlot->replot();

}

void MainWidget::drawQQPlot(int x, int y)
{
	QVector<double> X,Y,x45,y45;
	QVector<double> varX;
	QVector<double> varY;
	double minX,minY,maxX,maxY;
	int nRows = inputData.getSamples();
	int nCols = inputData.getFeatures();
	QVector<double> YTickVector;
	QVector<QString> YTickLabels;
	QVector<double> XTickVector;
	QVector<QString> XTickLabels;

	if(x != y)
	{
		X.clear();
		Y.clear();
		for(int i = 0;i < nRows;i++)
		{
			X.push_back(inputData.model->item(i,x)->text().toDouble());
			Y.push_back(inputData.model->item(i,y)->text().toDouble());
		}
		basicStats.computeQuantiles(X,varX,100);
		basicStats.computeQuantiles(Y,varY,100);
		minX = *std::min_element(varX.constBegin(), varX.constEnd());
		minY = *std::min_element(varY.constBegin(), varY.constEnd());
		maxX = *std::max_element(varX.constBegin(), varX.constEnd());
		maxY = *std::max_element(varY.constBegin(), varY.constEnd());
		double marginX = (maxX - minX) * 0.1;
		double marginY = (maxY - minY) * 0.1;
		x45.push_back(minX);
		x45.push_back(maxX);
		y45.push_back(minY);
		y45.push_back(maxY);
		XTickVector.push_back(minX);
		XTickVector.push_back(maxX);
		XTickLabels.push_back(QString::number(minX,'f',2));
		XTickLabels.push_back(QString::number(maxX,'f',2));
		YTickVector.push_back(minY);
		YTickVector.push_back(maxY);
		YTickLabels.push_back(QString::number(minY,'f',2));
		YTickLabels.push_back(QString::number(maxY,'f',2));

		QCPScatterStyle qqScatter;
		qqScatter.setShape(QCPScatterStyle::ssCircle);
		qqScatter.setPen(Qt::NoPen);
		qqScatter.setBrush(QColor(255,174,0,128));
		qqScatter.setSize(6);
		QPen pen(QColor(Qt::white),2);
		QPen pen2(QColor(255,174,0,128),2);
		ui->qqPlot->addGraph();
		ui->qqPlot->addGraph();
		ui->qqPlot->xAxis->setRange(minX - marginX,maxX + marginX);
		ui->qqPlot->yAxis->setRange(minY - marginY,maxY + marginY);
		ui->qqPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
		ui->qqPlot->graph(1)->setScatterStyle(qqScatter);
		ui->qqPlot->graph(0)->setPen(pen2);

		ui->qqPlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
		ui->qqPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
		ui->qqPlot->xAxis->setBasePen(pen);
		ui->qqPlot->xAxis->setLabel("X" + QString::number(x+1));
		ui->qqPlot->xAxis->setLabelFont(QFont("Roboto",11,QFont::Light));
		ui->qqPlot->xAxis->setLabelColor(QColor(45,65,102,255));
		ui->qqPlot->xAxis->setAutoTicks(false);
		ui->qqPlot->xAxis->setAutoTickLabels(false);
		ui->qqPlot->xAxis->setTicks(true);
		ui->qqPlot->xAxis->setTickLabels(true);
		ui->qqPlot->xAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
		ui->qqPlot->xAxis->setTickLabelColor(QColor(45,65,102,255));
		ui->qqPlot->xAxis->setTickVector(XTickVector);
		ui->qqPlot->xAxis->setTickVectorLabels(XTickLabels);
		ui->qqPlot->xAxis->setTickPen(QColor(255, 255, 255, 255));

		ui->qqPlot->yAxis->setTicks(false);
		ui->qqPlot->yAxis->setTickLabels(false);
		ui->qqPlot->yAxis->grid()->setPen(QColor(255, 255, 255, 255));
		ui->qqPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
		ui->qqPlot->yAxis->setBasePen(pen);
		ui->qqPlot->yAxis->setLabel("X" + QString::number(y+1));
		ui->qqPlot->yAxis->setLabelFont(QFont("Roboto",11,QFont::Light));
		ui->qqPlot->yAxis->setLabelColor(QColor(45,65,102,255));
		ui->qqPlot->yAxis->setAutoTicks(false);
		ui->qqPlot->yAxis->setAutoTickLabels(false);
		ui->qqPlot->yAxis->setTicks(true);
		ui->qqPlot->yAxis->setTickLabels(true);
		ui->qqPlot->yAxis->setTickLabelFont(QFont("Roboto",9,QFont::Light));
		ui->qqPlot->yAxis->setTickLabelColor(QColor(45,65,102,255));
		ui->qqPlot->yAxis->setTickVector(YTickVector);
		ui->qqPlot->yAxis->setTickVectorLabels(YTickLabels);
		ui->qqPlot->yAxis->setTickPen(QColor(255, 255, 255, 255));

		ui->qqPlot->graph(1)->clearData();
		ui->qqPlot->graph(1)->setData(varX,varY);
		ui->qqPlot->graph(0)->clearData();
		ui->qqPlot->graph(0)->setData(x45,y45);
		ui->qqPlot->replot();
	}
	else
	{
		ui->qqPlot->xAxis->grid()->setPen(QColor(255, 255, 255, 255));
		ui->qqPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
		ui->qqPlot->xAxis->setBasePen(QPen(Qt::white));
		ui->qqPlot->xAxis->setAutoTicks(false);
		ui->qqPlot->xAxis->setAutoTickLabels(false);
		ui->qqPlot->xAxis->setTicks(false);
		ui->qqPlot->xAxis->setTickLabels(false);
		ui->qqPlot->xAxis->setTickPen(QColor(255, 255, 255, 255));

		ui->qqPlot->yAxis->grid()->setPen(QColor(255, 255, 255, 255));
		ui->qqPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
		ui->qqPlot->yAxis->setBasePen(QPen(Qt::white));
		ui->qqPlot->yAxis->setAutoTicks(false);
		ui->qqPlot->yAxis->setAutoTickLabels(false);
		ui->qqPlot->yAxis->setTicks(false);
		ui->qqPlot->yAxis->setTickLabels(false);
		ui->qqPlot->yAxis->setTickPen(QColor(255, 255, 255, 255));
	}
}

void MainWidget::on_treeFunctionsSource_itemClicked(QTreeWidgetItem *item, int column)
{
	QString functionString;
	int arity;
	QPixmap pix(190,40);
	QPainter painter;
	pix.fill(QColor(246,248,250,255));
	ui->label_69->setText(item->text(column));
	functionOperator.useSTIX();
	functionOperator.set_fontSize(12);
	functionOperatorToLatex(item->text(column),functionString,arity);
	functionOperator.parse(functionString);
	painter.begin(&pix);
	functionOperator.set_fontColor(QColor(45,65,102,255));
	functionOperator.draw(painter, Qt::AlignVCenter, QRectF(0,0,pix.width(), pix.height()), false);
	painter.end();
	ui->label_70->setPixmap(pix);
	ui->label_71->setText(QString::number(arity));
}

void MainWidget::functionOperatorToLatex(QString name, QString &output, int &arity)
{
	if(name.compare("Addition") == 0) { output = "\\it{x_1} + \\it{x_2}"; arity = 2; }
	if(name.compare("Division") == 0) { output = "\\it{x_1} \\div \\it{x_2}"; arity = 2; }
	if(name.compare("Modulo") == 0) { output = "mod(\\it{x_1} \\div \\it{x_2})"; arity = 2; }
	if(name.compare("Multiplication") == 0) { output = "\\it{x_1} \\times \\it{x_2}"; arity = 2; }
	if(name.compare("Subtraction") == 0) { output = "\\it{x_1} - \\it{x_2}"; arity = 2; }
	if(name.compare("If Then Else") == 0) { output = "if\\{ \\it{x_1} \\} then\\{ \\it{x_2} \\} else\\{ \\it{x_3} \\}"; arity = 3; }
	if(name.compare("And") == 0) { output = "\\it{x_1} \\land \\it{x_2}"; arity = 2; }
	if(name.compare("Maximum") == 0) { output = "max( \\it{x_1} , \\it{x_2} )"; arity = 2; }
	if(name.compare("Minimum") == 0) { output = "min( \\it{x_1} , \\it{x_2} )"; arity = 2; }
	if(name.compare("Negation") == 0) { output = "\\neg \\it{x_1}"; arity = 1; }
	if(name.compare("Or") == 0) { output = "\\it{x_1} \\lor \\it{x_2}"; arity = 2; }
	if(name.compare("Xor") == 0) { output = "\\it{x_1} \\oplus \\it{x_2}"; arity = 2; }
	if(name.compare("Absolute") == 0) { output = "\\left| \\it{x_1} \\right|"; arity = 1; }
	if(name.compare("Ceil") == 0) { output = "\\lceil \\it{x_1} \\rceil"; arity = 1; }
	if(name.compare("Cube") == 0) { output = "\\it{x_1}^3"; arity = 1; }
	if(name.compare("Exponential") == 0) { output = "exp( \\it{x_1} )"; arity = 1; }
	if(name.compare("Exponential base-10") == 0) { output = "exp_{10}( \\it{x_1} )"; arity = 1; }
	if(name.compare("Exponential base-2") == 0) { output = "exp_2( \\it{x_1} )"; arity = 1; }
	if(name.compare("Floor") == 0) { output = "\\lfloor \\it{x_1} \\rfloor"; arity = 1; }
	if(name.compare("Logarithm base-10") == 0) { output = "log_{10}( \\it{x_1} )"; arity = 1; }
	if(name.compare("Logarithm base-2") == 0) { output = "log_2( \\it{x_1} )"; arity = 1; }
	if(name.compare("Mean") == 0) { output = "( \\it{x_1} + \\it{x_2} ) \\div 2"; arity = 2; }
	if(name.compare("Natural logarithm") == 0) { output = "ln( \\it{x_1} )"; arity = 1; }
	if(name.compare("Power") == 0) { output = "\\it{x_1}^{\\it{x_2}}"; arity = 2; }
	if(name.compare("Power exponent 4") == 0) { output = "\\it{x_1}^4"; arity = 1; }
	if(name.compare("Root square") == 0) { output = "\\sqrt{ \\it{x_1} }"; arity = 1; }
	if(name.compare("Round") == 0) { output = "\\lfloor \\it{x_1} \\rfloor \\lor \\lceil \\it{x_1} \\rceil"; arity = 1; }
	if(name.compare("Sign") == 0) { output = "- \\it{x_1}"; arity = 1; }
	if(name.compare("Square") == 0) { output = "\\it{x_1}^2"; arity = 1; }
	if(name.compare("Natural logarithm") == 0) { output = "ln( \\it{x_1} )"; arity = 1; }
	if(name.compare("Equal") == 0) { output = "\\it{x_1} = \\it{x_2}"; arity = 2; }
	if(name.compare("Greater") == 0) { output = "\\it{x_1} > \\it{x_2}"; arity = 2; }
	if(name.compare("Greater or equal") == 0) { output = "\\it{x_1} \\geq \\it{x_2}"; arity = 2; }
	if(name.compare("Less") == 0) { output = "\\it{x_1} < \\it{x_2}"; arity = 2; }
	if(name.compare("Less or equal") == 0) { output = "\\it{x_1} \\leq \\it{x_2}"; arity = 2; }
	if(name.compare("Not equal") == 0) { output = "\\it{x_1} \\neq \\it{x_2}"; arity = 2; }
	if(name.compare("Equal") == 0) { output = "\\it{x_1} = \\it{x_2} )"; arity = 2; }
	if(name.compare("Cosine") == 0) { output = "cos( \\it{x_1} )"; arity = 1; }
	if(name.compare("Sine") == 0) { output = "sin( \\it{x_1} )"; arity = 1; }
	if(name.compare("Tangent") == 0) { output = "tan( \\it{x_1} )"; arity = 1; }
}

void MainWidget::on_listFunctionsTarget_itemClicked(QListWidgetItem *item)
{
	QString functionString;
	int arity;
	QPixmap pix(190,40);
	QPainter painter;
	pix.fill(QColor(246,248,250,255));
	ui->label_69->setText(item->text());
	functionOperator.useSTIX();
	functionOperator.set_fontSize(12);
	functionOperatorToLatex(item->text(),functionString,arity);
	functionOperator.parse(functionString);
	painter.begin(&pix);
	functionOperator.set_fontColor(QColor(45,65,102,255));
	functionOperator.draw(painter, Qt::AlignVCenter, QRectF(0,0,pix.width(), pix.height()), false);
	painter.end();
	ui->label_70->setPixmap(pix);
	ui->label_71->setText(QString::number(arity));
}
