# -------------------------------------------------
# AmbilightUSB
#
# Very simple implementation of the backlight for a laptop (as in Philips TV).
# Author: brunql
#
# Project created by QtCreator 2010-04-28T19:08:13
# -------------------------------------------------
DESTDIR = ./build
TARGET = AmbilightUSB
TRANSLATIONS = res/translations/AmbilightUSB_ru_RU.ts
CONFIG -= console
CONFIG -= app_bundle

win32 {
    # In windows using WinAPI for HID
    LIBS += -lhid -lusbcamd -lsetupapi
}

unix {
    # In unix using libusb
    LIBS += -lusb
}

INCLUDEPATH += ./inc
INCLUDEPATH += ../Firmware/inc/
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
    ../Firmware/inc/usbconfig.h \
    ../Firmware/inc/commands.h \
    ../Firmware/inc/RGB.h \
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
RESOURCES += res/res_ambilight.qrc
RC_FILE = res/AmbilightUSB.rc
