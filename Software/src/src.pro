# -------------------------------------------------
# src.pro
#
# Copyright (c) 2010,2011 Mike Shatohin, mikeshatohin [at] gmail.com
# http://lightpack.googlecode.com
# Project created by QtCreator 2010-04-28T19:08:13
# -------------------------------------------------

TARGET      = Prismatik
CONFIG(msvc) {
    PRE_TARGETDEPS += ../lib/grab.lib
} else {
    PRE_TARGETDEPS += ../lib/libgrab.a
}
DESTDIR     = bin
TEMPLATE    = app
QT         += network widgets
win32 {
    QT += serialport
}
macx {
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

LIBS    += -L../lib -lgrab -lprismatik-math
CONFIG(gcc):QMAKE_CXXFLAGS += -std=c++11

unix:!macx{
    CONFIG    += link_pkgconfig
    PKGCONFIG += libusb-1.0

    DESKTOP = $$(XDG_CURRENT_DESKTOP)

    equals(DESKTOP, "Unity") {
        DEFINES += UNITY_DESKTOP
        PKGCONFIG += gtk+-2.0 appindicator-0.1 libnotify
    }

    INCLUDEPATH += ../qtserialport/include
    LIBS += -L../qtserialport/lib -lQt5SerialPort
    QMAKE_LFLAGS += -Wl,-rpath=/usr/lib/prismatik
}

win32 {
    CONFIG(msvc):DEFINES += _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE
    # Windows version using WinAPI for HID
    LIBS    += -lsetupapi
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
    LIBS    += -lwtsapi32

    CONFIG(msvc) {
        QMAKE_POST_LINK = cd $(DESTDIR) && \
                cp -f \"../../lib/prismatik-hooks.dll\" ./ && \
                cp -f \"../../lib/libraryinjector.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/Qt5Core$${DEBUG_EXT}.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/Qt5Gui$${DEBUG_EXT}.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/Qt5SerialPort$${DEBUG_EXT}.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/Qt5Widgets$${DEBUG_EXT}.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/Qt5Network$${DEBUG_EXT}.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/icudt51.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/icuin51.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/icuuc51.dll\" ./
    } else {
        QMAKE_POST_LINK = cd $(DESTDIR) && \
                cp -f \"../../lib/prismatik-hooks.dll\" ./ && \
                cp -f \"../../lib/libraryinjector.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/Qt5Core$${DEBUG_EXT}.dll\" ./ && \
                cp -f \"$${QTDIR}/bin/Qt5Gui$${DEBUG_EXT}.dll\" ./ && \
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
}

unix:!macx{
    # Linux version using libusb and hidapi codes
    SOURCES += hidapi/linux/hid-libusb.c
    # For QSerialDevice
    LIBS += -ludev -lrt -lXext -lX11
}

macx{
    QMAKE_LFLAGS += -F/System/Library/Frameworks
    # MacOS version using libusb and hidapi codes
    SOURCES += hidapi/mac/hid.c
    LIBS += \
            -framework Cocoa \
            -framework Carbon \
            -framework CoreFoundation \
            #-framework CoreServices \
            -framework Foundation \
 #           -framework CoreGraphics \
            -framework ApplicationServices \
            -framework OpenGL \

    ICON = ../res/icons/Prismatik.icns

    QMAKE_INFO_PLIST = ./Info.plist

    # For build universal binaries (native on Intel and PowerPC)
    QMAKE_MAC_SDK = macosx10.9

    CONFIG(clang) {
        QMAKE_CXXFLAGS += -mmacosx-version-min=10.9 -stdlib=libc+ -x objective-c++
        CONFIG +=c++11
    }
}

INCLUDEPATH += ./hidapi ./grab ./alienfx ../grab/include ../math/include ./

SOURCES += \
    LightpackApplication.cpp  main.cpp   SettingsWindow.cpp  Settings.cpp \
      GrabWidget.cpp  GrabConfigWidget.cpp \
    SpeedTest.cpp \
    LedDeviceLightpack.cpp \
    LedDeviceAdalight.cpp \
    LedDeviceArdulight.cpp \
    LedDeviceVirtual.cpp \
    ColorButton.cpp \
    ApiServer.cpp \
    ApiServerSetColorTask.cpp \
    MoodLampManager.cpp \
    LedDeviceManager.cpp \
    SelectWidget.cpp \
    GrabManager.cpp \
    AbstractLedDevice.cpp \
    PluginsManager.cpp \
    Plugin.cpp \
    LightpackPluginInterface.cpp \
    TimeEvaluations.cpp \
    EndSessionDetector.cpp \
    wizard/ZoneWidget.cpp \
    wizard/ZonePlacementPage.cpp \
    wizard/Wizard.cpp \
    wizard/SelectProfilePage.cpp \
    wizard/MonitorIdForm.cpp \
    wizard/MonitorConfigurationPage.cpp \
    wizard/LightpackDiscoveryPage.cpp \
    wizard/GrabAreaWidget.cpp \
    wizard/AndromedaDistributor.cpp \
    wizard/ConfigureDevicePage.cpp \
    wizard/SelectDevicePage.cpp \
    wizard/CassiopeiaDistributor.cpp \
    wizard/PegasusDistributor.cpp \
    systrayicon/SysTrayIcon.cpp \
    UpdatesProcessor.cpp

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
    MoodLampManager.hpp \
    LedDeviceManager.hpp \
    SelectWidget.hpp \
    ../common/D3D10GrabberDefs.hpp \
    AbstractLedDevice.hpp \
    PluginsManager.hpp \
    Plugin.hpp \
    LightpackPluginInterface.hpp \
    EndSessionDetector.hpp \
    wizard/ZoneWidget.hpp \
    wizard/ZonePlacementPage.hpp \
    wizard/Wizard.hpp \
    wizard/SettingsAwareTrait.hpp \
    wizard/SelectProfilePage.hpp \
    wizard/MonitorIdForm.hpp \
    wizard/MonitorConfigurationPage.hpp \
    wizard/LightpackDiscoveryPage.hpp \
    wizard/GrabAreaWidget.hpp \
    wizard/AndromedaDistributor.hpp \
    wizard/ConfigureDevicePage.hpp \
    wizard/SelectDevicePage.hpp \
    types.h \
    wizard/AreaDistributor.hpp \
    wizard/CassiopeiaDistributor.hpp \
    wizard/PegasusDistributor.hpp \
    systrayicon/SysTrayIcon.hpp \
    systrayicon/SysTrayIcon_p.hpp \
    UpdatesProcessor.hpp

!contains(DEFINES,UNITY_DESKTOP) {
    HEADERS += systrayicon/SysTrayIcon_qt_p.hpp
}

contains(DEFINES,UNITY_DESKTOP) {
    HEADERS += systrayicon/SysTrayIcon_unity_p.hpp
}

win32 {
    SOURCES += LedDeviceAlienFx.cpp
    HEADERS += LedDeviceAlienFx.hpp
}

FORMS += SettingsWindow.ui \
    GrabWidget.ui \
    GrabConfigWidget.ui \
    wizard/ZoneWidget.ui \
    wizard/ZonePlacementPage.ui \
    wizard/Wizard.ui \
    wizard/SelectProfilePage.ui \
    wizard/MonitorIdForm.ui \
    wizard/MonitorConfigurationPage.ui \
    wizard/LightpackDiscoveryPage.ui \
    wizard/GrabAreaWidget.ui \
    wizard/ConfigureDevicePage.ui \
    wizard/SelectDevicePage.ui

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

OTHER_FILES += \
    Info.plist
