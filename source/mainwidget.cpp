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
	ui->listFunctionsTarget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetData, SIGNAL(currentRowChanged(int)), ui->stackedWidgetData, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetSetup, SIGNAL(currentRowChanged(int)), ui->stackedWidgetSetup, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetGP, SIGNAL(currentRowChanged(int)), ui->stackedWidgetGP, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetResults, SIGNAL(currentRowChanged(int)), ui->stackedWidgetResults, SLOT(setCurrentIndex(int)));
	connect(ui->toolBoxData, SIGNAL(currentChanged(int)), ui->stackedWidgetDataAdvanced, SLOT(setCurrentIndex(int)));
	connect(ui->toolBoxSetup, SIGNAL(currentChanged(int)), ui->stackedWidgetSetupAdvanced, SLOT(setCurrentIndex(int)));
	connect(ui->tableViewDataSummary, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showSelectionMenu(const QPoint&)));
	connect(ui->listFunctionsTarget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showDeleteMenu(const QPoint&)));
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
	ui->circularProgress->setCircular1BackgroundColor(QColor(246,248,250,255));
	ui->circularProgress->setCircular2BackgroundColor(QColor(246,248,250,255));
	ui->circularProgress->setBackgroundColor(QColor(246,248,250,255));
	ui->circularProgress->setCircular1Color(QColor(255,174,0,255));
	ui->circularProgress->setCircular2Color(QColor(188,95,211,255));
	ui->circularProgress->setFont(QFont("Roboto",9,QFont::Light));
	ui->circularProgress->setFontColor(QColor(45,65,102));
	ui->circularProgress->setDoubleProgress(true);
	ui->circularProgress->setProgress1(400);
	ui->circularProgress->setProgress2(800);
}

MainWidget::~MainWidget()
{
	delete ui;
}
