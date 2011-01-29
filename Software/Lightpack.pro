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
# https://github.com/brunql/Lightpack
#
# This project based on:
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
DESTDIR = ./build
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
UI_DIR = ./ui
RCC_DIR = ./res

TARGET = Lightpack

TRANSLATIONS = res/translations/ru_RU.ts
RESOURCES = res/LightpackResources.qrc
RC_FILE = res/Lightpack.rc

CONFIG -= console
CONFIG -= app_bundle

unix{
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0
}


win32 {
    # Windows using WinAPI for HID
    LIBS += -lhid -lusbcamd -lsetupapi
    SOURCES += hidapi/windows/hid.cpp
}

unix:!macx{
    # Linux version using libusb and hidapi codes
    SOURCES += hidapi/linux/hid-libusb.c    
}

macx{
    # MacOS version using libusb and hidapi codes
    SOURCES += hidapi/mac/hid.c
}

INCLUDEPATH += ./inc ./hidapi
TEMPLATE = app
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/ambilightusb.cpp \
    src/aboutdialog.cpp \
    src/grabdesktopwindowleds.cpp \
    src/movemewidget.cpp
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
    inc/grabdesktopwindowleds.h \
    inc/movemewidget.h \
    inc/struct_rgb.h \
    inc/ledcolors.h
FORMS += src/mainwindow.ui \
    src/aboutdialog.ui
