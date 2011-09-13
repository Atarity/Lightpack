#-------------------------------------------------
#
# Project created by QtCreator 2011-09-09T12:09:24
#
#-------------------------------------------------

QT         += network testlib

QT         += gui

TARGET      = LightpackApiTest
DESTDIR     = bin

CONFIG     += console
CONFIG     -= app_bundle

TEMPLATE    = app

# QMake and GCC produce a lot of stuff
OBJECTS_DIR = stuff
MOC_DIR     = stuff
UI_DIR      = stuff
RCC_DIR     = stuff


DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../src/ ../../src/grab
SOURCES += \
    LightpackApiTest.cpp \
    ../../src/ApiServerSetColorTask.cpp \
    ../../src/ApiServer.cpp \
    ../../src/Settings.cpp
HEADERS += \
    ../../src/inlinemath.hpp \
    ../../src/ApiServerSetColorTask.hpp \
    ../../src/ApiServer.hpp \
    ../../src/apidebug.h \
    ../../src/debug.h \
    ../../src/Settings.hpp


