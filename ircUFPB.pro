#-------------------------------------------------
#
# Project created by QtCreator 2016-11-10T15:39:27
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ircUFPB
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow/mainwindow.cpp \
    client/clientwindow.cpp \
    coordinator/coordinatorwindow.cpp

HEADERS  += mainwindow/mainwindow.h \
    client/clientwindow.h \
    coordinator/coordinatorwindow.h

FORMS    += mainwindow/mainwindow.ui \
    client/clientwindow.ui \
    coordinator/coordinatorwindow.ui
