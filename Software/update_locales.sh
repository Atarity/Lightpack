#!/bin/bash
#
# update_locales.sh
#
#    Created on: 29.07.2010
#        Author: Mike Shatohin (brunql)
#   Description: Updating translation files for AmbilightUSB project
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

echo 
echo "================================================="
echo "Updating translation files for Ambilight project:"
echo 
lupdate AmbilightUSB.pro

echo 
echo "==========================================================="
echo "Generating release translation files for Ambilight project:"
echo 
lrelease AmbilightUSB.pro

echo

exit 0

