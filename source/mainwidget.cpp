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
}

MainWidget::~MainWidget()
{
	delete ui;
}
