#ifndef CIRCULARPROGRESSWIDGET_H
#define CIRCULARPROGRESSWIDGET_H

#include <QWidget>
#include <QPen>

class circularProgressWidget : public QWidget
{
	Q_OBJECT
public:
	explicit circularProgressWidget(QWidget *parent = 0);
	void setFont(QFont font);
	void setFontColor(QColor color);
	void setCircular1Color(QColor color);
	void setCircular2Color(QColor color);
	void setCircular1BackgroundColor(QColor color);
	void setCircular2BackgroundColor(QColor color);
	void setBackgroundColor(QColor color);
	void setDoubleProgress(bool option);

signals:

public slots:
	void setProgress1(int val);
	void setProgress2(int val);

protected:
	 void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;;

private:
	 void drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
						const QString & text, QRectF * boundingRect = 0);
	 void drawText(QPainter & painter, const QPointF & point, Qt::Alignment flags,
						const QString & text, QRectF * boundingRect = 0);
	 double progress1;
	 double progress2;
	 QColor circular1;
	 QColor circular2;
	 QColor circular1Back;
	 QColor circular2Back;
	 QColor background;
	 QFont displayFont;
	 QColor displayFontColor;
	 bool doubleGraph;
};

#endif // CIRCULARPROGRESSWIDGET_H
