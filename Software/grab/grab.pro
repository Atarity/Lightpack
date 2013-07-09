#-------------------------------------------------
#
# Project created by QtCreator 2013-06-11T16:47:52
#
#-------------------------------------------------

QT       += widgets

DESTDIR = bin
TARGET = grab
TEMPLATE = lib
CONFIG += staticlib

include(../build-config.prf)

INCLUDEPATH += ./include

HEADERS += \
    include/D3D9Grabber.hpp \
    include/calculations.hpp \
    include/D3D10Grabber.hpp \
    include/X11Grabber.hpp \
    include/WinAPIGrabberEachWidget.hpp \
    include/WinAPIGrabber.hpp \
    include/TimeredGrabber.hpp \
    include/QtGrabberEachWidget.hpp \
    include/QtGrabber.hpp \
    include/MacOSGrabber.hpp \
    include/IGrabber.hpp \
    include/GrabberBase.hpp

SOURCES += \
    D3D9Grabber.cpp \
    calculations.cpp \
    D3D10Grabber.cpp \
    X11Grabber.cpp \
    WinAPIGrabberEachWidget.cpp \
    WinAPIGrabber.cpp \
    TimeredGrabber.cpp \
    QtGrabberEachWidget.cpp \
    QtGrabber.cpp \
    MacOSGrabber.cpp \
    GrabberBase.cpp

win32 {
    INCLUDEPATH += "$${DIRECTX_SDK_DIR}/Include"
}
