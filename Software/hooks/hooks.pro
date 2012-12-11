#-------------------------------------------------
#
# Project created by QtCreator 2012-06-05T13:03:08
#
#-------------------------------------------------

QT       -= core gui

DESTDIR  = bin
TARGET   = prismatik-hooks
TEMPLATE = lib

include(../build-config.prf)

INCLUDEPATH += "$${DIRECTX_SDK_DIR}/Include"

QMAKE_POST_LINK = cp -f \"$(dir $(DESTDIR))prismatik-hooks.dll\" ../src/bin

LIBS += -lwsock32 -lshlwapi -ladvapi32 -L"$${DIRECTX_SDK_DIR}/Lib/x86" -ldxguid
QMAKE_LFLAGS += -static
QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas
QMAKE_LFLAGS_EXCEPTIONS_ON -= -mthreads
QMAKE_CXXFLAGS_EXCEPTIONS_ON -= -mthreads

CONFIG -= rtti

DEFINES += HOOKSDLL_EXPORTS \
     UNICODE


SOURCES += \
    hooks.cpp \
    ProxyFuncJmp.cpp \
    hooksutils.cpp \
    IPCContext.cpp \
    ProxyFuncVFTable.cpp \
    GAPIProxyFrameGrabber.cpp \
    DxgiFrameGrabber.cpp \
    Logger.cpp \
    D3D9FrameGrabber.cpp

HEADERS += \
    hooks.h \
    ../common/D3D10GrabberDefs.hpp \
    ../common/defs.h \
    ProxyFunc.hpp \
    ProxyFuncJmp.hpp \
    hooksutils.h \
    IPCContext.hpp \
    GAPISubstFunctions.hpp \
    ProxyFuncVFTable.hpp \
    res/logmessages.h \
    GAPIProxyFrameGrabber.hpp \
    DxgiFrameGrabber.hpp \
    Logger.hpp \
    LoggableTrait.hpp \
    ../common/BufferFormat.h \
    msvcstub.h \
    D3D9FrameGrabber.hpp

