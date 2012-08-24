#-------------------------------------------------
#
# Project created by QtCreator 2012-07-17T17:18:38
#
#-------------------------------------------------

QT       += testlib


TARGET = GrabCalculationTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_GrabCalculationTest.cpp \
    ../../src/grab/calculations.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../src/grab/calculations.hpp \
    ../../src/enums.hpp
