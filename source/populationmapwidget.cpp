#include <QtWidgets>

#include "populationmapwidget.h"

populationMapWidget::populationMapWidget(QWidget *parent)
  : QWidget(parent)
{
	totalIndividuals = 100;
	totalGenerations = 0;
	maxGenerations = 100;
	IndSize = 10; // pixels
	//addGeneration();
	setMouseTracking(true);
	imageMap.fill(qRgb(255, 255, 255));
	currentIndividual = 0;
	currentGeneration = 0;
	lastIndividual = 0;
	lastGeneration = 0;
	forceUpdate = 1;
	currentStyle = STYLE_FITNESS;
	leftMargin = 30;
	displayFont.setPointSize(7);
	individualSelected[0] = -1; // x
	individualSelected[1] = -1; // y
	inheritanceSelected = false;
	maxFitness = 0;
}


int populationMapWidget::getTotalGenerations()
{
	return maxGenerations;
}

void populationMapWidget::setTotalGenerations(int n)
{
	maxGenerations = n;
}

void populationMapWidget::setTotalIndividuals(int n)
{
	totalIndividuals = n;
}

int populationMapWidget::getTotalIndividuals()
{
	return totalIndividuals;
}

void populationMapWidget::addGeneration()
{
	QVector<Individual> newGen;
	Individual temp;
	for(int i = 0;i < totalIndividuals;i++)
	{
		double r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		int r2 = rand() % 300;
		temp.Id = i;
		temp.Fitness = r;
		if(r < 0.25)
			temp.Operator = OP_RANDOM;
		if(r >= 0.25 && r < 0.5)
			temp.Operator = OP_CROSSOVER;
		if(r >= 0.5 && r < 0.75)
			temp.Operator = OP_MUTATION;
		if(r >= 0.75 && r < 1)
			temp.Operator = OP_CLONE;
		temp.Color = Interpolate(QColor(219, 243, 255, 255),QColor(0, 84, 121, 255),r);
		temp.Size = r2;
		newGen.append(temp);
	}
	Population.append(newGen);
	totalGenerations++;
}

void populationMapWidget::addData(QList<QStringList> data)
{
	QStringList temp;
	QVector<Individual> tempGen;
	Individual tempInd;
	int count = 1;
	for(int i = 0;i < data.size();++i)
	{
		temp = data.at(i);
		tempInd.Id = temp.at(0).toInt();
		tempInd.Fitness = temp.at(1).toDouble();
		if(temp.at(2).compare("crossover") == 0)
			tempInd.Operator = OP_CROSSOVER;
		if(temp.at(2).compare("reproduction") == 0)
			tempInd.Operator = OP_CLONE;
		if(temp.at(2).compare("random") == 0)
			tempInd.Operator = OP_RANDOM;
		if(temp.at(2).compare("mutation") == 0)
			tempInd.Operator = OP_MUTATION;
		if(temp.at(2).compare("elitist") == 0)
			tempInd.Operator = OP_ELITIST;
		//tempInd.Color = Interpolate(QColor(219, 243, 255, 255),QColor(0, 84, 121, 255),tempInd.Fitness);
		tempInd.Color = QColor::fromHslF(tempInd.Fitness, 0.95, 0.5);
		tempInd.Parents[0] = temp.at(3).toInt();
		tempInd.Parents[1] = temp.at(4).toInt();
		tempGen.append(tempInd);
		if(count == totalIndividuals)
		{
			Population.append(tempGen);
			totalGenerations++;
			tempGen.clear();
			count = 0;
		}
		count++;
	}
}

