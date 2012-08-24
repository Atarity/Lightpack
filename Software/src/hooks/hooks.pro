#-------------------------------------------------
#
# Project created by QtCreator 2012-06-05T13:03:08
#
#-------------------------------------------------

QT       -= core gui

TARGET = lightpack-hooks
TEMPLATE = lib

include(../src.pri)

isEmpty( DIRECTX_SDK_DIR ) {
    error("please make sure you have defined DIRECTX_SDK_DIR in your src.pri file")
}
INCLUDEPATH += "$${DIRECTX_SDK_DIR}/Include"

debug {
    QMAKE_POST_LINK = cp -f debug/lightpack-hooks.dll ../bin
}

!debug {
    QMAKE_POST_LINK = cp -f release/lightpack-hooks.dll ../bin
}

LIBS += -lwsock32 -lshlwapi -ladvapi32 -L"$${DIRECTX_SDK_DIR}/Lib/x86" -ldxguid -ld3dx10 -ld3d10 -ldxgi
QMAKE_LFLAGS +=-Wl,--kill-at,--static

DEFINES += HOOKSDLL_EXPORTS \
     UNICODE


SOURCES += \
    hooks.cpp

HEADERS += \
    hooks.h \
    ../grab/D3D10Grabber/D3D10GrabberDefs.hpp

