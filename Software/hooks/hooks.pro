#-------------------------------------------------
#
# Project created by QtCreator 2012-06-05T13:03:08
#
#-------------------------------------------------

QT       -= core gui

TARGET = lightpack-hooks
TEMPLATE = lib

include(../build-config.prf)

INCLUDEPATH += "$${DIRECTX_SDK_DIR}/Include"

QMAKE_POST_LINK = cp -f \"$(dir $(DESTDIR))lightpack-hooks.dll\" ../src/bin

LIBS += -lwsock32 -lshlwapi -ladvapi32 -L"$${DIRECTX_SDK_DIR}/Lib/x86" -ldxguid -ld3dx10 -ld3d10 -ldxgi
QMAKE_LFLAGS += --kill-at -static
QMAKE_LFLAGS_EXCEPTIONS_ON -= -mthreads
QMAKE_CXXFLAGS_EXCEPTIONS_ON -= -mthreads

DEFINES += HOOKSDLL_EXPORTS \
     UNICODE


SOURCES += \
    hooks.cpp

HEADERS += \
    hooks.h \
    ../common/D3D10GrabberDefs.hpp \
    ../common/defs.h

