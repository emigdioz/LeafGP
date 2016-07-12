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

bool math_stats::Regress(QVector<double> Y, QVector<QVector<double> > X)
{
	// Y[j]   = j-th observed data point
	// X[i,j] = j-th value of the i-th independent varialble
	// W[j]   = j-th weight value

	QVector<QVector<double> > V;            // Least squares and var/covar matrix
	QVector <double> C;      // Coefficients
	QVector <double> SEC;    // Std Error of coefficients
	double RYSQ;            // Multiple correlation coefficient
	double SDV;             // Standard deviation of errors
	double FReg;            // Fisher F statistic for regression
	QVector<double> Ycalc;         // Calculated values of Y
	QVector<double> DY;            // Residual values of Y
	QVector<double> W;

	int M = Y.size();             // M = Number of data points
	int N = X.at(0).size();         // N = Number of linear terms
	int NDF = M - N;              // Degrees of freedom
	//Ycalc = new double[M];
	//DY = new double[M];
	Ycalc.resize(M);
	DY.resize(M);

	for(int i = 0;i < M;i++)
		W.push_back(1);

	// If not enough data, don't attempt regression
	if (NDF < 1)
	{
		return false;
	}
	//V = new double[N, N];
	//C = new double[N];
	//SEC = new double[N];

	C.resize(N);
	SEC.resize(N);

	//double[] B = new double[N];   // Vector for LSQ
	QVector<double> B;
	B.resize(N);

	// Clear the matrices to start out
	for (int i = 0; i < N; i++)
	{
		QVector<double> temp;
		for (int j = 0; j < N; j++)
			temp.append(0);
		V.append(temp);
	}

	// Form Least Squares Matrix
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			V[i][j] = 0;
			for (int k = 0; k < M; k++)
				V[i][j] += W.at(k) * X.at(k).at(i) * X.at(k).at(j);
		}
		B[i] = 0;
		for (int k = 0; k < M; k++)
			B[i] += W.at(k) * X.at(k).at(i) * Y.at(k);
	}
	// V now contains the raw least squares matrix
	if (!symmetricMatrixInvert(V))
	{
		return false;
	}
	// V now contains the inverted least square matrix
	// Matrix multpily to get coefficients C = VB
	for (int i = 0; i < N; i++)
	{
		C[i] = 0;
		for (int j = 0; j < N; j++)
			C[i] += V.at(i).at(j) * B.at(j);
	}

	// Calculate statistics
	double TSS = 0;
	double RSS = 0;
	double YBAR = 0;
	double WSUM = 0;
	for (int k = 0; k < M; k++)
	{
		YBAR = YBAR + W.at(k) * Y.at(k);
		WSUM = WSUM + W.at(k);
	}
	YBAR = YBAR / WSUM;
	for (int k = 0; k < M; k++)
	{
		Ycalc[k] = 0;
		for (int i = 0; i < N; i++)
			Ycalc[k] += C.at(i) * X.at(k).at(i);
		DY[k] = Ycalc.at(k) - Y.at(k);
		TSS = TSS + W.at(k) * (Y.at(k) - YBAR) * (Y.at(k) - YBAR);
		RSS = RSS + W.at(k) * DY.at(k) * DY.at(k);
	}
	double SSQ = RSS / NDF;
	RYSQ = 1 - RSS / TSS;
	FReg = 9999999;
	if(RYSQ < 0.9999999)
		FReg = RYSQ / (1 - RYSQ) * NDF / (N - 1);
	SDV = std::sqrt(SSQ);

	// Calculate var-covar matrix and std error of coefficients
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			V[i][j] *= SSQ;
		SEC[i] = std::sqrt(V.at(i).at(i));
	}
	return true;
}

bool math_stats::symmetricMatrixInvert(QVector<QVector<double> > &V)
{
	 int N = (int)std::sqrt(V[0].size());
	 double *t = new double[N];
	 double *Q = new double[N];
	 double *R = new double[N];
	 double AB;
	 int K, L, M;

	 // Invert a symetric matrix in V
	 for (M = 0; M < N; M++)
		  R[M] = 1;
	 K = 0;
	 for (M = 0; M < N; M++)
	 {
		  double Big = 0;
		  for (L = 0; L < N; L++)
		  {
				AB = std::abs(V.at(L).at(L));
				if ((AB > Big) && (R[L] != 0))
				{
					 Big = AB;
					 K = L;
				}
		  }
		  if (Big == 0)
		  {
				return false;
		  }
		  R[K] = 0;
		  Q[K] = 1 / V.at(K).at(K);
		  t[K] = 1;
		  V[K][K] = 0;
		  if (K != 0)
		  {
				for (L = 0; L < K; L++)
				{
					 t[L] = V.at(L).at(K);
					 if (R[L] == 0)
						  Q[L] = V.at(L).at(K) * Q[K];
					 else
						  Q[L] = -V.at(L).at(K) * Q[K];
					 V[L][K] = 0;
				}
		  }
		  if ((K + 1) < N)
		  {
				for (L = K + 1; L < N; L++)
				{
					 if (R[L] != 0)
						  t[L] = V.at(K).at(L);
					 else
						  t[L] = -V.at(K).at(L);
					 Q[L] = -V.at(K).at(L) * Q[K];
					 V[K][L] = 0;
				}
		  }
		  for (L = 0; L < N; L++)
				for (K = L; K < N; K++)
					 V[L][K] += t[L] * Q[K];
	 }
	 M = N;
	 L = N - 1;
	 for (K = 1; K < N; K++)
	 {
		  M = M - 1;
		  L = L - 1;
		  for (int J = 0; J <= L; J++)
				V[M][J] = V.at(J).at(M);
	 }
	 return true;
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
