#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QMainWindow>
#include <QFontDatabase>
#include <QFileDialog>
#include "dataset.h"
#include "jkqtmathtext.h"
#include "math_stats.h"
#include "kde.h"
#include "starplotwidget.h"
#include "gridselectorwidget.h"
#include "listwidget.h"
#include "circularprogresswidget.h"
#include "workergp.h"
#include "cl.hpp"

namespace Ui {
class MainWidget;
}

class MainWidget : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWidget(QWidget *parent = 0);
	~MainWidget();
	Dataset inputData;
	void populatePreviewPlot(int nSamples);
	JKQTmathText functionText;
	JKQTmathText functionOperator;
	math_stats basicStats;
	KDE *featurePDF;

private slots:
	void on_loadButton_pressed();
	void on_comboBox_activated(int index);
	void on_tableViewDataSummary_clicked(const QModelIndex &index);
	void on_actionSelect_by_feature_triggered();
	void on_actionSelect_by_sample_triggered();
	void showSelectionMenu(const QPoint &pos);
	void showDeleteMenu(const QPoint &pos);
	void updateOtherPlots(const int x, const int y);
	void on_treeFunctionsSource_itemClicked(QTreeWidgetItem *item, int column);
	void on_listFunctionsTarget_itemClicked(QListWidgetItem *item);
	void on_actionRemove_triggered();
	void on_listTerminalsSource_itemClicked(QListWidgetItem *item);
	void on_listTerminalsTarget_itemClicked(QListWidgetItem *item);
	void on_startButton_clicked();
	void showElapsedTime(void);
	void receivedProgress1(int value);
	void algorithmFinished(void);

private:
	Ui::MainWidget *ui;
	void setTableDataItemsAligment(Qt::AlignmentFlag aligment);
	void setTableHeaderLabels(void);
	void functionOperatorToLatex(QString name, QString &output, int &arity);
	void terminalListNames(QString name);
	void drawPdfPlot(float nPoints);
	void drawFeaturePlot(void);
	void drawFeatureOutputPlot(void);
	void drawRegressionPlot(int x);
	void drawQQPlot(int x, int y);
	void populateTerminalList(void);
	QVector <double> selectedFeature;
	QVector <double> selectedSample;
	int tableSummarySelectionType;
	void showStartedTime(void);
	QString secondsToDHMS(qint64 duration);
	void getInfoOpenCL(void);

	QThread* threadGP;
	workerGP* workerAlgorithm;
	void requestAlgorithmStart(void);
	QTimer* timerGP;
	QDateTime startedDateTime;
};

#endif // MAINWIDGET_H
