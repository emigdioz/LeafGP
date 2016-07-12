#include "gridselectorwidget.h"
#include <QtWidgets>
#include <QDebug>

gridSelectorWidget::gridSelectorWidget(QWidget *parent) : QWidget(parent)
{
	nVariables = 5;
	spacing = 0.1;
	hoverX = 0;
	hoverY = 0;
	selectedX = 0;
	selectedY = 0;
	setMouseTracking(true);
}

void gridSelectorWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QRect redrawRect = event->rect();
	painter.fillRect(event->rect(), QBrush(Qt::white));
	painter.setRenderHint(QPainter::Antialiasing);
	int width = redrawRect.right() - redrawRect.left();
	int height = redrawRect.bottom() - redrawRect.top();
	gridWidth = width/nVariables;
	gridHeight = height/nVariables;
	int rectWidth = gridWidth * (1 - spacing);
	int rectHeight = gridHeight * (1 - spacing);
	int x,y;

	// draw normal state grid
	for(int i = 0;i < nVariables;i++)
	{
		for(int j = 0;j < nVariables;j++)
		{
			x = (i * gridWidth) + (gridWidth * spacing/2);
			y = (j * gridHeight) + (gridHeight * spacing/2);
			if(i == j)
				painter.fillRect(x,y,rectWidth,rectHeight,QBrush(diagonal));
			else
				painter.fillRect(x,y,rectWidth,rectHeight,QBrush(normalState));
		}
	}
	// draw hover & selected states
	if(nVariables > 1)
	{
		x = (selectedX * gridWidth) + (gridWidth * spacing/2);
		y = (selectedY * gridHeight) + (gridHeight * spacing/2);
		painter.fillRect(x,y,rectWidth,rectHeight,QBrush(selectedState));
		x = (hoverX * gridWidth) + (gridWidth * spacing/2);
		y = (hoverY * gridHeight) + (gridHeight * spacing/2);
		painter.fillRect(x,y,rectWidth,rectHeight,QBrush(hoverState));
	}
}

void gridSelectorWidget::mouseMoveEvent(QMouseEvent *event)
{
	QPoint widgetPosition = mapFromGlobal(event->globalPos());
	hoverX = widgetPosition.x()/gridWidth;
	hoverY = widgetPosition.y()/gridHeight;
	if(hoverX > (nVariables - 1))
		hoverX = nVariables - 1;
	if(hoverY > (nVariables - 1))
		hoverY = nVariables - 1;
	emit sendHoverCoordinates(hoverX, hoverY);
	update();
}

void gridSelectorWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		QPoint widgetPosition = mapFromGlobal(event->globalPos());
		selectedX = widgetPosition.x()/gridWidth;
		selectedY = widgetPosition.y()/gridHeight;
		if(selectedX > (nVariables - 1))
			selectedX = nVariables - 1;
		if(selectedY > (nVariables - 1))
			selectedY = nVariables - 1;
		emit sendSelectedCoordinates(selectedX, selectedY);
		update();
	}
}

void gridSelectorWidget::setNumberVariables(int n)
{
	nVariables = n;
	update();
}

void gridSelectorWidget::setSpacing(double n)
{
	spacing = n;
	update();
}

void gridSelectorWidget::setNormalStateColor(QColor n)
{
	normalState = n;
	update();
}

void gridSelectorWidget::setHoverStateColor(QColor n)
{
	hoverState = n;
	update();
}


void gridSelectorWidget::setSelectedStateColor(QColor n)
{
	selectedState = n;
	update();
}

void gridSelectorWidget::setDiagonalColor(QColor n)
{
	diagonal = n;
	update();
}

void gridSelectorWidget::setInitialSelection(int x, int y)
{
	selectedX = x;
	selectedY = y;
	update();
}
