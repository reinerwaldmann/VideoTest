#-------------------------------------------------
#
# Project created by QtCreator 2015-10-23T21:25:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ElsiTest
TEMPLATE = app

LIBS += `pkg-config opencv --libs`

SOURCES += main.cpp\
        mainwindow.cpp \
    videoprocessor.cpp

HEADERS  += mainwindow.h \
    videoprocessor.h

FORMS    += mainwindow.ui
