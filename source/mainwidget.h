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
#include "populationmapwidget.h"
#include "workergp.h"
#include "cl.hpp"
#include "GP.h"
#include "params.h"
#include <QPainter>
#include <QPixmap>
#include <QLinearGradient>
#include "gpexperiment.h"
#include <QVector>
#include "qcustomplot.h"
#include <QPrinter>
#include "filecontainer.h"
#include "reportgp.h"

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
	gpExperiment userExperiment;
	int indexRun;
	fileContainer projectFile;
	bool validDataProject;
	QString projectFilename;
	reportGP userReport;
	float bestOverallFitness;

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
	void receivedPopInfo(GP::popInfo info);
	void on_checkBox_toggled(bool checked);
	void on_listWidgetDataAdvanced_currentRowChanged(int currentRow);
	void on_listWidgetData_currentRowChanged(int currentRow);
	void on_listWidgetSetup_currentRowChanged(int currentRow);
	void on_spinBox_5_valueChanged(int arg1);
	void on_horizontalSlider_valueChanged(int value);
	void on_listWidgetGP_currentRowChanged(int currentRow);
	void on_comboBox_7_currentIndexChanged(int index);
	void on_spinBox_valueChanged(int arg1);
	void on_spinBox_2_valueChanged(int arg1);
	void on_spinBox_3_valueChanged(int arg1);
	void on_lineEdit_textChanged(const QString &arg1);
	void on_spinBox_4_valueChanged(int arg1);
	void on_lineEdit_3_textEdited(const QString &arg1);
	void on_lineEdit_4_textChanged(const QString &arg1);
	void on_lineEdit_5_textChanged(const QString &arg1);
	void on_spinBox_6_valueChanged(int arg1);
	void on_listWidgetResults_currentRowChanged(int currentRow);
	void on_runSelectionView_clicked(const QModelIndex &index);
	void individualMapSelected(int &ind, int &gen, int &id);

	void on_pushButton_9_clicked();

	void on_pushButton_12_clicked();

	void on_pushButton_13_clicked();

	void on_pushButton_3_clicked();

	void on_pushButton_2_clicked();

	void on_pushButton_11_clicked();

	void on_pushButton_10_clicked();

	void on_checkBox_2_toggled(bool checked);

	void on_checkBox_3_toggled(bool checked);

	void on_checkBox_6_toggled(bool checked);

	void on_checkBox_7_toggled(bool checked);

	void on_checkBox_4_toggled(bool checked);

	void on_checkBox_8_toggled(bool checked);

	void on_checkBox_5_toggled(bool checked);

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
	void drawBoxplots(void);
	void drawPerformancePlot(void);
	void setupQualityPlot(void);
	void setupSizePlot(void);
	void setupBoxplots(void);
	void setupQualityRunPlot(void);
	void setupSizeRunPlot(void);
	void setupPerformancePlot(void);
	void populateTerminalList(void);
	void populateRunsSelectionList(void);
	void populateDataForContainer(void);
	void showResultsExpressInfo(void);
	void drawExpressCorrelationPlot(QVector<double> actualY, QVector<double> expectedY);
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
	int runObserver;
	QPixmap drawSmallCircle(QColor color, int size);
	QPixmap drawGradient(int width, int height);
	QPixmap drawGradient90(int width, int height);
	void resetDefaultGPParameters(void);	
	gpExperiment::popData currPop;
	QCPStatisticalBox* bestTrainFitness;
	QCPStatisticalBox* bestTestFitness;
	QCPStatisticalBox* avgSize;
	QCPStatisticalBox* bestSize;
	QStringListModel *modelRunsList;
	int performancePlotSlices = 20;

};

#endif // MAINWIDGET_H
