/*****************************************************************************
 * reportgp.h
 *
 * Created: 10/11/2016 2016 by emigdio
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
#ifndef REPORTGP_H
#define REPORTGP_H

#include <QPrinter>
#include <QPainter>
#include <QImage>
#include <QPixmap>
#include "qcustomplot.h"
#include "gpexperiment.h"

class reportGP
{
public:
  reportGP();

  void setFilename(QString value);
  void setLogo(QImage value);
  void generate(void);
  QPixmap qualityBestPlot;
  int remainingSpace;
  gpExperiment outputExperiment;
  int indexRunBest;
  bool includeLogo;
  bool includeDateTime;
  bool includeQualityBestPlot;
  bool includeBestSymbolic;
  bool includeStatistics;
  bool includePopulationInfo;
  bool includeBestOutputPlot;
  int indexPopulationInfo;

private:
  void drawHeader(void);
  void drawCopyright(void);
  void drawDateTime(QString value);
  void drawPerformancePlot(void);
  void drawSizeOverallPlot(void);

  bool drawQualityBestPlot(int &starty, int &availableSpace);
  bool drawSolutionBest(int &starty, int &availableSpace);

  QString filename;
  QImage logo;
  int pageWidth;
  int pageHeight;
  float logoMargin;
  float logoSize;
  float headerHeight;
  float copyrightBottom;
  float datetimeBottom;
  float moduleStart;
  QPainter painter;
};

#endif // REPORTGP_H
