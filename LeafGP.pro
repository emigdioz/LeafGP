#-------------------------------------------------
#
# Project created by QtCreator 2016-07-01T11:37:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = LeafGP
TEMPLATE = app

INCLUDEPATH += ./source

SOURCES += source/main.cpp \
    source/mainwidget.cpp \
    source/dataset.cpp \
    source/qcustomplot.cpp \
    source/benchmark.cpp \
    source/jkqtptools.cpp \
    source/jkqtmathtext.cpp \
    source/math_stats.cpp \
    source/kde.cpp \
    source/starplotwidget.cpp

HEADERS  += source/mainwidget.h \
    source/dataset.h \
    source/qcustomplot.h \
    source/benchmark.h \
    source/jkqtptools.h \
    source/jkqtp_imexport.h \
    source/jkqtmathtext.h \
    source/math_stats.h \
    source/kde.h \
    source/starplotwidget.h

FORMS    += source/mainwidget.ui

RESOURCES += \
    media.qrc
