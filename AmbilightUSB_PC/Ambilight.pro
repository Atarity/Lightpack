# -------------------------------------------------
# Project created by QtCreator 2010-04-28T19:08:13
# -------------------------------------------------
DESTDIR = ./build
TARGET = Ambilight
CONFIG += console
CONFIG -= app_bundle
LIBS += -lusb
INCLUDEPATH += ./inc
INCLUDEPATH += ../AmbilightUSB_Hardware/inc
OBJECTS_DIR = ./obj
TEMPLATE = app
SOURCES += src/hiddata.cpp \
    src/main.cpp
HEADERS += hidsdi.h \
    hiddata.h \
    RGB.h \
    usbconfig.h \
    inc/main.h
