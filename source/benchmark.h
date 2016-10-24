#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <QStandardItem>
#include "math.h"

class benchmark
{
public:
	benchmark();
	bool generateData(int selection, QStandardItemModel &model_out);
	int getRows(void);
	int getCols(void);

private:
	double rollUniform(double inLow, double inUp);
	QStandardItemModel *model;
	int nRows;
	int nCols;
};

#endif // BENCHMARK_H
