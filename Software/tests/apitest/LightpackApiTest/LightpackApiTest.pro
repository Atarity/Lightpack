#-------------------------------------------------
#
# Project created by QtCreator 2011-09-09T12:09:24
#
#-------------------------------------------------

QT       += network testlib

QT       += gui

TARGET = LightpackApiTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../../src/ ../../../inc
SOURCES += LightpackApiTest.cpp \
    ../../../src/ApiServerSetColorTask.cpp \
    ../../../src/ApiServer.cpp
HEADERS += \
    ../../../src/inlinemath.hpp \
    ../../../src/ApiServerSetColorTask.hpp \
    ../../../src/ApiServer.hpp \
    ../../../src/apidebug.h \
    ../../../inc/debug.h


