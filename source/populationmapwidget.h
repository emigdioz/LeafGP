#ifndef POPULATIONMAPWIDGET_H
#define POPULATIONMAPWIDGET_H

#include <QWidget>
#include <QVector>
#include <QImage>
#include <QPoint>
#include <QColor>
#include <QString>
#include "GP.h"
#include "gpexperiment.h"

QT_BEGIN_NAMESPACE
class QMouseEvent;
class QPaintEvent;
QT_END_NAMESPACE

#define OP_RANDOM    1
#define OP_CROSSOVER 2
#define OP_MUTATION  3
#define OP_CLONE     4
#define OP_ELITIST   5

#define STYLE_FITNESS  1
#define STYLE_OPERATOR 2
#define STYLE_SIZE     3

class populationMapWidget : public QWidget
{
  Q_OBJECT
public:
  populationMapWidget(QWidget *parent = 0);
  QSize sizeHint() const Q_DECL_OVERRIDE;
  void addData(gpExperiment::popData input);
  void addSingleGeneration(GP::popInfo data);

signals:
  void individualIsSelected(int &ind, int &gen);

protected:
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

public slots:
	void updateAddGeneration(void);
	void updateMap(void);
	void showInheritance(void);
	void updateStyle(int value);

public:
	unsigned int numberIndividuals;
	void addGeneration(void);
	void setTotalGenerations(int n);
	int getTotalGenerations(void);
	void setTotalIndividuals(int n);
	int getTotalIndividuals(void);
	void clearPopulation(void);
	void colorIndividual(QColor value, int Ind, int Gen);
	void setVisualizationStyle(int style);
	void updateScaleFitness(void);

private:
	QColor Interpolate(QColor startC, QColor endC, double value);
	void drawLastGeneration(void);
	void drawAllGenerations(void);
	void highlightIndividual(void);
	void drawInheritance(int x, int y);
	void changeStyle(void);
	void resizeImage(QImage *image, const QSize &newSize);
	int totalGenerations;
	int maxGenerations;
	int totalIndividuals;
	struct Individual
	{
		int Id;
		float Fitness;
		float RealFitness;
		int Operator;
		int Size;
		int Parents[2];
		QColor Color;
	};
	QVector<QVector<Individual> > Population;
	int IndSize;
	int lastIndividual;
	int lastGeneration;
	int currentIndividual;
	int currentGeneration;
	QImage imageMap;
	QImage bufferMap;
	int forceUpdate; // 0 last generation, 1 all map, 2 none
	int currentStyle;
	int leftMargin;
	QFont displayFont;
	int individualSelected[2];
	bool inheritanceSelected;
	float maxFitness;
	int maxSize;
};

#endif // POPULATIONMAPWIDGET_H
