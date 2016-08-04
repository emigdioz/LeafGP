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
#include "drawtreewidget.h"
#include "workergp.h"
#include "cl.hpp"
#include "GP.h"
#include "params.h"

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
	std::vector<std::vector<float> > input_data_matrix;
public slots:
	void receivedRunProgress(int value, int run);

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
	void algorithmFinished(void);
	void parallelDevicesChecked(bool t);
	void receivedSingleTree(GP::treeStruct data);
	void receivedBasicInfo(GP::basicInfo info);

	void on_checkBox_toggled(bool checked);

	void on_listWidgetDataAdvanced_currentRowChanged(int currentRow);

	void on_listWidgetData_currentRowChanged(int currentRow);

	void on_listWidgetSetup_currentRowChanged(int currentRow);

	void on_spinBox_5_valueChanged(int arg1);

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
	void drawCorrelationPlotGP(QVector<double> actualY, QVector<double> expectedY);
	void populateTerminalList(void);
	QVector <double> selectedFeature;
	QVector <double> selectedSample;
	int tableSummarySelectionType;
	void showStartedTime(void);
	QString secondsToDHMS(qint64 duration);
	void getInfoOpenCL(void);
	void populateDataForGP(void);
	QThread* threadGP;
	workerGP* workerAlgorithm;
	void requestAlgorithmStart(void);
	QTimer* timerGP;
	QDateTime startedDateTime;
	int devicesNumber;
	int deviceChecked;
	QVector<int> devicesType;
	GP::treeStruct selectedTree;
	int countLeaves(int index, int count);
	void positionLeaves(int index, int depth, int numberLeaves);
	void positionParents(int index, int depth);
	void setDefaultGPParameters(void);
};

#endif // MAINWIDGET_H
