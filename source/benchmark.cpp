#include "benchmark.h"

benchmark::benchmark()
{

}

bool benchmark::generateData(int selection, QStandardItemModel &model_out)
{
	int i,j;
	double randomN,output;
	if(selection > 11) return false; // temporal
	switch(selection) {
	case 0:  // Koza-1
		nCols = 2;
		nRows = 20;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(-1,1);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = pow(rowitems.at(0)->text().toDouble(),4) +
					pow(rowitems.at(0)->text().toDouble(),3) +
					pow(rowitems.at(0)->text().toDouble(),2) +
					rowitems.at(0)->text().toDouble();
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;
	case 1:  // Koza-2
		nCols = 2;
		nRows = 20;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(-1,1);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = pow(rowitems.at(0)->text().toDouble(),5) -
					2*pow(rowitems.at(0)->text().toDouble(),3) +
					rowitems.at(0)->text().toDouble();
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;
	case 2:  // Koza-3
		nCols = 2;
		nRows = 20;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(-1,1);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = pow(rowitems.at(0)->text().toDouble(),6) -
					2*pow(rowitems.at(0)->text().toDouble(),4) +
					pow(rowitems.at(0)->text().toDouble(),2);
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;
	case 3:  // Nguyen-1
		nCols = 2;
		nRows = 20;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(-1,1);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = pow(rowitems.at(0)->text().toDouble(),3) +
					pow(rowitems.at(0)->text().toDouble(),2) +
					rowitems.at(0)->text().toDouble();
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;
	case 4:  // Nguyen-3
		nCols = 2;
		nRows = 20;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(-1,1);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = pow(rowitems.at(0)->text().toDouble(),5) +
					pow(rowitems.at(0)->text().toDouble(),4) +
					pow(rowitems.at(0)->text().toDouble(),3) +
					pow(rowitems.at(0)->text().toDouble(),2) +
					rowitems.at(0)->text().toDouble();
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;
	case 5:  // Nguyen-4
		nCols = 2;
		nRows = 20;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(-1,1);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = pow(rowitems.at(0)->text().toDouble(),6) +
					pow(rowitems.at(0)->text().toDouble(),5) +
					pow(rowitems.at(0)->text().toDouble(),4) +
					pow(rowitems.at(0)->text().toDouble(),3) +
					pow(rowitems.at(0)->text().toDouble(),2) +
					rowitems.at(0)->text().toDouble();
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;
	case 6:  // Nguyen-5
		nCols = 2;
		nRows = 20;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(-1,1);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = sin(pow(rowitems.at(0)->text().toDouble(),2)) *
					cos(rowitems.at(0)->text().toDouble()) - 1;
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;
	case 7:  // Nguyen-6
		nCols = 2;
		nRows = 20;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(-1,1);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = sin(rowitems.at(0)->text().toDouble()) +
					sin(rowitems.at(0)->text().toDouble()+pow(rowitems.at(0)->text().toDouble(),2));
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;
	case 8:  // Nguyen-7
		nCols = 2;
		nRows = 20;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(0,2);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = log(rowitems.at(0)->text().toDouble()+1) +
					log(pow(rowitems.at(0)->text().toDouble(),2)+1);
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;
	case 9:  // Nguyen-8
		nCols = 2;
		nRows = 20;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(0,4);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = sqrt(rowitems.at(0)->text().toDouble());
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;
	case 10:  // Nguyen-9
		nCols = 3;
		nRows = 100;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(0,4);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = sin(rowitems.at(0)->text().toDouble()) +
					sin(pow(rowitems.at(1)->text().toDouble(),2));
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;
	case 11:  // Nguyen-10
		nCols = 3;
		nRows = 100;
		for(i = 0;i < nRows;i++)
		{
			QList<QStandardItem*> rowitems;
			// Inputs;
			for(j = 0;j < nCols-1;j++)
			{
				randomN = rollUniform(0,4);
				QStandardItem *item = new QStandardItem(QString::number(randomN));
				rowitems.append(item);
			}
			// Output
			output = 2*sin(rowitems.at(0)->text().toDouble()) *
					cos(rowitems.at(1)->text().toDouble());
			QStandardItem *item = new QStandardItem(QString::number(output));
			rowitems.append(item);
			model_out.appendRow(rowitems);
			rowitems.clear();
		}
		break;

	}
	return true;
}

double benchmark::rollUniform(double inLow, double inUp)
{
  return inLow + ((inUp-inLow) * double(std::rand()) / double(RAND_MAX+1.0));
}

int benchmark::getRows()
{
	return nRows;
}

int benchmark::getCols()
{
	return nCols;
}
