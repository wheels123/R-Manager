#-------------------------------------------------
#
# Project created by QtCreator 2016-04-28T12:36:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#DEFINES += QT_NO_WARNING_OUTPUT\
#                   QT_NO_DEBUG_OUTPUT
QT       += network
QT += gui
CONFIG += c++11
CONFIG += qwt

TARGET = TrackLiveCC
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        qmainplot.cpp \
        qtrackclient.cpp \
    canvaspicker.cpp \
    qcurvedatacus.cpp \
    dialogoutputoption.cpp \
    qtrackserver.cpp \
    fortuneserver.cpp \
    fortunethread.cpp \
    myclient.cpp \
    mytask.cpp \
    dialogserver.cpp \
    robot.cpp \
    shapefactory.cpp \
    editshapeitem.cpp \
    control.cpp \
    twowayregion.cpp \
    onewayregion.cpp \
    robotmath.cpp

HEADERS  += mainwindow.h \
        qmainplot.h \
        qtrackclient.h \
    canvaspicker.h \
    qcurvedatacus.h \
    dialogoutputoption.h \
    qtrackserver.h \
    fortuneserver.h \
    fortunethread.h \
    myclient.h \
    mytask.h \
    dialogserver.h \
    robot.h \
    shapefactory.h \
    editshapeitem.h \
    control.h \
    twowayregion.h \
    onewayregion.h \
    robotmath.h

FORMS += \
    dialogoutputoption.ui \
    dialogserver.ui
