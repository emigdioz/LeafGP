#include "circularprogresswidget.h"
#include <QtWidgets>

circularProgressWidget::circularProgressWidget(QWidget *parent) : QWidget(parent)
{
	doubleGraph = false;
	progress1 = 0;
	progress2 = 0;
}

void circularProgressWidget::setProgress1(int val)
{
	progress1 = (double)val/1000;
	update();
}

void circularProgressWidget::setProgress2(int val)
{
	progress2 = (double)val/1000;
	update();
}

void circularProgressWidget::setDoubleProgress(bool option)
{
	doubleGraph = option;
}

void circularProgressWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QRect redrawRect = event->rect();
	int width = redrawRect.right() - redrawRect.left();
	int height = redrawRect.bottom() - redrawRect.top();
	int marginX = width * 0.1;
	int marginY = height * 0.1;
	int marginX2 = width * 0.2;
	int marginY2 = height * 0.2;
	int marginX3 = width * 0.25;
	int marginY3 = height * 0.25;
	float width1 = 0.1;
	float width2 = 0.05;
	painter.fillRect(event->rect(), QBrush(Qt::white));
	painter.setRenderHint(QPainter::Antialiasing);
	int startAngle1 = 90 * 16;
	int spanAngle1 = progress1 * 360 * 16;
	int startAngle2 = 90 * 16;
	int spanAngle2 = progress2 * 360 * 16;

	QPen pen1,pen2;
	QBrush brush(background);

	if(width > height)
	{
		pen1.setColor(circular1Back);
		pen1.setCapStyle(Qt::FlatCap);
		pen1.setWidth(height*width1*0.95);
		painter.setPen(pen1);
		painter.drawEllipse(((width-height)/2)+marginY,marginY,height-(2*marginY),height-(2*marginY));

		pen1.setColor(circular1);
		pen1.setCapStyle(Qt::FlatCap);
		pen1.setWidth(height*width1);
		painter.setPen(pen1);
		painter.drawArc(((width-height)/2)+marginY,marginY,height-(2*marginY),height-(2*marginY),startAngle1,spanAngle1);
		if(doubleGraph)
		{
			pen2.setColor(circular2Back);
			pen2.setCapStyle(Qt::FlatCap);
			pen2.setWidth(height*width2*0.95);
			painter.setPen(pen2);
			painter.drawEllipse(((width-height)/2)+marginY2,marginY2,height-(2*marginY2),height-(2*marginY2));

			pen2.setColor(circular2);
			pen2.setCapStyle(Qt::FlatCap);
			pen2.setWidth(height*width2);
			painter.setPen(pen2);
			painter.drawArc(((width-height)/2)+marginY2,marginY2,height-(2*marginY2),height-(2*marginY2),startAngle2,spanAngle2);
		}
		painter.setBrush(brush);
		painter.setPen(Qt::NoPen);
		painter.drawEllipse(((width-height)/2)+marginY3,marginY3,height-(2*marginY3),height-(2*marginY3));
	}
	else
	{
		pen1.setColor(circular1Back);
		pen1.setCapStyle(Qt::FlatCap);
		pen1.setWidth(width*width1*0.95);
		painter.setPen(pen1);
		painter.drawEllipse(marginX,((height-width)/2)+marginX,width-(2*marginX),width-(2*marginX));

		pen1.setColor(circular1);
		pen1.setCapStyle(Qt::FlatCap);
		pen1.setWidth(width*width1);
		painter.setPen(pen1);
		painter.drawArc(marginX,((height-width)/2)+marginX,width-(2*marginX),width-(2*marginX),startAngle1,spanAngle1);
		if(doubleGraph)
		{
			pen2.setColor(circular2Back);
			pen2.setCapStyle(Qt::FlatCap);
			pen2.setWidth(width*width2*0.95);
			painter.setPen(pen2);
			painter.drawEllipse(marginX2,((height-width)/2)+marginX2,width-(2*marginX2),width-(2*marginX2));

			pen2.setColor(circular2);
			pen2.setCapStyle(Qt::FlatCap);
			pen2.setWidth(width*width2);
			painter.setPen(pen2);
			painter.drawArc(marginX2,((height-width)/2)+marginX2,width-(2*marginX2),width-(2*marginX2),startAngle2,spanAngle2);
		}
		painter.setBrush(brush);
		painter.setPen(Qt::NoPen);
		painter.drawEllipse(marginX3,((height-width)/2)+marginX3,width-(2*marginX3),width-(2*marginX3));
	}

	painter.setPen(QPen(displayFontColor));
	displayFont.setPointSize(16);
	painter.setFont(displayFont);
	drawText(painter,width/2,height/2,Qt::AlignVCenter | Qt::AlignHCenter,QString::number(progress1*100)+" %");
	if(doubleGraph)
	{
		displayFont.setPointSize(8);
		painter.setFont(displayFont);
		drawText(painter,width/2,(height/2)*1.2,Qt::AlignVCenter | Qt::AlignHCenter,QString::number(progress2*100)+" %");
	}
}

void circularProgressWidget::drawText(QPainter &painter, const QPointF &point, Qt::Alignment flags,
										const QString &text, QRectF *boundingRect)
{
	drawText(painter, point.x(), point.y(), flags, text, boundingRect);
}

void circularProgressWidget::drawText(QPainter &painter, qreal x, qreal y, Qt::Alignment flags,
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

void circularProgressWidget::setFont(QFont font)
{
	displayFont = font;
	update();
}

void circularProgressWidget::setFontColor(QColor color)
{
	displayFontColor = color;
	update();
}

void circularProgressWidget::setCircular1Color(QColor color)
{
	circular1 = color;
	update();
}

void circularProgressWidget::setCircular2Color(QColor color)
{
	circular2 = color;
	update();
}

void circularProgressWidget::setCircular1BackgroundColor(QColor color)
{
	circular1Back = color;
	update();
}

void circularProgressWidget::setCircular2BackgroundColor(QColor color)
{
	circular2Back = color;
	update();
}

void circularProgressWidget::setBackgroundColor(QColor color)
{
	background = color;
	update();
}
