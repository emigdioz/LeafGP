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
	math_stats basicStats;
	KDE *featurePDF;

private slots:
	void on_loadButton_pressed();
	void on_comboBox_activated(int index);
	void on_tableViewDataSummary_clicked(const QModelIndex &index);
	void on_actionSelect_by_feature_triggered();
	void on_actionSelect_by_sample_triggered();
	void showSelectionMenu(const QPoint &pos);
	void updateOtherPlots(const int x, const int y);

private:
	Ui::MainWidget *ui;
	void setTableDataItemsAligment(Qt::AlignmentFlag aligment);
	void setTableHeaderLabels(void);
	void drawPdfPlot(float nPoints);
	void drawFeaturePlot(void);
	void drawFeatureOutputPlot(void);
	void drawRegressionPlot(void);
	QVector <double> selectedFeature;
	QVector <double> selectedSample;
	int tableSummarySelectionType;
};

#endif // MAINWIDGET_H
