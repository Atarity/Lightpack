# -------------------------------------------------
# Lightpack.pro
#
# Created on: 28.04.2010
#
# Lightpack is very simple implementation of the backlight for a laptop
# 
# Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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


CONFIG-=create_cmake

#linux-g++:QMAKE_TARGET.arch = $$QMAKE_HOST.arch
#linux-g++-32:QMAKE_TARGET.arch = x86
#linux-g++-64:QMAKE_TARGET.arch = x86_64

#contains(QMAKE_TARGET.arch, x86_64):{
#        unix:QMAKE_CXXFLAGS+=-m32
#        unix:QMAKE_LFLAGS+=-m32 -L/usr/lib32 -L/usr/lib/i386-linux-gnu
#    }

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = math grab

unix:!macx {
    SUBDIRS += qtserialport
}

win32:SUBDIRS += libraryinjector hooks tests
SUBDIRS += src
