#-------------------------------------------------
#
# Project created by QtCreator 2015-06-18T20:38:14
#
#-------------------------------------------------

QT       += core

QT       -= gui
QT       += network

# INCLUDEPATH += C:/dev/boost_1_58_0

TARGET = gui
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    lmsreader.cpp \
    lmsemulator.cpp

HEADERS += \
    lmsreader.h \
    lmsemulator.h
