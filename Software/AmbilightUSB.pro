# -------------------------------------------------
# AmbilightUSB.pro
#
# Created on: 28.04.2010
#     Author: Mike Shatohin (brunql)
#
# AmbilightUSB is very simple implementation of the backlight for a laptop
# 
# Copyright (c) 2010 Mike Shatohin, mikeshatohin [at] gmail.com
# 
# AmbilightUSB is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
# 
# AmbilightUSB is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#  
# Project created by QtCreator 2010-04-28T19:08:13
# -------------------------------------------------
DESTDIR = ./build
TARGET = AmbilightUSB
TRANSLATIONS = res/translations/AmbilightUSB_ru_RU.ts
CONFIG -= console
CONFIG -= app_bundle

win32 {
    # In windows using WinAPI for HID
    LIBS += -lhid -lusbcamd -lsetupapi
}

unix {
    # In unix using libusb
    LIBS += -lusb
}

INCLUDEPATH += ./inc
INCLUDEPATH += ../Firmware/inc/
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
TEMPLATE = app
SOURCES += src/hiddata.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/ambilightusb.cpp \
    src/aboutdialog.cpp \
    src/grabdesktopwindowleds.cpp
HEADERS += inc/hiddata.h \
    ../Firmware/inc/usbconfig.h \
    ../Firmware/inc/commands.h \
    ../Firmware/inc/RGB.h \
    inc/mainwindow.h \
    inc/ambilightusb.h \
    inc/version.h \
    inc/settings.h \
    inc/aboutdialog.h \
    inc/timeevaluations.h \
    inc/grabdesktopwindowleds.h \
    inc/struct_rgb.h \
    inc/ledcolors.h
FORMS += src/mainwindow.ui \
    src/aboutdialog.ui
RESOURCES += res/res_ambilight.qrc
RC_FILE = res/AmbilightUSB.rc
