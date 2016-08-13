#include "dataset.h"
#include <QDebug>

Dataset::Dataset()
{
}

bool Dataset::readFile(QString fileName)
{
	QMessageBox warningBox;
	mat_t *matfp;
	matvar_t *matvar;
	int start[2] = {0,0},stride[2] = {1,1},edge[2] = {1,4};

	QFileInfo infoFile(fileName);

	if(fileName.isEmpty())
		return false;
	else
	{
		if(infoFile.completeSuffix().compare("csv") == 0)
		{
			model = new QStandardItemModel();
			QFile file (fileName);
			if (file.open(QIODevice::ReadOnly)) {
				 QString data = file.readAll();
				 data.remove( QRegExp("\r") ); //remove all ocurrences of CR (Carriage Return)
				 QString temp;
				 QChar character;
				 QTextStream textStream(&data);
				 while (!textStream.atEnd()) {
						textStream >> character;
						if (character == ',') {
							checkString(temp, character);
						} else if (character == '\n') {
							checkString(temp, character);
						} else if (textStream.atEnd()) {
							temp.append(character);
							checkString(temp);
						} else {
							temp.append(character);
						}
				 }
			}
			nRows = model->rowCount();
			nCols = model->columnCount();
			X.clear();
			Y.clear();
			minimum.clear();
			maximum.clear();
			for(int i = 0;i < nRows;i++)
			{
				X.push_back(i+1);
				Y.push_back(model->item(i,nCols-1)->text().toDouble());
				if(i == 0) Ymin = Ymax = Y.at(0);
				else {
					if(Y.at(i) < Ymin) Ymin = Y.at(i);
					if(Y.at(i) > Ymax) Ymax = Y.at(i);
				}
				for(int j = 0;j < nCols;j++)
				{
					if(i == 0)
					{
						minimum.push_back(model->item(i,j)->text().toDouble());
						maximum.push_back(model->item(i,j)->text().toDouble());
					}
					else
					{
						if(model->item(i,j)->text().toDouble() < minimum.at(j))
							minimum[j] = model->item(i,j)->text().toDouble();
						if(model->item(i,j)->text().toDouble() > maximum.at(j))
							maximum[j] = model->item(i,j)->text().toDouble();
					}
				}
			}
			return true;
		}
		if(infoFile.completeSuffix().compare("mat") == 0)
		{
			matfp = Mat_Open(fileName.toLatin1(),MAT_ACC_RDONLY);
			if ( NULL == matfp ) {
				warningBox.setText("Error opening MAT file");
				warningBox.exec();
				qDebug()<<"Error opening MAT file";
				return false;
			}
			else
			{
				bool foundVariable = false;
				while ( (matvar = Mat_VarReadNextInfo(matfp)) != NULL )
				{
					if(matvar->rank == 2)
					{
						foundVariable = true;
						nRows = matvar->dims[0];
						nCols = matvar->dims[1];
						break;
					}
				}
				if(foundVariable)
				{
					if(!matvar->isComplex)
					{
						model = new QStandardItemModel();
						double ptr[nRows*nCols];
						edge[0] = nRows;
						edge[1] = nCols;
						Mat_VarReadData(matfp,matvar,ptr,start,stride,edge);
						X.clear();
						Y.clear();
						minimum.clear();
						maximum.clear();
						for(int i = 0;i < nRows;i++)
						{
							X.push_back(i+1);
							QList<QStandardItem*> rowitems;
							for(int j = 0;j < nCols;j++)
							{
								QStandardItem *item = new QStandardItem(QString::number(ptr[(j*nRows)+i]));
								rowitems.append(item);
							}
							model->appendRow(rowitems);
							rowitems.clear();
							Y.push_back(ptr[((nCols-1)*nRows)+i]);

							if(i == 0) Ymin = Ymax = Y.at(0);
							else {
								if(Y.at(i) < Ymin) Ymin = Y.at(i);
								if(Y.at(i) > Ymax) Ymax = Y.at(i);
							}
							for(int j = 0;j < nCols;j++)
							{
								if(i == 0)
								{
									minimum.push_back(model->item(i,j)->text().toDouble());
									maximum.push_back(model->item(i,j)->text().toDouble());
								}
								else
								{
									if(model->item(i,j)->text().toDouble() < minimum.at(j))
										minimum[j] = model->item(i,j)->text().toDouble();
									if(model->item(i,j)->text().toDouble() > maximum.at(j))
										maximum[j] = model->item(i,j)->text().toDouble();
								}
							}
						}
					}
				}
				Mat_VarFree(matvar);
			}
			Mat_Close(matfp);
			return true;
		}
	}
}

