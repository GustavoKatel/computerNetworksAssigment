#-------------------------------------------------
#
# Project created by QtCreator 2016-11-10T15:39:27
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ircUFPB
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow/mainwindow.cpp \
    client/clientwindow.cpp \
    coordinator/coordinatorwindow.cpp \
    server/serverwindow.cpp \
    server/channel.cpp \
    server/channelslist.cpp \
    utils.cpp \
    client/connecttocoordinatordialog.cpp \
    protocol/protocol.cpp \
    dataclass/d_channel.cpp \
    dataclass/d_server.cpp \
    coordinator/coordinatorclient.cpp \
    server/serverclient.cpp

HEADERS  += mainwindow/mainwindow.h \
    client/clientwindow.h \
    coordinator/coordinatorwindow.h \
    server/serverwindow.h \
    server/channel.h \
    server/channelslist.h \
    utils.h \
    client/connecttocoordinatordialog.h \
    protocol/protocol.h \
    dataclass/d_channel.h \
    dataclass/d_server.h \
    coordinator/coordinatorclient.h \
    server/serverclient.h

FORMS    += mainwindow/mainwindow.ui \
    client/clientwindow.ui \
    coordinator/coordinatorwindow.ui \
    server/serverwindow.ui \
    client/connecttocoordinatordialog.ui

DISTFILES += \
    PROTOCOLO.md
