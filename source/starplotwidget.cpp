#include "starplotwidget.h"
#include <QtWidgets>
#include <QDebug>

starPlotWidget::starPlotWidget(QWidget *parent) : QWidget(parent)
{
	nVertices = 5;
	sizePolygon = 1;
}

void starPlotWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	double radius100,radius75,radius50,radius25;
	int tempx,tempy,x1,x2,y1,y2;
	QPointF points100[nVertices];
	QPointF points75[nVertices];
	QPointF points50[nVertices];
	QPointF points25[nVertices];
	QRect redrawRect = event->rect();
	painter.fillRect(event->rect(), QBrush(Qt::white));
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(gridPen);
	int width = redrawRect.right() - redrawRect.left();
	int height = redrawRect.bottom() - redrawRect.top();
	//qDebug()<<width<<" x "<<height;
	if(width < height)
	{
		radius100 = (width/2) * sizePolygon;
		radius75 = radius100 * 0.75;
		radius50 = radius100 * 0.5;
		radius25 = radius100 * 0.25;
	}
	else
	{
		radius100 = (height/2) * sizePolygon;
		radius75 = radius100 * 0.75;
		radius50 = radius100 * 0.5;
		radius25 = radius100 * 0.25;
	}
	// draw grid
	for(int i = 0;i < nVertices;i++)
	{
		tempx = radius100 * cos(2*M_PI*(i+1)/nVertices) + (width/2);
		tempy = radius100 * sin(2*M_PI*(i+1)/nVertices) + (height/2);
		points100[i] = QPointF(tempx,tempy);
		painter.drawLine(width/2,height/2,tempx,tempy);
		tempx = radius75 * cos(2*M_PI*(i+1)/nVertices) + (width/2);
		tempy = radius75 * sin(2*M_PI*(i+1)/nVertices) + (height/2);
		points75[i] = QPointF(tempx,tempy);
		tempx = radius50 * cos(2*M_PI*(i+1)/nVertices) + (width/2);
		tempy = radius50 * sin(2*M_PI*(i+1)/nVertices) + (height/2);
		points50[i] = QPointF(tempx,tempy);
		tempx = radius25 * cos(2*M_PI*(i+1)/nVertices) + (width/2);
		tempy = radius25 * sin(2*M_PI*(i+1)/nVertices) + (height/2);
		points25[i] = QPointF(tempx,tempy);
	}
	painter.drawPolygon(points100, nVertices);
	painter.drawPolygon(points75, nVertices);
	painter.drawPolygon(points50, nVertices);
	painter.drawPolygon(points25, nVertices);
	// draw data
	painter.setPen(dataPen);
	if(dataMagnitude.size() == nVertices)
	{
		for(int i = 0;i < (dataMagnitude.size() - 1);i++)
		{
			x1 = (radius100 * dataMagnitude.at(i)) * cos(2*M_PI*(i+1)/nVertices) + (width/2);
			y1 = (radius100 * dataMagnitude.at(i)) * sin(2*M_PI*(i+1)/nVertices) + (height/2);
			x2 = (radius100 * dataMagnitude.at(i+1)) * cos(2*M_PI*(i+2)/nVertices) + (width/2);
			y2 = (radius100 * dataMagnitude.at(i+1)) * sin(2*M_PI*(i+2)/nVertices) + (height/2);
			painter.drawLine(x1,y1,x2,y2);
		}
		x1 = (radius100 * dataMagnitude.at(dataMagnitude.size()-1)) * cos(2*M_PI*(dataMagnitude.size())/nVertices) + (width/2);
		y1 = (radius100 * dataMagnitude.at(dataMagnitude.size()-1)) * sin(2*M_PI*(dataMagnitude.size())/nVertices) + (height/2);
		x2 = (radius100 * dataMagnitude.at(0)) * cos(2*M_PI*(1)/nVertices) + (width/2);
		y2 = (radius100 * dataMagnitude.at(0)) * sin(2*M_PI*(1)/nVertices) + (height/2);
		painter.drawLine(x1,y1,x2,y2);
	}
	// draw labels
	painter.setFont(displayFont);
	painter.setPen(QPen(displayFontColor));
	if(dataLabels.size() == nVertices)
	{
		for(int i = 0;i < nVertices;i++)
		{
			tempx = radius100 * 1.2 * cos(2*M_PI*(i+1)/nVertices) + (width/2);
			tempy = radius100 * 1.2 * sin(2*M_PI*(i+1)/nVertices) + (height/2);
			painter.drawText(tempx,tempy,dataLabels.at(i));
		}
	}
	else
	{
		for(int i = 0;i < nVertices;i++)
		{
			tempx = radius100 * 1.2 * cos(2*M_PI*(i+1)/nVertices) + (width/2);
			tempy = radius100 * 1.2 * sin(2*M_PI*(i+1)/nVertices) + (height/2);
			drawText(painter,tempx,tempy,Qt::AlignVCenter | Qt::AlignHCenter,"X" + QString::number(i + 1));
		}
	}
}

void starPlotWidget::setNumberVariables(int n)
{
	nVertices = n;
	update();
}

int starPlotWidget::getNumberVariables()
{
	return nVertices;
}


void starPlotWidget::setGridPen(QPen pen)
{
	gridPen = pen;
	update();
}

void starPlotWidget::setDataPen(QPen pen)
{
	dataPen = pen;
	update();
}

void starPlotWidget::setData(QVector<double> data)
{
	dataMagnitude.clear();
	for(int i = 0;i < data.size();i++)
		dataMagnitude.push_back(data.at(i));
	update();
}

void starPlotWidget::setSizePolygon(double value)
{
	sizePolygon = value;
	update();
}

void starPlotWidget::setLabelsFont(QFont font)
{
	displayFont = font;
	update();
}

void starPlotWidget::setLabelsFontColor(QColor color)
{
	displayFontColor = color;
	update();
}

void starPlotWidget::drawText(QPainter &painter, const QPointF &point, Qt::Alignment flags,
										const QString &text, QRectF *boundingRect)
{
	drawText(painter, point.x(), point.y(), flags, text, boundingRect);
}

void starPlotWidget::drawText(QPainter &painter, qreal x, qreal y, Qt::Alignment flags,
										const QString &text, QRectF *boundingRect)
{
	const qreal size = 32767.0;
	QPointF corner(x, y - size);
	if (flags & Qt::AlignHCenter) corner.rx() -= size/2.0;
	else if (flags & Qt::AlignRight) corner.rx() -= size;
	if (flags & Qt::AlignVCenter) corner.ry() += size/2.0;
	else if (flags & Qt::AlignTop) corner.ry() += size;
	else flags |= Qt::AlignBottom;
	QRectF rect(corner, QSizeF(size, size));
	painter.drawText(rect, flags, text, boundingRect);
}
