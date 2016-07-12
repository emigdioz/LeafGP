#ifndef MATH_STATS_H
#define MATH_STATS_H

#include <QVector>
#include <algorithm>
#include "dataanalysis.h"

class math_stats
{
public:
	math_stats();
	double computeMean(QVector <double> input);
	double computeMedian(QVector <double> input);
	QVector<double> computeMode(QVector <double> input);
	double computeStdDev(QVector <double> input);
	double computeVariance(QVector <double> input);
	double computeSkewness(QVector <double> input);
	double computeKurtosis(QVector <double> input);
	bool computeUnivariateLinearRegression(QVector<double> x, QVector<double> y, double &slope, double &intercept, double &coefficient);
	double computePearson(QVector<double> x, QVector<double> y);
	void designHistogram(QVector<double> data, int nBins, QVector<double> &keys, QVector<double> &frequencies, int &maxY);
	bool symmetricMatrixInvert(QVector<QVector<double> > &V);
	bool Regress(QVector<double> Y, QVector<QVector<double> > X);
	bool computeMultipleLinearRegression(QVector<QVector<double> > X, QVector<double> Y, QVector<double> &Coeff);
	void processMultipleLinearRegression(QVector<double> variables, double &Y);
private:
	alglib::linearmodel model;
};

#endif // MATH_STATS_H