void populationMapWidget::addSingleGeneration(GP::popInfo data)
{
	Individual tempInd;
	QVector<Individual> tempGen;
	int popSize = data.id.size();
	for(int i = 0;i < popSize;i++)
	{
		tempInd.Id = data.id.at(i);
		tempInd.Fitness = data.normFitness.at(i);
		tempInd.RealFitness = data.realFitness.at(i);
		tempInd.Operator = data.operatorT.at(i);
		tempInd.Parents[0] = data.parents[0].at(i);
		tempInd.Parents[1] = data.parents[1].at(i);
		tempInd.Size = data.size.at(i);
		tempInd.Color = QColor::fromHslF(tempInd.Fitness, 0.95, 0.5);
		tempGen.append(tempInd);
		if(tempInd.Fitness > maxFitness) maxFitness = tempInd.Fitness;
	}
	qSort(tempGen.begin(),tempGen.end(),[](const Individual& a, const Individual& b) { return a.Fitness > b.Fitness; });

	Population.append(tempGen);
	totalGenerations++;
	adjustSize();
	forceUpdate = 0;
	//drawLastGeneration();
	update();
	colorIndividual(QColor(255,204,0,255),0,totalGenerations-1);  // temporal
	forceUpdate = 1;
	update();
	if(totalGenerations == maxGenerations) updateScaleFitness();
}

void populationMapWidget::updateAddGeneration()
{
	addGeneration();	
	adjustSize();
	forceUpdate = 0;
	//drawLastGeneration();
	update();
	colorIndividual(QColor(255,204,0,255),0,totalGenerations-1);  // temporal
	forceUpdate = 1;
	update();
}

void populationMapWidget::updateMap()
{

	forceUpdate = 1;
	//drawAllGenerations();
	update();
}

void populationMapWidget::updateScaleFitness()
{
	for(int gen = 0;gen < totalGenerations;gen++)
	{
		for(int ind = 0;ind < totalIndividuals;ind++)
			Population[gen][ind].Color = QColor::fromHslF((Population.at(gen).at(ind).Fitness / maxFitness) * 0.8,0.95,0.5);
	}
	forceUpdate = 1;
	update();
}

void populationMapWidget::updateStyle(int value)
{
	currentStyle = value + 1;
	changeStyle();
}

void populationMapWidget::showInheritance()
{
	if(individualSelected[0] >= 0 && individualSelected[1] >= 0)
	{
		drawInheritance(individualSelected[0],individualSelected[1]);
		update();
		inheritanceSelected = true;
	}
}

void populationMapWidget::changeStyle()
{
	for(int row = 0;row < totalGenerations;row++)
	{
		for(int col = 0;col < totalIndividuals;col++)
		{
			switch(currentStyle) {
				case STYLE_FITNESS:					
					//Population[row][col].Color = Interpolate(QColor(219, 243, 255, 255),QColor(0, 84, 121, 255),Population.at(row).at(col).Fitness);
					Population[row][col].Color = QColor::fromHslF((Population.at(row).at(col).Fitness / maxFitness) * 0.8, 1, 0.5);
					break;
				case STYLE_OPERATOR:
					if(Population[row][col].Operator == OP_RANDOM)
						Population[row][col].Color = QColor(148,204,20,255);
					if(Population[row][col].Operator == OP_CROSSOVER)
						Population[row][col].Color = QColor(75,143,185,255);
					if(Population[row][col].Operator == OP_MUTATION)
						Population[row][col].Color = QColor(255,174,0,255);
					if(Population[row][col].Operator == OP_CLONE)
						Population[row][col].Color = QColor(255,85,85,255);
					if(Population[row][col].Operator == OP_ELITIST)
						Population[row][col].Color = QColor(188,95,211,255);
			}
		}
	}
	forceUpdate = 1;
	//drawAllGenerations();
	update();
}

QSize populationMapWidget::sizeHint() const
{
	 return QSize(totalIndividuals*IndSize, (65536/totalIndividuals)*IndSize);
}

void populationMapWidget::paintEvent(QPaintEvent *event)
{
	this->setMinimumHeight((IndSize*totalGenerations)+1); //Temporal
	this->setMinimumWidth((IndSize*totalIndividuals)+1+leftMargin); //Temporal
	QPainter painter(this);
	QRect dirtyRect = event->rect();
	if(forceUpdate == 0)
		drawLastGeneration();
	if(forceUpdate == 1)
		drawAllGenerations();
	painter.drawImage(dirtyRect, imageMap, dirtyRect);
}

