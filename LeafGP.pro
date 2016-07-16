#-------------------------------------------------
#
# Project created by QtCreator 2016-07-01T11:37:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = LeafGP
TEMPLATE = app

INCLUDEPATH += ./source \
             ./source/alglib

SOURCES += source/main.cpp \
    source/mainwidget.cpp \
    source/dataset.cpp \
    source/qcustomplot.cpp \
    source/benchmark.cpp \
    source/jkqtptools.cpp \
    source/jkqtmathtext.cpp \
    source/math_stats.cpp \
    source/kde.cpp \
    source/starplotwidget.cpp \
    source/gridselectorwidget.cpp \
    source/alglib/alglibinternal.cpp \
    source/alglib/alglibmisc.cpp \
    source/alglib/ap.cpp \
    source/alglib/dataanalysis.cpp \
    source/alglib/diffequations.cpp \
    source/alglib/fasttransforms.cpp \
    source/alglib/integration.cpp \
    source/alglib/interpolation.cpp \
    source/alglib/linalg.cpp \
    source/alglib/optimization.cpp \
    source/alglib/solvers.cpp \
    source/alglib/specialfunctions.cpp \
    source/alglib/statistics.cpp \
    source/listwidget.cpp \
    source/pagesetup.cpp \
    source/pagedata.cpp \
    source/circularprogresswidget.cpp

HEADERS  += source/mainwidget.h \
    source/dataset.h \
    source/qcustomplot.h \
    source/benchmark.h \
    source/jkqtptools.h \
    source/jkqtp_imexport.h \
    source/jkqtmathtext.h \
    source/math_stats.h \
    source/kde.h \
    source/starplotwidget.h \
    source/gridselectorwidget.h \
    source/alglib/alglibinternal.h \
    source/alglib/alglibmisc.h \
    source/alglib/ap.h \
    source/alglib/dataanalysis.h \
    source/alglib/diffequations.h \
    source/alglib/fasttransforms.h \
    source/alglib/integration.h \
    source/alglib/interpolation.h \
    source/alglib/linalg.h \
    source/alglib/optimization.h \
    source/alglib/solvers.h \
    source/alglib/specialfunctions.h \
    source/alglib/statistics.h \
    source/alglib/stdafx.h \
    source/listwidget.h \
    source/circularprogresswidget.h

FORMS    += source/mainwidget.ui

RESOURCES += \
    media.qrc
