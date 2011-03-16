# -------------------------------------------------
# Lightpack.pro
#
# Created on: 28.04.2010
#     Author: Mike Shatohin (brunql)
#
# Lightpack is very simple implementation of the backlight for a laptop
# 
# Copyright (c) 2010 Mike Shatohin, mikeshatohin [at] gmail.com
#
# http://lightpack.googlecode.com
#
# Lightpack based on:
# LUFA: http://www.fourwalledcubicle.com/LUFA.php
# hidapi: https://github.com/signal11/hidapi
#
# Lightpack is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
# 
# Lightpack is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#  
# Project created by QtCreator 2010-04-28T19:08:13
# -------------------------------------------------

TARGET      = Lightpack
DESTDIR     = bin

# Portable version store logs and settings in application directory
win32{
    # Use it on your own risk!
    DEFINES    += PORTABLE_VERSION
}

TEMPLATE    = app

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

TRANSLATIONS = res/translations/ru_RU.ts
RESOURCES    = res/LightpackResources.qrc
RC_FILE      = res/Lightpack.rc

unix{
    CONFIG    += link_pkgconfig
    PKGCONFIG += libusb-1.0
}

win32 {
    # Windows version using WinAPI for HID
    LIBS    += -lhid -lusbcamd -lsetupapi
    SOURCES += hidapi/windows/hid.cpp
    # Windows version using WinAPI + GDI for grab colors
    LIBS    += -lgdi32
    SOURCES += grab/grab_winapi.cpp
}

unix:!macx{
    # Linux version using libusb and hidapi codes
    SOURCES += hidapi/linux/hid-libusb.c
    # Linux version using Qt grabWindow(..) for grab colors
    SOURCES += grab/grab_qt.cpp
}

macx{
    # MacOS version using libusb and hidapi codes
    SOURCES += hidapi/mac/hid.c
    # MacOS version using Qt grabWindow(..) for grab colors
    SOURCES += grab/grab_qt.cpp
}

INCLUDEPATH += ./inc ./hidapi ./grab
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/ambilightusb.cpp \
    src/aboutdialog.cpp \
    src/grabmanager.cpp \
    src/movemewidget.cpp \
    src/desktop.cpp \
    src/settings.cpp
HEADERS += hidapi/hidapi.h \
    ../CommonHeaders/commands.h \
    ../CommonHeaders/RGB.h \
    ../CommonHeaders/USB_ID.h \
    inc/mainwindow.h \
    inc/ambilightusb.h \
    inc/version.h \
    inc/settings.h \
    inc/aboutdialog.h \
    inc/timeevaluations.h \
    inc/grabmanager.h \
    inc/movemewidget.h \
    inc/struct_rgb.h \
    inc/desktop.h \
    grab/grab_api.h
FORMS += src/mainwindow.ui \
    src/aboutdialog.ui \
    src/movemewidget.ui
