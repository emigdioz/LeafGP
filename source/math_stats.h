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
	bool computeMultipleLinearRegression(QVector<QVector<double> > X, QVector<double> Y, QVector<double> &Coeff);
	void processMultipleLinearRegression(QVector<double> variables, double &Y);
	void computeQuantiles(QVector<double> input, QVector<double> &output, int divisions);

private:
	alglib::linearmodel model;
	double quantile(std::vector<double> d, int size, double q);
};

#endif // MATH_STATS_H