void Dataset::checkString(QString &temp, QChar character)
{
	if(temp.count("\"")%2 == 0) {
		 //if (temp.size() == 0 && character != ',') //problem with line endings
		 //    return;
		 if (temp.startsWith( QChar('\"')) && temp.endsWith( QChar('\"') ) ) {
				temp.remove( QRegExp("^\"") );
				temp.remove( QRegExp("\"$") );
		 }
		 //FIXME: will possibly fail if there are 4 or more reapeating double quotes
		 temp.replace("\"\"", "\"");
		 QStandardItem *item = new QStandardItem(temp);
		 standardItemList.append(item);
		 if (character != QChar(',')) {
			  model->appendRow(standardItemList);
			  standardItemList.clear();
		 }
		 temp.clear();
	} else {
		 temp.append(character);
	}
}

int Dataset::getFeatures()
{
	return nCols-1;
}

int Dataset::getSamples()
{
	return nRows;
}

bool Dataset::readBenchmark(int choice)
{
	model = new QStandardItemModel();
	if(!dBenchmark.generateData(choice,*model))
		return false;
	else
	{
		nRows = dBenchmark.getRows();
		nCols = dBenchmark.getCols();
		X.clear();
		Y.clear();
		minimum.clear();
		maximum.clear();
		for(int i = 0;i < nRows;i++)
		{
			X.push_back(i+1);
			Y.push_back(model->item(i,nCols-1)->text().toDouble());
			if(i == 0) Ymin = Ymax = Y.at(0);
			else {
			  if(Y.at(i) < Ymin) Ymin = Y.at(i);
			  if(Y.at(i) > Ymax) Ymax = Y.at(i);
			}
			for(int j = 0;j < nCols;j++)
			{
				if(i == 0)
				{
					minimum.push_back(model->item(i,j)->text().toDouble());
					maximum.push_back(model->item(i,j)->text().toDouble());
				}
				else
				{
					if(model->item(i,j)->text().toDouble() < minimum.at(j))
						minimum[j] = model->item(i,j)->text().toDouble();
					if(model->item(i,j)->text().toDouble() > maximum.at(j))
						maximum[j] = model->item(i,j)->text().toDouble();
				}
			}
		}
		return true;
	}
}

QString Dataset::getBenchmarkName(int choice)
{
	if(choice == 0) return "Koza-1";
	if(choice == 1) return "Koza-2";
	if(choice == 2) return "Koza-3";
	if(choice == 3) return "Nguyen-1";
	if(choice == 4) return "Nguyen-3";
	if(choice == 5) return "Nguyen-4";
	if(choice == 6) return "Nguyen-5";
	if(choice == 7) return "Nguyen-6";
	if(choice == 8) return "Nguyen-7";
	if(choice == 9) return "Nguyen-8";
	if(choice == 10) return "Nguyen-9";
	if(choice == 11) return "Nguyen-10";
}

QString Dataset::getBenchmarkFunctionLatex(int choice)
{
	if(choice == 0) return "x^4+x^3+x^2+x";
	if(choice == 1) return "x^5-2x^3+x";
	if(choice == 2) return "x^6-2x^4+x^2";
	if(choice == 3) return "x^3+2x^2+x";
	if(choice == 4) return "x^5+x^4+x^3+x^2+x";
	if(choice == 5) return "x^6+x^5+x^4+x^3+x^2+x";
	if(choice == 6) return "sin(x^2)cos(x)-1";
	if(choice == 7) return "sin(x)+sin(x+x^2)";
	if(choice == 8) return "ln(x+1)+ln(x^2+1)";
	if(choice == 9) return "\\sqrt{x}";
	if(choice == 10) return "sin(x)+sin(y^2)";
	if(choice == 11) return "2sin(x)cos(y)";
}

QString Dataset::getBenchmarkRange(int choice)
{
	if(choice == 0) return "Random [-1,1]";
	if(choice == 1) return "Random [-1,1]";
	if(choice == 2) return "Random [-1,1]";
	if(choice == 3) return "Random [-1,1]";
	if(choice == 4) return "Random [-1,1]";
	if(choice == 5) return "Random [-1,1]";
	if(choice == 6) return "Random [-1,1]";
	if(choice == 7) return "Random [-1,1]";
	if(choice == 8) return "Random [0,2]";
	if(choice == 9) return "Random [0,4]";
	if(choice == 10) return "Random [-1,1]";
	if(choice == 11) return "Random [-1,1]";
}
