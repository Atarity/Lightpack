#-------------------------------------------------
#
# Project created by QtCreator 2013-06-11T16:47:52
#
#-------------------------------------------------

QT       += gui

DESTDIR = bin
TARGET = grab
TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    D3D9Grabber.hpp \
    calculations.hpp \
    D3D10Grabber/D3D10Grabber.hpp \
    X11Grabber.hpp \
    WinAPIGrabberEachWidget.hpp \
    WinAPIGrabber.hpp \
    TimeredGrabber.hpp \
    QtGrabberEachWidget.hpp \
    QtGrabber.hpp \
    MacOSGrabber.hpp \
    IGrabber.hpp \
    GrabberBase.hpp

SOURCES += \
    D3D9Grabber.cpp \
    calculations.cpp \
    D3D10Grabber/D3D10Grabber.cpp \
    X11Grabber.cpp \
    WinAPIGrabberEachWidget.cpp \
    WinAPIGrabber.cpp \
    TimeredGrabber.cpp \
    QtGrabberEachWidget.cpp \
    QtGrabber.cpp \
    MacOSGrabber.cpp \
    GrabberBase.cpp
