# -------------------------------------------------
# src.pro
#
# Copyright (c) 2010,2011 Mike Shatohin, mikeshatohin [at] gmail.com
# http://lightpack.googlecode.com
# Project created by QtCreator 2010-04-28T19:08:13
# -------------------------------------------------

TARGET      = Lightpack
DESTDIR     = bin
TEMPLATE    = app
QT         += network

# QMake and GCC produce a lot of stuff
OBJECTS_DIR = stuff
MOC_DIR     = stuff
UI_DIR      = stuff
RCC_DIR     = stuff

# Find currect mercurial revision
HG_REVISION = $$system(hg id -i)

# For update HG_REVISION use it:
#   $ qmake Lightpack.pro && make clean && make
#
# Or simply edit this file (add space anythere
# for cause to call qmake) and re-build project

isEmpty( HG_REVISION ){
    # In code uses #ifdef HG_REVISION ... #endif
    message( "Mercurial not found, HG_REVISION will be undefined" )
} else {
    # Define currect mercurial revision id
    # It will be show in about dialog and --help output
    DEFINES += HG_REVISION=\\\"$${HG_REVISION}\\\"
}

TRANSLATIONS = ../res/translations/ru_RU.ts
RESOURCES    = ../res/LightpackResources.qrc
RC_FILE      = ../res/Lightpack.rc

unix{
    CONFIG    += link_pkgconfig
    PKGCONFIG += libusb-1.0
}

win32 {
    # Windows version using WinAPI for HID
    LIBS    += -lhid -lusbcamd -lsetupapi
    # For QSerialDevice
    LIBS    += -luuid -ladvapi32

    SOURCES += hidapi/windows/hid.c
    # Windows version using WinAPI + GDI for grab colors
    LIBS    += -lgdi32 -ld3d9
}

unix:!macx{
    # Linux version using libusb and hidapi codes
    SOURCES += hidapi/linux/hid-libusb.c
    # For QSerialDevice
    LIBS += -ludev
}

macx{
    # MacOS version using libusb and hidapi codes
    SOURCES += hidapi/mac/hid.c
    LIBS += -framework IOKit \
            -framework Cocoa \
            -framework Carbon \
            -framework CoreFoundation \
            -framework ApplicationServices \
            -framework OpenGL
    ICON = ../res/icons/Lightpack.icns

    # For build universal binaries (native on Intel and PowerPC)
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk
}

INCLUDEPATH += ./hidapi ./grab ./alienfx ./

SOURCES += \
    LightpackApplication.cpp \
    main.cpp \
    SettingsWindow.cpp \
    Settings.cpp \
    AboutDialog.cpp \
    GrabManager.cpp \
    GrabWidget.cpp \
    GrabConfigWidget.cpp \
    SpeedTest.cpp \
    LedDeviceFactory.cpp \
    LedDeviceLightpack.cpp \
    LedDeviceAlienFx.cpp \
    LedDeviceAdalight.cpp \
    LedDeviceArdulight.cpp \
    LedDeviceVirtual.cpp \
    ColorButton.cpp \
    grab/X11Grabber.cpp \
    grab/WinAPIGrabber.cpp \
    grab/WinAPIGrabberEachWidget.cpp \
    grab/QtGrabber.cpp \
    grab/QtGrabberEachWidget.cpp \
    grab/MacOSGrabber.cpp \
    grab/D3D9Grabber.cpp \
    ApiServer.cpp \
    ApiServerSetColorTask.cpp \
    LightpackMath.cpp \
    MoodLampManager.cpp \
    LightpackPluginInterface.cpp \
    PluginManager.cpp \
    plugins/PyPlugin.cpp \
    AboutPluginDialog.cpp

HEADERS += \
    LightpackApplication.hpp \
    SettingsWindow.hpp \
    Settings.hpp \
    SettingsDefaults.hpp \
    version.h \
    AboutDialog.hpp \
    TimeEvaluations.hpp \
    GrabManager.hpp \
    GrabWidget.hpp \
    GrabConfigWidget.hpp \
    debug.h \
    SpeedTest.hpp \
    alienfx/LFXDecl.h \
    alienfx/LFX2.h \
    LedDeviceFactory.hpp \
    ILedDevice.hpp \    
    LedDeviceLightpack.hpp \
    LedDeviceAlienFx.hpp \
    LedDeviceAdalight.hpp \
    LedDeviceArdulight.hpp \
    LedDeviceVirtual.hpp \
    ColorButton.hpp \
    grab/IGrabber.hpp \
    grab/QtGrabber.hpp \
    grab/QtGrabberEachWidget.hpp \
    grab/X11Grabber.hpp \
    grab/MacOSGrabber.hpp \
    grab/WinAPIGrabber.hpp \
    grab/WinAPIGrabberEachWidget.hpp \
    defs.h \
    enums.hpp \
    ApiServer.hpp \
    ApiServerSetColorTask.hpp \
    hidapi/hidapi.h \
    ../../CommonHeaders/LIGHTPACK_HW.h \
    ../../CommonHeaders/COMMANDS.h \
    ../../CommonHeaders/USB_ID.h \
    grab/D3D9Grabber.hpp \
    LightpackMath.hpp \
    StructRgb.hpp \
    MoodLampManager.hpp \
    LightpackPluginInterface.hpp \
    PluginManager.hpp \
    plugins/PyPlugin.h \
    AboutPluginDialog.hpp


FORMS += SettingsWindow.ui \
    AboutDialog.ui \
    GrabWidget.ui \
    GrabConfigWidget.ui \
    AboutPluginDialog.ui

#
#   QSerialDevice
#
include(qserialdevice/qserialdevice/qserialdevice.pri)
include(qserialdevice/qserialdeviceenumerator/qserialdeviceenumerator.pri)
unix:include(qserialdevice/unix/ttylocker.pri)

#
# QtSingleApplication
#
include(qtsingleapplication/src/qtsingleapplication.pri)

#
# Hotkeys based on PSI and QKeySequenceWidget
#
include(hotkeys/globalshortcut/globalshortcut.pri)
include(hotkeys/qkeysequencewidget/qkeysequencewidget.pri)

#
# PythonQt
#
include (../PythonQT/build/common.prf )
include (../PythonQT/build/PythonQt.prf )
include (../PythonQT/build/PythonQt_QtAll.prf )

OTHER_FILES += \
    test.js

