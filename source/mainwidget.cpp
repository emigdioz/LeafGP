#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWidget)
{
	ui->setupUi(this);
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Lato-Light.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Lato-Medium.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Roboto-Light.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Roboto-Medium.ttf");
	connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetData, SIGNAL(currentRowChanged(int)), ui->stackedWidgetData, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetSetup, SIGNAL(currentRowChanged(int)), ui->stackedWidgetSetup, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetGP, SIGNAL(currentRowChanged(int)), ui->stackedWidgetGP, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetResults, SIGNAL(currentRowChanged(int)), ui->stackedWidgetResults, SLOT(setCurrentIndex(int)));
	QSizePolicy sp_retain = ui->InfoBox->sizePolicy();
	sp_retain.setRetainSizeWhenHidden(true);
	ui->InfoBox->setSizePolicy(sp_retain);
	ui->InfoBox->hide();
	ui->label_30->hide();
	ui->previewPlot->hide();
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
	ui->previewPlot->graph(0)->setPen(QColor(255,153,85,255));
	ui->previewPlot->graph(0)->setBrush(QBrush(QColor(255,153,85,20)));
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
	}
}
