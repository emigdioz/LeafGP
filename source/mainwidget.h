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

private:
	Ui::MainWidget *ui;
	void setTableDataItemsAligment(Qt::AlignmentFlag aligment);
	void setTableHeaderLabels(void);
	void functionOperatorToLatex(QString name, QString &output, int &arity);
	void drawPdfPlot(float nPoints);
	void drawFeaturePlot(void);
	void drawFeatureOutputPlot(void);
	void drawRegressionPlot(int x);
	void drawQQPlot(int x, int y);
	QVector <double> selectedFeature;
	QVector <double> selectedSample;
	int tableSummarySelectionType;
};

#endif // MAINWIDGET_H
