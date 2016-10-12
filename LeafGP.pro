#-------------------------------------------------
#
# Project created by QtCreator 2016-07-01T11:37:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = LeafGP
TEMPLATE = app
DEFINES += USE_OPENCL

contains(DEFINES, USE_OPENCL) {
LIBS += -lOpenCL
}

INCLUDEPATH += ./source \
             ./source/alglib \
             ./source/CL \
             ./source/matio \
             ./source/cmaes

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
    source/circularprogresswidget.cpp \
    source/workergp.cpp \
    source/pagegp.cpp \
    source/CPU.cpp \
    source/GP.cpp \
    source/GPU.cpp \
    source/params.cpp \
    source/primitives.cpp \
    source/drawtreewidget.cpp \
    source/populationmapwidget.cpp \
    source/matio/endian.c \
    source/matio/inflate.c \
    source/matio/io.c \
    source/matio/mat.c \
    source/matio/mat4.c \
    source/matio/mat5.c \
    source/matio/mat73.c \
    source/matio/matvar_cell.c \
    source/matio/matvar_struct.c \
    source/matio/read_data.c \
    source/matio/snprintf.c \
    source/gpexperiment.cpp \
    source/pageresults.cpp \
    source/filecontainer.cpp \
    source/reportgp.cpp

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
    source/circularprogresswidget.h \
    source/workergp.h \
    source/CPU.h \
    source/GP.h \
    source/GPU.h \
    source/params.h \
    source/primitives.h \
    source/util.h \
    source/random.h \
    source/CL/cl.h \
    source/CL/cl.hpp \
    source/CL/cl_d3d10.h \
    source/CL/cl_ext.h \
    source/CL/cl_gl.h \
    source/CL/cl_gl_ext.h \
    source/CL/cl_platform.h \
    source/CL/opencl.h \
    source/drawtreewidget.h \
    source/populationmapwidget.h \
    source/matio/mat4.h \
    source/matio/mat5.h \
    source/matio/mat73.h \
    source/matio/matio.h \
    source/matio/matio_private.h \
    source/matio/matio_pubconf.h \
    source/matio/matioConfig.h \
    source/gpexperiment.h \
    source/cmaes/cmaes.h \
    source/cmaes/parameters.h \
    source/cmaes/timings.h \
    source/cmaes/utils.h \
    source/cmaes/randomCMAES.h \
    source/filecontainer.h \
    source/reportgp.h

FORMS    += source/mainwidget.ui

RESOURCES += \
    media.qrc

DISTFILES += \
    source/matio/matioConfig.h.in
