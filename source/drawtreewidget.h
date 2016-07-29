/*****************************************************************************
 * drawtreewidget.h
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
#ifndef DRAWTREEWIDGET_H
#define DRAWTREEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QImage>
#include <QPoint>
#include <QColor>
#include <QString>

class drawTreeWidget : public QWidget
{
  Q_OBJECT
public:
  explicit drawTreeWidget(QWidget *parent = 0);
  void setData(QVector<float> x, QVector<float> y, QVector<QString> n, QVector<int> a);
  void setScalable(bool value);
  void setLinkColor(QColor value);
  void setNodeColor(QColor value);
  void setNameColor(QColor value);
  void setNodeHoverColor(QColor value);

signals:

protected:
  void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
  void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
  void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
  void drawLinks(void);
  void drawNodes(void);
  void drawNames(void);
  void resizeImage(QImage *image, const QSize &newSize);
  QImage imageTree;
  QVector<float> posX;
  QVector<float> posY;
  QVector<int> arity;
  QVector<QString> name;
  int currentWidth;
  int currentHeight;
  int widgetWidth;
  int widgetHeight;
  int nodeSize;
  int fixedWidgetWidth;
  int fixedWidgetHeight;
  int maxDepth;
  bool scalable;
  int margin;
  bool showNames;
  QColor linkColor;
  QColor nodeColor;
  QFont displayFont;
  QColor displayFontColor;
  QColor nodeHoverColor;
  int currentNode;
  int lastNode;
  void highlightNode(void);
  int forceUpdate;

public slots:
};

#endif // DRAWTREEWIDGET_H
