#include "math_stats.h"

math_stats::math_stats()
{

}

double math_stats::computeMean(QVector<double> input)
{
	qSort(input.begin(),input.end());
	return std::accumulate(input.begin(), input.end(), .0) / input.size();
}

double math_stats::computeMedian(QVector<double> input)
{
	qSort(input.begin(),input.end());
	return input.size()%2 ? input[input.size() / 2]	: ((double)input[input.size() / 2 - 1] + input[input.size() / 2]) * .5;
}

QVector <double> math_stats::computeMode(QVector<double> input)
{
	QVector <double> modes;
	qSort(input.begin(),input.end());
	QVector<double>::const_iterator it = input.begin();
	double runValue = *it++;
	unsigned runCount = 1;
	unsigned highestRunCount = runCount;
	modes.push_back(runValue);
	while (it != input.end())
	{
		if(runValue == *it)
		{
			if(++runCount > highestRunCount)
			{
				highestRunCount = runCount;
				if(modes.front() != runValue)
				{
					modes.clear();
					modes.push_back(runValue);
				}
			}
			else if (runCount == highestRunCount)
				modes.push_back(runValue);
		}
		else
			runValue = *it, runCount = 0;
		++it;
	}
	return modes;
}

double math_stats::computeStdDev(QVector<double> input)
{
	return std::sqrt(computeVariance(input));
}

double math_stats::computeVariance(QVector<double> input)
{
	double mean = computeMean(input);
	std::vector<double> diff(input.size());
	std::transform(input.begin(), input.end(), diff.begin(), [mean](double x) { return x - mean; });
	return std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0) / input.size();
}

double math_stats::computeSkewness(QVector<double> input)
{
	double gamma = 0,sigma = 0,med = computeMean(input);
	for(int i = 0;i < input.size();i++)
	{
		double temp = (input.at(i) - med);
		gamma += pow(temp,3);
		sigma += temp*temp;
	}
	sigma = std::sqrt(sigma/input.size());
	gamma /= std::pow(sigma,3);
	return gamma/(input.size() - 1);
}

double math_stats::computeKurtosis(QVector<double> input)
{
	double gamma = 0,sigma = 0,med = computeMean(input);
	for(int i = 0;i < input.size();i++)
	{
		double temp = (input.at(i) - med);
		gamma += pow(temp,4);
		sigma += temp*temp;
	}
	sigma = std::sqrt(sigma/input.size());
	gamma /= std::pow(sigma,4);
	return gamma/(input.size() - 1) - 3;
}
