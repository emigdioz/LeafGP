#ifndef DATASET_H
#define DATASET_H

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStandardItemModel>
#include "benchmark.h"
#include <QMessageBox>
#include <QFileDialog>
#include "matio.h"

class Dataset
{
public:
	Dataset();
	bool readFile(QString fileName);
	bool readBenchmark(int choice);
	int getFeatures(void);
	int getSamples(void);
	QString getBenchmarkName(int choice);
	QString getBenchmarkFunctionLatex(int choice);
	QString getBenchmarkRange(int choice);
	QVector<double> Y;
	QVector<double> X;
	double Ymin;
	double Ymax;
	double Xmin;
	double Xmax;
	benchmark dBenchmark;
	QStandardItemModel *model;
	QVector<double> minimum;
	QVector<double> maximum;

private:
	void checkString(QString &temp, QChar character = 0);
	QList<QStandardItem*> standardItemList;
	int nRows;
	int nCols;
};

#endif // DATASET_H
