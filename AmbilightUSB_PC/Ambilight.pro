# -------------------------------------------------
# Project created by QtCreator 2010-04-28T19:08:13
# -------------------------------------------------
DESTDIR = ./build
TARGET = Ambilight
TRANSLATIONS = ./translations/Ambilight_ru_RU.ts
CONFIG += console
CONFIG -= app_bundle
LIBS += -lusb
INCLUDEPATH += ./inc
INCLUDEPATH += ../AmbilightUSB_Hardware/inc/
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
TEMPLATE = app
SOURCES += src/hiddata.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/ambilightusb.cpp \
    src/rectgetpixel.cpp \
    src/aboutdialog.cpp
HEADERS += inc/hidsdi.h \
    inc/hiddata.h \
    ../AmbilightUSB_Hardware/inc/RGB.h \
    ../AmbilightUSB_Hardware/inc/usbconfig.h \
    inc/mainwindow.h \
    inc/ambilightusb.h \
    inc/rectgetpixel.h \
    inc/version.h \
    inc/settings.h \
    inc/aboutdialog.h
FORMS += src/mainwindow.ui \
    src/aboutdialog.ui
RESOURCES += icons.qrc
