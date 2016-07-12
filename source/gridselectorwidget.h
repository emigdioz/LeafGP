#ifndef GRIDSELECTORWIDGET_H
#define GRIDSELECTORWIDGET_H

#include <QWidget>

class gridSelectorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit gridSelectorWidget(QWidget *parent = 0);
	void setNumberVariables(int n);
	void setSpacing(double n);
	void setNormalStateColor(QColor n);
	void setHoverStateColor(QColor n);
	void setSelectedStateColor(QColor n);
	void setDiagonalColor(QColor n);
	void setInitialSelection(int x, int y);
	int heightForWidth(int w) const {
		 return w;
	  }

signals:
	void sendHoverCoordinates(const int x, const int y);
	void sendSelectedCoordinates(const int x, const int y);

public slots:

protected:
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
	int nVariables;
	double spacing;
	QColor normalState;
	QColor hoverState;
	QColor selectedState;
	QColor diagonal;
	int gridWidth;
	int gridHeight;
	int hoverX;
	int hoverY;
	int selectedX;
	int selectedY;
};

#endif // GRIDSELECTORWIDGET_H
