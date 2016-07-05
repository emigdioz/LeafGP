#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QMainWindow>
#include <QFontDatabase>
#include <QFileDialog>
#include "dataset.h"
#include "jkqtmathtext.h"

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

private slots:
	void on_loadButton_pressed();

	void on_comboBox_activated(int index);

private:
	Ui::MainWidget *ui;
	void setTableDataItemsAligment(Qt::AlignmentFlag aligment);
};

#endif // MAINWIDGET_H
