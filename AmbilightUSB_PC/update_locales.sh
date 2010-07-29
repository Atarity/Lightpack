#!/bin/bash
#
# update_locales.sh
#
#    Created on: 29.07.2010
#        Author: brunql
#       Project: AmbilightUSB
#	Description: Updating translation files for Ambilight project
#
TRANSLATION_DIR=translations
TRANSLATION_FILES=Ambilight_ru_RU.qm

echo 
echo "================================================="
echo "Updating translation files for Ambilight project:"
echo 
lupdate Ambilight.pro

echo 
echo "==========================================================="
echo "Generating release translation files for Ambilight project:"
echo 
lrelease Ambilight.pro

echo 
echo "============================================="
echo "Copying translation files to build directory:"

for file in $TRANSLATION_FILES
do
	echo "    $file"
	cp "$TRANSLATION_DIR/$file" "build/$file"
done

echo
	
exit 0

