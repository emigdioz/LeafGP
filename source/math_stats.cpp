#include "math_stats.h"
#include <QDebug>

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

bool math_stats::computeUnivariateLinearRegression(QVector<double> x, QVector<double> y, double &slope, double &intercept, double &coefficient)
{
	double xa = 0, ya = 0;
	double m_a, m_b, m_coeff;
	if(x.size() != y.size())
	{
		slope = 0;
		intercept = 0;
		coefficient = 0;
		return false;
	}
	// calculate the averages of arrays x and y
	for(int i = 0;i < x.size();i++)
	{
		xa += x.at(i);
		ya += y.at(i);
	}
	xa /= x.size();
	ya /= y.size();
	// calculate auxiliary sums
	double xx = 0, yy = 0, xy = 0;
	for(int i = 0;i < x.size();i++)
	{
		double tmpx = x.at(i) - xa, tmpy = y.at(i) - ya;
		xx += tmpx * tmpx;
		yy += tmpy * tmpy;
		xy += tmpx * tmpy;
	}
	// calculate regression line parameters

	// make sure slope is not infinite

	if(std::fabs(xx) == 0)
	{
		slope = 0;
		intercept = 0;
		coefficient = 0;
		return false;
	}

	m_b = xy / xx;
	m_a = ya - m_b * xa;
	m_coeff = (std::fabs(yy) == 0) ? 1 : xy / std::sqrt(xx * yy);
	slope = m_b;
	intercept = m_a;
	coefficient = m_coeff;
	return true;
}

double math_stats::computePearson(QVector<double> x, QVector<double> y)
{
	double med = computeMean(x),
			med1 = computeMean(y),
			rez = 0, rez1 = 0, rez2 = 0;

	for(int i = 0;i < x.size();i++)
	{
		rez += (x.at(i) - med) * (y.at(i) - med1);
		rez1 += (x.at(i) - med) * (x.at(i) - med);
		rez2 += (y.at(i) - med1) * (y.at(i) - med1);
	}
	return rez/std::sqrt(rez1 * rez2);
}

void math_stats::designHistogram(QVector<double> data, int nBins, QVector<double> &keys, QVector<double> &frequencies, int &maxY)
{
	double min,max,leftBoundary,rightBoundary;
	for(int i = 0;i < data.size();i++)
	{
		if(i == 0)
			min = max = data.at(i);
		else
		{
			if(data.at(i) < min)
				min = data.at(i);
			if(data.at(i) > max)
				max = data.at(i);
		}
	}
	double binWidth = (max - min)/nBins;
	for(int i = 0;i < nBins;i++)
	{
		keys.push_back(i);
		frequencies.push_back(0);
	}
	int start = 0;
	for(int i = 0;i < data.size();i++)
	{
		for(int j = 0;j < nBins;j++)
		{
			leftBoundary = (j*binWidth) + min;
			rightBoundary = ((j+1)*binWidth) + min;
			if(data.at(i) > leftBoundary && data.at(i) <= rightBoundary)
			{
				frequencies[j]++;
				break;
			}
		}
	}
	maxY = 0;
	for(int i = 0;i < nBins;i++)
	{
		if(frequencies.at(i) > maxY)
			maxY = frequencies.at(i);
	}
}

bool math_stats::computeMultipleLinearRegression(QVector<QVector<double> > X,
																 QVector<double> Y,QVector<double> &Coeff)
{
	int rows = X.size();
	int cols = X.at(0).size();
	alglib::ae_int_t info;
	alglib::ae_int_t nvars;
	alglib::lrreport rep;
	alglib::real_1d_array c;
	alglib::real_2d_array data("[[]]");
	data.setlength(rows,cols+1);
	for(int i = 0;i < rows;i++)
	{
		for(int j = 0;j < cols;j++)
			data[i][j] = X.at(i).at(j);
		data[i][cols] = Y.at(i);
	}
	alglib::lrbuildz(data, rows, cols, info, model, rep);
	alglib::lrunpack(model, c, nvars);
	for(int i = 0;i < cols;i++)
		Coeff.push_back(c[i]);
}

void math_stats::processMultipleLinearRegression(QVector<double> variables, double &Y)
{
	alglib::real_1d_array x;
	x.setlength(variables.size());
	for(int i = 0;i < variables.size();i++)
		x[i] = variables.at(i);
	Y = alglib::lrprocess(model,x);
}

