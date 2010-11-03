# -------------------------------------------------
# Project created by QtCreator 2010-04-28T19:08:13
# -------------------------------------------------
DESTDIR = ./build
TARGET = Ambilight
TRANSLATIONS = ./translations/Ambilight_ru_RU.ts
CONFIG += console
CONFIG -= app_bundle
#WIN32 Libs
LIBS = -lhid \
 -lusbcamd \
-lsetupapi
INCLUDEPATH += ./inc
INCLUDEPATH += ../AmbilightUSB_Hardware/inc/
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
TEMPLATE = app
SOURCES += src/hiddata.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/ambilightusb.cpp \
    src/aboutdialog.cpp \
    src/grabdesktopwindowleds.cpp
HEADERS += inc/hiddata.h \
    ../AmbilightUSB_Hardware/inc/usbconfig.h \
    ../AmbilightUSB_Hardware/inc/commands.h \
    ../AmbilightUSB_Hardware/inc/RGB.h \
    inc/mainwindow.h \
    inc/ambilightusb.h \
    inc/version.h \
    inc/settings.h \
    inc/aboutdialog.h \
    inc/timeevaluations.h \
    inc/grabdesktopwindowleds.h \
    inc/struct_rgb.h \
    inc/ledcolors.h
FORMS += src/mainwindow.ui \
    src/aboutdialog.ui
RESOURCES += res_ambilight.qrc