void populationMapWidget::drawLastGeneration()
{
	QPainter painter(&imageMap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setFont(displayFont);
	int row = totalGenerations - 1;
	painter.fillRect(0,row*IndSize,leftMargin,IndSize,QBrush(QColor(215,227,244,255)));
	painter.setPen(QPen(QColor(97,145,211,255),2));
	painter.drawLine(leftMargin-1,row*IndSize,leftMargin-1,row*IndSize+IndSize);
	painter.setPen(QPen(Qt::black));
	painter.drawText(0,row*IndSize,leftMargin-3,IndSize,Qt::AlignVCenter | Qt::AlignRight,QString::number(totalGenerations-1));
	painter.setPen(Qt::NoPen);
	for(int col = 0;col < totalIndividuals;col++)
	{
		painter.setBrush(Population.at(row).at(col).Color);
		painter.drawEllipse(col*IndSize+leftMargin,row*IndSize,IndSize,IndSize);
	}
}

void populationMapWidget::drawAllGenerations()
{
	QPainter painter(&imageMap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setFont(displayFont);
	painter.setPen(Qt::NoPen);
	imageMap.fill(qRgb(255, 255, 255));

	for(int row = 0;row < totalGenerations;row++)
	{
		painter.fillRect(0,row*IndSize,leftMargin,IndSize,QBrush(QColor(215,227,244,255)));
		painter.setPen(QPen(QColor(97,145,211,255),2));
		painter.drawLine(leftMargin-1,row*IndSize,leftMargin-1,row*IndSize+IndSize);
		painter.setPen(QPen(Qt::black));
		painter.drawText(0,row*IndSize,leftMargin-3,IndSize,Qt::AlignVCenter | Qt::AlignRight,QString::number(row));
		painter.setPen(Qt::NoPen);
		for(int col = 0;col < totalIndividuals;col++)
		{
			painter.setBrush(Population.at(row).at(col).Color);
			painter.drawEllipse(col*IndSize+leftMargin,row*IndSize,IndSize,IndSize);
		}
	}
	bufferMap = imageMap.copy();
}

void populationMapWidget::drawInheritance(int x, int y)
{
	QPainter painter(&imageMap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setFont(displayFont);
	painter.setPen(Qt::NoPen);
	imageMap.fill(qRgb(255, 255, 255));
	for(int row = 0;row < totalGenerations;row++)
	{
		painter.fillRect(0,row*IndSize,leftMargin,IndSize,QBrush(QColor(215,227,244,255)));
		painter.setPen(QPen(QColor(97,145,211,255),2));
		painter.drawLine(leftMargin-1,row*IndSize,leftMargin-1,row*IndSize+IndSize);
		painter.setPen(QPen(Qt::black));
		painter.drawText(0,row*IndSize,leftMargin-3,IndSize,Qt::AlignVCenter | Qt::AlignRight,QString::number(row));
		painter.setPen(Qt::NoPen);
		for(int col = 0;col < totalIndividuals;col++)
		{
			painter.setBrush(QColor(235,235,235,255));
			painter.drawEllipse(col*IndSize+leftMargin,row*IndSize,IndSize,IndSize);
		}
	}
	QVector<bool> currentSelectionFlag(totalIndividuals);
	QVector<bool> futureSelectionFlag(totalIndividuals);
	int parent1,parent2;
	currentSelectionFlag.fill(false);
	futureSelectionFlag.fill(false);
	// selected individual first and paint it
	currentSelectionFlag[x] = true;
	for(int row = y;row > 0;row--)
	{
		// Look for the available selected individuals
		for(int col = 0;col < totalIndividuals;col++)
		{
			if(currentSelectionFlag.at(col)) // Found a selected individual
			{
				parent1 = Population[row][col].Parents[0];
				parent2 = Population[row][col].Parents[1];
				// Look up for parents on previous generation
				for(int col2 = 0;col2 < totalIndividuals;col2++)
				{
					if(Population[row-1][col2].Id == parent1 || Population[row-1][col2].Id == parent2) // Found!
					{
						futureSelectionFlag[col2] = true; // Mark it
						// draw link
						painter.setPen(QPen(QColor(255,0,0,255),1));
						painter.drawLine(col*IndSize+leftMargin+(IndSize/2),row*IndSize+(IndSize/2),
											  col2*IndSize+leftMargin+(IndSize/2),(row-1)*IndSize+(IndSize/2));
					}
				}
				// draw node
				painter.setPen(Qt::NoPen);
				//painter.fillRect(col2*IndSize+leftMargin,(row-1)*IndSize,IndSize,IndSize,QBrush(Qt::white));
				painter.setBrush(Population.at(row).at(col).Color);
				painter.drawEllipse(col*IndSize+leftMargin,row*IndSize,IndSize,IndSize);
			}
		}
		currentSelectionFlag = futureSelectionFlag;
		futureSelectionFlag.fill(false);
	}
	for(int col = 0;col < totalIndividuals;col++)
	{
		if(currentSelectionFlag.at(col)) // Found a selected individual
		{
			// draw node
			painter.setPen(Qt::NoPen);
			//painter.fillRect(col2*IndSize+leftMargin,(row-1)*IndSize,IndSize,IndSize,QBrush(Qt::white));
			painter.setBrush(Population.at(0).at(col).Color);
			painter.drawEllipse(col*IndSize+leftMargin,0,IndSize,IndSize);
		}
	}
	bufferMap = imageMap.copy();
}

void populationMapWidget::highlightIndividual(void)
{
	QPainter painter(&imageMap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	if(lastIndividual != individualSelected[0] || lastGeneration != individualSelected[1]) // If some individual has been selected then avoid repaint over
	{
		QImage localBuffer = bufferMap.copy(lastIndividual*IndSize+leftMargin,lastGeneration*IndSize,IndSize,IndSize);
		QPixmap pixmap;
		pixmap.convertFromImage(localBuffer);
		painter.drawPixmap(lastIndividual*IndSize+leftMargin,lastGeneration*IndSize,pixmap);
	}
	if(currentIndividual != individualSelected[0] || currentGeneration != individualSelected[1]) // If some individual has been selected then avoid repaint over
	{
		painter.setBrush(QColor(255, 255, 255, 255));
		painter.drawEllipse(currentIndividual*IndSize+2+leftMargin,currentGeneration*IndSize+2,IndSize-4,IndSize-4);
	}
	lastIndividual = currentIndividual;
	lastGeneration = currentGeneration;
}

void populationMapWidget::mouseMoveEvent(QMouseEvent *event)
{	
	QPoint widgetPosition = mapFromGlobal(event->globalPos());
	int posX = (widgetPosition.x()-leftMargin)/IndSize;
	int posY = widgetPosition.y()/IndSize;
	if(posX<totalIndividuals && posX >= 0 && posY<totalGenerations)
	{
		QString op;
		if(Population.at(posY).at(posX).Operator == OP_RANDOM) op = "Random";
		if(Population.at(posY).at(posX).Operator == OP_CROSSOVER) op = "Crossover";
		if(Population.at(posY).at(posX).Operator == OP_MUTATION) op = "Mutation";
		if(Population.at(posY).at(posX).Operator == OP_CLONE) op = "Reproduction";
		if(Population.at(posY).at(posX).Operator == OP_ELITIST) op = "Elitist";

		QString text = QString::fromLatin1("<p>Id: ") +
				QString::number(Population.at(posY).at(posX).Id) +
				QString::fromLatin1("</p><p>Fitness: ") +
				QString::number(Population.at(posY).at(posX).RealFitness) +
				QString::fromLatin1("</p><p>Operator: ") +
				op + QString::fromLatin1("</p><p>Size: ") +
				QString::number(Population.at(posY).at(posX).Size) +
				QString::fromLatin1("</p>");
		QToolTip::showText(event->globalPos(), text, this);
		currentIndividual = posX;
		currentGeneration = posY;
		if(!event->buttons())
		{
			forceUpdate = 2;
			highlightIndividual();
			update();
		}
	}
}

void populationMapWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		QPoint widgetPosition = mapFromGlobal(event->globalPos());
		int posX = (widgetPosition.x()-leftMargin)/IndSize;
		int posY = widgetPosition.y()/IndSize;
		if(posX<totalIndividuals && posX >= 0 && posY<totalGenerations)
		{
			QPainter painter(&imageMap);
			painter.setRenderHint(QPainter::Antialiasing);
			painter.setPen(Qt::NoPen);
			if(individualSelected[0] >= 0 && individualSelected[1] >= 0) // Avoid first time selection invalid pointer
			{
				QImage localBuffer = bufferMap.copy(individualSelected[0]*IndSize+leftMargin,individualSelected[1]*IndSize,IndSize,IndSize);
				QPixmap pixmap;
				pixmap.convertFromImage(localBuffer);
				painter.drawPixmap(individualSelected[0]*IndSize+leftMargin,individualSelected[1]*IndSize,pixmap);
			}
			individualSelected[0] = posX;
			individualSelected[1] = posY;
			painter.setPen(Qt::NoPen);
			painter.fillRect(lastIndividual*IndSize+leftMargin,lastGeneration*IndSize,IndSize,IndSize,QBrush(Qt::white));
			painter.setBrush(Population.at(lastGeneration).at(lastIndividual).Color);
			painter.drawEllipse(lastIndividual*IndSize+leftMargin,lastGeneration*IndSize,IndSize,IndSize);
			QPen pen(Qt::white);
			pen.setWidth(2);
			painter.setPen(pen);
			painter.drawLine(currentIndividual*IndSize+(IndSize/2)+leftMargin,currentGeneration*IndSize+1,currentIndividual*IndSize+(IndSize/2)+leftMargin,currentGeneration*IndSize+IndSize-1);
			painter.drawLine(currentIndividual*IndSize+1+leftMargin,currentGeneration*IndSize+(IndSize/2),currentIndividual*IndSize+IndSize-1+leftMargin,currentGeneration*IndSize+(IndSize/2));
			update();
		}
	}
	if (event->button() == Qt::RightButton)
	{
		individualSelected[0] = -1;
		individualSelected[1] = -1;
		forceUpdate = 1;
		update();
	}
}

void populationMapWidget::resizeEvent(QResizeEvent *event)
{
	 if (width() > imageMap.width() || height() > imageMap.height()) {
		  int newWidth = qMax(width() + 128, imageMap.width());
		  int newHeight = qMax(height() + 128, imageMap.height());
		  resizeImage(&imageMap, QSize(newWidth, newHeight));
		  update();
	 }
	 QWidget::resizeEvent(event);
}

QColor populationMapWidget::Interpolate(QColor startC, QColor endC, double value)
{
	int red = (int)(value*startC.red() + (1-value)*endC.red());
	int green = (int)(value*startC.green() + (1-value)*endC.green());
	int blue = (int)(value*startC.blue() + (1-value)*endC.blue());
	QColor result;
	result.setRed(red);
	result.setGreen(green);
	result.setBlue(blue);
	return result;
}

void populationMapWidget::resizeImage(QImage *image, const QSize &newSize)
{
	 if (image->size() == newSize)
		  return;

	 QImage newImage(newSize, QImage::Format_RGB32);
	 newImage.fill(qRgb(255, 255, 255));
	 QPainter painter(&newImage);
	 painter.drawImage(QPoint(0, 0), *image);
	 *image = newImage;
}

void populationMapWidget::colorIndividual(QColor value, int Ind, int Gen)
{
	QPainter painter(&imageMap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	painter.setBrush(value);
	painter.drawEllipse(Ind*IndSize+leftMargin,Gen*IndSize,IndSize,IndSize);
	Population[Gen][Ind].Color = value;
}

void populationMapWidget::clearPopulation()
{
	Population.clear();
	totalGenerations = 0;
}
