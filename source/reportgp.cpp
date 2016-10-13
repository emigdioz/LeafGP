/*****************************************************************************
 * reportgp.cpp
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
#include "reportgp.h"
#include <QDebug>

void drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
              const QString & text, QRectF * boundingRect = 0)
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

void drawText(QPainter & painter, const QPointF & point, Qt::Alignment flags,
              const QString & text, QRectF * boundingRect = 0)
{
   drawText(painter, point.x(), point.y(), flags, text, boundingRect);
}

reportGP::reportGP()
{

}

void reportGP::setFilename(QString value)
{
  filename = value;
}

void reportGP::setLogo(QImage value)
{
  logo = value;
}

void reportGP::generate()
{
  int startModules, availableSpace;
  logoMargin = 0.3;
  headerHeight = 0.1;
  logoSize = 0.1;
  copyrightBottom = 0.1;
  datetimeBottom = 0.05;
  moduleStart = 0.12;
  int iPage = 1;

  if(!filename.isEmpty())
  {
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::Letter);
    printer.setOrientation(QPrinter::Portrait);
    printer.setPageMargins (15,15,15,15,QPrinter::Millimeter);
    printer.setFullPage(false);
    printer.setOutputFileName(filename);
    printer.setOutputFormat(QPrinter::PdfFormat);

    pageWidth = printer.pageRect().width();
    pageHeight = printer.pageRect().height();
    remainingSpace = pageHeight;
    //qDebug()<<pageWidth<<" x "<<pageHeight;

    painter.begin(&printer);
    drawHeader();
    drawCopyright();
    if(includeDateTime)
      drawDateTime("Generated Tuesday, October 11,2016");

    availableSpace = pageHeight - (pageWidth * headerHeight) - (copyrightBottom * pageHeight);
    startModules = pageHeight * moduleStart;
    painter.setFont(QFont("Roboto",10));
    painter.setPen(QPen(QColor(51,51,51,255)));
    drawText(painter,QPointF(pageWidth,pageHeight),
                             Qt::AlignTop | Qt::AlignRight, "Page " + QString::number(iPage));

    if(includeBestSymbolic)
    {
      if(!drawSolutionBest(startModules,availableSpace))
      {
        startModules = 0;
        availableSpace = pageHeight;
        printer.newPage();
        iPage += 1;
        painter.setFont(QFont("Roboto",10));
        painter.setPen(QPen(QColor(51,51,51,255)));
        drawText(painter,QPointF(pageWidth,pageHeight),
                                 Qt::AlignTop | Qt::AlignRight, "Page " + QString::number(iPage));
        drawSolutionBest(startModules,availableSpace);
      }
    }
    if(includeQualityBestPlot)
    {
      if(!drawQualityBestPlot(startModules,availableSpace))
      {
        startModules = 0;
        availableSpace = pageHeight;
        printer.newPage();
        iPage += 1;
        painter.setFont(QFont("Roboto",10));
        painter.setPen(QPen(QColor(51,51,51,255)));
        drawText(painter,QPointF(pageWidth,pageHeight),
                                 Qt::AlignTop | Qt::AlignRight, "Page " + QString::number(iPage));
        drawQualityBestPlot(startModules,availableSpace);
      }
    }
    painter.end();
  }
}

void reportGP::drawHeader()
{
  if(logo.isNull())
    logo.load(":/icons/resources/images/leafgplogo.png");
  int logoWidth = logo.width();
  int logoHeight = logo.height();
  float ratioLogo = (float)logoHeight/logoWidth;
  QRectF source(logo.rect());
  QRectF target;
  if(logoWidth > logoHeight)
    target.setRect(0,
                   ((pageWidth * logoSize)/2)-(logoHeight/2),
                   pageWidth * logoSize,
                   pageWidth * logoSize * ratioLogo);
  else
    target.setRect(((pageWidth * logoSize)/2)-(logoWidth/2),
                   0,
                   pageWidth * logoSize * ratioLogo,
                   pageWidth * logoSize);

//  int startxBackground = pageWidth * logoSize + (pageWidth * logoSize * logoMargin);
//  painter.fillRect(startxBackground,
//                   0,
//                   pageWidth - startxBackground,
//                   pageHeight * headerHeight,
//                   QBrush(QColor(45,65,102,255)));
  painter.drawImage(target,logo,source);

  painter.setFont(QFont("Roboto",16));
  painter.setPen(QPen(QColor(51,51,51,255)));
  drawText(painter,QPointF(pageWidth/2,(pageWidth * headerHeight)/2),
                           Qt::AlignTop | Qt::AlignHCenter, "Project results report");
  remainingSpace -= pageWidth * logoSize;
}

void reportGP::drawCopyright()
{
  painter.setFont(QFont("Roboto",14,QFont::Light));
  painter.setPen(QPen(QColor(180,180,180,255)));
  drawText(painter,QPointF(pageWidth/2,pageHeight - (pageHeight*copyrightBottom)),
                           Qt::AlignTop | Qt::AlignHCenter, "Copyright (c) 2016, LeafGP. All rights reserved.");

}

void reportGP::drawDateTime(QString value)
{
  painter.setFont(QFont("Roboto",10));
  painter.setPen(QPen(QColor(51,51,51,255)));
  drawText(painter,QPointF(pageWidth/2,pageHeight - (pageHeight*datetimeBottom)),
                           Qt::AlignTop | Qt::AlignHCenter, value);
}

void reportGP::drawPerformancePlot()
{


//  int starty = pageHeight * moduleStart;
//  int plotWidth = performancePlot.width();
//  int plotHeight = performancePlot.height();
//  float sizeRatio = (float)plotWidth/plotHeight;

//  painter.setFont(QFont("Lato",18,QFont::Light));
//  painter.setPen(QPen(QColor(51,51,51,255)));
//  drawText(painter,QPointF(0,starty),Qt::AlignTop, "Best solution quality performance");

//  painter.drawPixmap(0,starty + 350,pageWidth,pageWidth/sizeRatio,performancePlot);
}

bool reportGP::drawQualityBestPlot(int &starty, int &availableSpace)
{
  int plotWidth = qualityBestPlot.width();
  int plotHeight = qualityBestPlot.height();
  float sizeRatio = (float)plotWidth/plotHeight;
  int requiredSpace = (pageWidth/sizeRatio) + 400;
  requiredSpace += 400;
  if(availableSpace < requiredSpace)
    return false;
  painter.setFont(QFont("Lato",16,QFont::Light));
  painter.setPen(QPen(QColor(51,51,51,255)));
  drawText(painter,QPointF(0,starty),Qt::AlignTop, "Best solution quality performance (run " + QString::number(indexRunBest + 1) + ")");
  painter.drawPixmap(0,starty + 400,pageWidth,pageWidth/sizeRatio,qualityBestPlot);
  availableSpace -= requiredSpace;
  starty += requiredSpace;
  return true;
}

bool reportGP::drawSolutionBest(int &starty, int &availableSpace)
{
  int maxChars = 100;
  int nGens = outputExperiment.gpParams.m_number_of_generations;
  //int starty = pageHeight * moduleStart;
  int startC,chunkLength, stepCounter;
  QString fullText = outputExperiment.population.at(indexRunBest).tree.at(nGens - 1).at(0).syntaxPrefix;
  int lengthText = fullText.length();
  painter.setFont(QFont("Roboto",10));
  painter.setPen(QPen(QColor(51,51,51,255)));

  QVector<QString> textChunk;

  if(lengthText > maxChars)
  {
    startC = 0;
    chunkLength = maxChars;
    do
    {
      if((startC + maxChars) > lengthText)
      {
        textChunk.append(fullText.mid(startC,lengthText - startC - 1));
        break;
      }
      while(fullText.at(startC + chunkLength - 1) != ',' && fullText.at(startC + chunkLength - 1) != '(')
      {
        chunkLength -= 1;
        if(chunkLength == 0)
          break;
      };
      textChunk.append(fullText.mid(startC,chunkLength));
      startC += chunkLength;
      chunkLength = maxChars;
    }while(startC < lengthText);
  }
  else
    textChunk.append(fullText);

  int requiredSpace = (textChunk.length() * 200) + 400;
  requiredSpace += 400; // space
  if(availableSpace < requiredSpace)
    return false;
  stepCounter = 0;
  drawText(painter,QPointF(0,starty + 400 + stepCounter),Qt::AlignTop,"Y = " + textChunk.at(0));
  stepCounter += 200;
  if(textChunk.length() > 1)
  {
    for(int i = 1;i < textChunk.length();i++)
    {
      drawText(painter,QPointF(0,starty + 400 + stepCounter),Qt::AlignTop,"      " + textChunk.at(i));
      stepCounter += 200;
    }
  }
  painter.setFont(QFont("Lato",16,QFont::Light));
  painter.setPen(QPen(QColor(51,51,51,255)));
  drawText(painter,QPointF(0,starty),Qt::AlignTop, "Best solution in symbolic form (run " + QString::number(indexRunBest + 1) + ")");
  availableSpace -= requiredSpace;
  starty += requiredSpace;
  return true;
}
