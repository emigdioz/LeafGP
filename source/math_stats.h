#ifndef MATH_STATS_H
#define MATH_STATS_H

#include <QVector>
#include <algorithm>

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
};

#endif // MATH_STATS_H
