#-------------------------------------------------
#
# Project created by QtCreator 2016-07-01T11:37:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LeafGP
TEMPLATE = app

INCLUDEPATH += ./source

SOURCES += source/main.cpp \
    source/mainwidget.cpp

HEADERS  += source/mainwidget.h

FORMS    += source/mainwidget.ui

RESOURCES += \
    media.qrc