void math_stats::computeQuantiles(QVector<double> input, QVector<double> &output, int divisions)
{
	double temp,result;
	int size = input.size();
	std::vector<double> data(input.size());
	data = input.toStdVector();
	for(int i = 0;i <= divisions;i++)
	{
		temp = i / (divisions * 1.0);
		output.append(quantile(data,size,temp));
	}
}

double math_stats::quantile(std::vector<double> d, int size, double q)
{
  if (size == 0) return 0;
  if (size == 1) return d[0];
  if (q <= 0) return *std::min_element(std::begin(d),std::end(d));
  if (q >= 1) return *std::max_element(std::begin(d),std::end(d));

  double pos = (size - 1) * q;
  uint ind = uint(pos);
  double delta = pos - ind;
  std::vector<double> w(size); std::copy(d.begin(), d.end(), w.begin());
  std::nth_element(w.begin(), w.begin() + ind, w.end());
  double i1 = *(w.begin() + ind);
  double i2 = *std::min_element(w.begin() + ind + 1, w.end());
  return i1 * (1.0 - delta) + i2 * delta;
}

void math_stats::computeQuartiles(std::vector<double> data, double &Q1, double &Q2, double &Q3, double &min, double &max, QVector<double> &outliers)
{
  typedef std::vector<double>::size_type vecSize;
  vecSize N = data.size();
  if (N == 0)
      return; // No data!
  else
      sort(data.begin(), data.end()); // Sort vector
  double IQR;

  // declare new variables
  vecSize NMod4 = (N % 4); // identification of 1 of the 4 known datum distribution profiles
  std::string datumDistr = ""; // datum distribution profile
  vecSize M, ML, MU; // core vector indices for quartile computation
  double m, ml, mu; // quartile values are store here

  // compute quartiles for the 4 known patterns
  if (NMod4 == 0) {
      // Q1-Q3 datum distribution: [0 0 0]
      datumDistr = "[0 0 0]";
      M = N / 2;
      ML = M / 2;
      MU = M + ML;

      // grab quartile values
      ml = (data[ML] + data[ML - 1]) / 2; // datum: 0
      m = (data[M] + data[M - 1]) / 2; // datum: 0
      mu = (data[MU] + data[MU - 1]) / 2; // datum: 0
  }

  else if (NMod4 == 1) {
      // Q1-Q3 datum distribution: [0 1 0]
      datumDistr = "[0 1 0]";
      M = N / 2;
      ML = M / 2;
      MU = M + ML + 1;

      // grab quartile values
      datumDistr = "[0 0 0]";
      ml = (data[ML] + data[ML - 1]) / 2; // datum: 0
      m = data[M]; // datum: 1
      mu = (data[MU] + data[MU - 1]) / 2; // datum: 0
  }

  else if (NMod4 == 2) {
      // Q1-Q3 datum distribution: [1 0 1]
      datumDistr = "[1 0 1]";
      M = N / 2;
      ML = M / 2;
      MU = M + ML;

      // grab quartile values
      ml = data[ML]; // datum: 1
      m = (data[M] + data[M - 1]) / 2; // datum: 0
      mu = data[MU]; // datum: 1
  }

  else if (NMod4 == 3) {
      // Q1-Q3 datum distribution: [1 1 1]
      datumDistr = "[1 1 1]";
      M = N / 2;
      ML = M / 2;
      MU = M + ML + 1;

      // grab quartile values
      ml = data[ML]; // datum: 1
      m = data[M]; // datum: 0
      mu = data[MU]; // datum: 1
  }
  Q1 = ml;
  Q2 = m;
  Q3 = mu;
  IQR = Q3 - Q1;
  int lcount = 0;
  int ucount = N - 1;
  unsigned int i;
  for (i = 0; i < N; i++) {
      if (data[i] < (Q1 - (1.5 * IQR))) {
          outliers.push_back(data[i]); // lower data
          lcount = i + 1;
      }
  }
  for (i = N; i > 0; i--) {
      if (data[i - 1] > (Q3 + (1.5 * IQR))) {
          outliers.push_back(data[i - 1]); // upper data
          ucount = i - 2;
      }
  }
  min = data[lcount];
  max = data[ucount];
}
