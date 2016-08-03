/*****************************************************************************
 * drawtreewidget.cpp
 *
 * Created: 7/27/2016 2016 by emigdio
 *
 * Copyright 2016 emigdio. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#include "drawtreewidget.h"
#include <QtWidgets>

drawTreeWidget::drawTreeWidget(QWidget *parent) : QWidget(parent)
{
	imageTree.fill(qRgb(255, 255, 255));
	scalable = true;
	setMouseTracking(true);
	fixedWidgetWidth = 300;
	fixedWidgetHeight = 300;
	showNames = true;
	displayFont.setPointSize(7);
	displayFontColor = QColor(255,255,255,255);
	currentNode = 0;
	lastNode = 0;
	forceUpdate = 1;
}

void drawTreeWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QRect dirtyRect = event->rect();
	currentWidth = dirtyRect.width();
	currentHeight = dirtyRect.height();
	if(!posX.empty())
	{
		if(scalable)
		{
			if(currentWidth > currentHeight)
			{
				nodeSize = currentHeight/posX.size() * 0.5;
				margin = currentHeight * 0.1;
			}
			else
			{
				nodeSize = currentWidth/posX.size() * 0.5;
				margin = currentWidth * 0.1;
			}
			widgetWidth = currentWidth - (2 * margin);
			widgetHeight = currentHeight - (2 * margin);

		}
		else
		{
			nodeSize = 10;
			widgetWidth = fixedWidgetWidth - (2 * fixedWidgetWidth * 0.1);
			widgetHeight = fixedWidgetHeight - (2 * fixedWidgetHeight * 0.1);
		}
		drawLinks();
		drawNodes();
		if(showNames)
			drawNames();
		if(forceUpdate == 2 && posX.size() > 1)
			highlightNode();
	}
	painter.drawImage(dirtyRect, imageTree, dirtyRect);
}

void drawTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
	QPoint widgetPosition = mapFromGlobal(event->globalPos());
	int X = widgetPosition.x();
	int Y = widgetPosition.y();
	int nodeX, nodeY;
	if(posX.size() > 1)
	{
		for(int i = 0;i < posX.size();i++)
		{
			nodeX = (widgetWidth * posX[i]) + (nodeSize / 2);
			nodeY = ((widgetHeight/maxDepth) * posY[i]) + (nodeSize / 2);
			//qDebug()<<nodeX<<","<<nodeY;
			if(X > nodeX && X < (nodeX + nodeSize) && Y > nodeY && Y < (nodeY + nodeSize))
			{
				//qDebug()<<"node"<<i;
				currentNode = i;
				if(!event->buttons())
				{
					forceUpdate = 2;
					update();
				}
			}
		}
	}
}

void drawTreeWidget::highlightNode()
{
	QPainter painter(&imageTree);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	painter.setBrush(nodeHoverColor);
	painter.drawEllipse((widgetWidth * posX[currentNode]) + (nodeSize / 2),
											((widgetHeight/maxDepth) * posY[currentNode]) + (nodeSize / 2),
											nodeSize,
											nodeSize);
	painter.setPen(QPen(displayFontColor));
	if(scalable)
		displayFont.setPointSize((int)(nodeSize*0.3));
	painter.setFont(displayFont);
	painter.drawText((widgetWidth * posX[currentNode]) + (nodeSize / 2),
									 ((widgetHeight/maxDepth) * posY[currentNode]) + (nodeSize / 2),
									 nodeSize,
									 nodeSize,
									 Qt::AlignVCenter | Qt::AlignHCenter,
									 name[currentNode]);

	lastNode = currentNode;
}

void drawTreeWidget::drawLinks()
{
	QPainter painter(&imageTree);
	painter.setRenderHint(QPainter::Antialiasing);
	imageTree.fill(qRgb(255, 255, 255));
	QPen linePen;
	linePen.setColor(linkColor);
	if(scalable)
		linePen.setWidth(nodeSize*0.2);
	else
		linePen.setWidth(3);
	painter.setPen(linePen);
	if(!posX.isEmpty())
	{
		int index = 0;
		int counter;
		float parentX, parentY, childrenX, childrenY;
		for(int depth = 0; depth < maxDepth;depth++)
		{
			do { // Search all nodes with the same depth
				if(posY[index] == depth)
				{
					counter = 0;
					parentX = (widgetWidth * posX[index] )+ nodeSize;
					parentY = ((widgetHeight/maxDepth) * posY[index]) + nodeSize;
					for(int k = 0;k < arity[index];k++)
					{
						do { // Search for childrens
							if(posY[index + counter] == (depth + 1))
							{
								counter += 1;
								break;
							}
							counter += 1;
						}while((index + counter) < posX.size());
						childrenX = (widgetWidth * posX[index + counter - 1]) + nodeSize ;
						childrenY = ((widgetHeight/maxDepth) * posY[index + counter - 1]) + nodeSize;
						painter.drawLine(parentX,parentY,childrenX,childrenY);
					}
				}
				index += 1;
			}while(index < posX.size());
			index = 0;
		}
	}
}

void drawTreeWidget::drawNodes()
{
	QPainter painter(&imageTree);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	painter.setBrush(nodeColor);
	if(posX.size() == 1)
	{
		painter.drawEllipse(margin,
												margin,
												nodeSize,
												nodeSize);
	}
	else
	{
		for(int i = 0;i < posX.size();i++)
		{
			painter.drawEllipse((widgetWidth * posX[i]) + (nodeSize / 2),
													((widgetHeight/maxDepth) * posY[i]) + (nodeSize / 2),
													nodeSize,
													nodeSize);
		}
	}
}

void drawTreeWidget::drawNames()
{
	QPainter painter(&imageTree);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(QPen(displayFontColor));
	if(scalable)
	{
		if((int)(nodeSize*0.3) == 0)
			return;
		displayFont.setPointSize((int)(nodeSize*0.3));
	}
	painter.setFont(displayFont);
	if(posX.size() == 1)
	{
		painter.drawText(margin,
										 margin,
										 nodeSize,
										 nodeSize,
										 Qt::AlignVCenter | Qt::AlignHCenter,
										 name[0]);

	}
	else
	{
		for(int i = 0;i < posX.size();i++)
		{
			painter.drawText((widgetWidth * posX[i]) + (nodeSize / 2),
											 ((widgetHeight/maxDepth) * posY[i]) + (nodeSize / 2),
											 nodeSize,
											 nodeSize,
											 Qt::AlignVCenter | Qt::AlignHCenter,
											 name[i]);
		}
	}
}

void drawTreeWidget::resizeEvent(QResizeEvent *event)
{
	if (width() > imageTree.width() || height() > imageTree.height()) {
		int newWidth, newHeight;
		if(scalable)
		{
			newWidth = qMax(width() + 128, imageTree.width());
			newHeight = qMax(height() + 128, imageTree.height());
		}
		else
		{
			newWidth = fixedWidgetWidth;
			newHeight = fixedWidgetHeight;
		}
		resizeImage(&imageTree, QSize(newWidth, newHeight));
		update();
	}
	QWidget::resizeEvent(event);
}

void drawTreeWidget::resizeImage(QImage *image, const QSize &newSize)
{
	if (image->size() == newSize)
		 return;
	QImage newImage(newSize, QImage::Format_RGB32);
	newImage.fill(qRgb(255, 255, 255));
	QPainter painter(&newImage);
	painter.drawImage(QPoint(0, 0), *image);
	*image = newImage;
}

void drawTreeWidget::setData(QVector<float> x, QVector<float> y, QVector<QString> n, QVector<int> a)
{
	posX = x;
	posY = y;
	name = n;
	arity = a;
	maxDepth = 0;
	for(int i = 0;i < posX.size();i++)
	{
		if(posY[i] > maxDepth)
			maxDepth = posY[i];
	}
	if(posX.size() == 1)
	{
		posX[0] = 0;
		posY[0] = 0;
	}
	currentNode = 0;
	update();
}

void drawTreeWidget::setScalable(bool value)
{
	scalable = value;
}

void drawTreeWidget::setLinkColor(QColor value)
{
	linkColor = value;
}

void drawTreeWidget::setNodeColor(QColor value)
{
	nodeColor = value;
}

void drawTreeWidget::setNameColor(QColor value)
{
	displayFontColor = value;
}

void drawTreeWidget::setNodeHoverColor(QColor value)
{
	nodeHoverColor = value;
}
