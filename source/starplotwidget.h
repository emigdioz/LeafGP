#ifndef STARPLOTWIDGET_H
#define STARPLOTWIDGET_H

#include <QWidget>
#include <math.h>
#include <QPen>

class starPlotWidget : public QWidget
{
	Q_OBJECT
public:
	starPlotWidget(QWidget *parent = 0);
	int getNumberVariables(void);
	void setGridPen(QPen pen);
	void setDataPen(QPen pen);
	void setData(QVector<double> data);
	void setSizePolygon(double value);
	void setLabels(QVector<QString> data);
	void setLabelsFont(QFont font);
	void setLabelsFontColor(QColor color);
signals:

public slots:
	void setNumberVariables(int n);

protected:
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
	void drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
					  const QString & text, QRectF * boundingRect = 0);
	void drawText(QPainter & painter, const QPointF & point, Qt::Alignment flags,
					  const QString & text, QRectF * boundingRect = 0);
	int nVertices;
	QPen gridPen;
	QPen dataPen;
	QVector<double> dataMagnitude;
	double sizePolygon;
	QVector<QString> dataLabels;
	QFont displayFont;
	QColor displayFontColor;
};

#endif // STARPLOTWIDGET_H
