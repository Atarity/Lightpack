# -------------------------------------------------
# src.pro
#
# Copyright (c) 2010,2011 Mike Shatohin, mikeshatohin [at] gmail.com
# http://lightpack.googlecode.com
# Project created by QtCreator 2010-04-28T19:08:13
# -------------------------------------------------

TARGET      = Prismatik
PRE_TARGETDEPS += ../grab/bin/libgrab.a
DESTDIR     = bin
TEMPLATE    = app
QT         += network widgets
win32 {
    QT += serialport
}

# QMake and GCC produce a lot of stuff
OBJECTS_DIR = stuff
MOC_DIR     = stuff
UI_DIR      = stuff
RCC_DIR     = stuff

# Find currect git revision
GIT_REVISION = $$system(git show -s --format="%h")

# For update GIT_REVISION use it:
#   $ qmake Lightpack.pro && make clean && make
#
# Or simply edit this file (add space anythere
# for cause to call qmake) and re-build project

isEmpty( GIT_REVISION ){
    # In code uses #ifdef GIT_REVISION ... #endif
    message( "GIT not found, GIT_REVISION will be undefined" )
} else {
    # Define current mercurial revision id
    # It will be show in about dialog and --help output
    DEFINES += GIT_REVISION=\\\"$${GIT_REVISION}\\\"
}

TRANSLATIONS += ../res/translations/ru_RU.ts \
       ../res/translations/uk_UA.ts
RESOURCES    = ../res/LightpackResources.qrc
RC_FILE      = ../res/Lightpack.rc

include(../build-config.prf)

LIBS    += -L../grab/bin -lgrab

unix:!macx{
    CONFIG    += link_pkgconfig
    PKGCONFIG += libusb-1.0 \
                 Qt5SerialPort
}

win32 {
    # Windows version using WinAPI for HID
    LIBS    += -lhid -lusbcamd -lsetupapi
    # For QSerialDevice
    LIBS    += -luuid -ladvapi32

    LIBS    += -lwsock32 -lshlwapi -lole32 -L$${DIRECTX_SDK_DIR}/Lib/x86 -ldxguid -ld3dx10 -ld3d10 -ld3d10_1 -ldxgi

    SOURCES += hidapi/windows/hid.c

    #DX9 grab
    LIBS    += -lgdi32 -ld3d9

    QMAKE_CFLAGS += -O2
    # Windows version using WinAPI + GDI + DirectX for grab colors

    LIBS    += -lwsock32 -lshlwapi -lole32

    LIBS    += -lpsapi

    QMAKE_POST_LINK = cd $(DESTDIR) && \
                cp -f \"$${QTDIR}/bin/Qt5Core$${DEBUG_EXT}.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/Qt5SerialPort$${DEBUG_EXT}.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/Qt5Widgets$${DEBUG_EXT}.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/Qt5Network$${DEBUG_EXT}.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/icudt51.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/icuin51.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/icuuc51.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/libwinpthread-1.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/libgcc_s_dw2-1.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/libstdc++-6.dll\" ./
}

unix:!macx{
    # Linux version using libusb and hidapi codes
    SOURCES += hidapi/linux/hid-libusb.c
    # For QSerialDevice
    LIBS += -ludev -lrt -lXext -lX11
}

macx{
    # MacOS version using libusb and hidapi codes
    SOURCES += hidapi/mac/hid.c
    LIBS += -framework IOKit \
            -framework Cocoa \
            -framework Carbon \
            -framework CoreFoundation \
            -framework ApplicationServices \
            -framework OpenGL \
            -L/opt/local/lib -lusb-1.0

    ICON = ../res/icons/Prismatik.icns

    # For build universal binaries (native on Intel and PowerPC)
    QMAKE_MAC_SDK = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk
}

INCLUDEPATH += ./hidapi ./grab ./alienfx ../grab/include ./

SOURCES += \
    LightpackApplication.cpp  main.cpp   SettingsWindow.cpp  Settings.cpp \
      GrabWidget.cpp  GrabConfigWidget.cpp \
    SpeedTest.cpp \
    LedDeviceLightpack.cpp \
    LedDeviceAlienFx.cpp \
    LedDeviceAdalight.cpp \
    LedDeviceArdulight.cpp \
    LedDeviceVirtual.cpp \
    ColorButton.cpp \
    ApiServer.cpp \
    ApiServerSetColorTask.cpp \
    LightpackMath.cpp \
    MoodLampManager.cpp \
    LedDeviceManager.cpp \
    SelectWidget.cpp \
    GrabManager.cpp \
    AbstractLedDevice.cpp \
    LightpackPluginInterface.cpp

HEADERS += \
    LightpackApplication.hpp \
    SettingsWindow.hpp \
    Settings.hpp \
    SettingsDefaults.hpp \
    version.h \
    TimeEvaluations.hpp \
    GrabManager.hpp \
    GrabWidget.hpp \
    GrabConfigWidget.hpp \
    debug.h \
    SpeedTest.hpp \
    alienfx/LFXDecl.h \
    alienfx/LFX2.h \
    LedDeviceLightpack.hpp \
    LedDeviceAlienFx.hpp \
    LedDeviceAdalight.hpp \
    LedDeviceArdulight.hpp \
    LedDeviceVirtual.hpp \
    ColorButton.hpp \
    ../common/defs.h \
    enums.hpp         ApiServer.hpp     ApiServerSetColorTask.hpp \
    hidapi/hidapi.h \
    ../../CommonHeaders/LIGHTPACK_HW.h \
    ../../CommonHeaders/COMMANDS.h \
    ../../CommonHeaders/USB_ID.h \
    LightpackMath.hpp \
    PluginManager.hpp \
    plugins/PyPlugin.h \    
    MoodLampManager.hpp \
    LedDeviceManager.hpp \
    SelectWidget.hpp \
    ../common/D3D10GrabberDefs.hpp \
    colorspace_types.h \
    AbstractLedDevice.hpp \
    SettingsWizard.hpp \
    LightpackPluginInterface.hpp

FORMS += SettingsWindow.ui \
    GrabWidget.ui \
    GrabConfigWidget.ui

#
#   QSerialDevice
#
#include(qserialdevice/qserialdevice/qserialdevice.pri)
#include(qserialdevice/qserialdeviceenumerator/qserialdeviceenumerator.pri)
#unix:include(qserialdevice/unix/ttylocker.pri)

#
# QtSingleApplication
#
include(qtsingleapplication/src/qtsingleapplication.pri)
