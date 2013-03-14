#!/bin/sh

DESTDIR=$1
FROMDIR=$2

FWDIR=Prismatik.app/Contents/Frameworks
APPDIR=Prismatik.app/Contents/MacOS

cd $DESTDIR

test ! -d $FWDIR && mkdir $FWDIR

cp -fRL $FROMDIR/../PythonQt/lib/libPythonQt.1.dylib $FWDIR/
cp -fRL $FROMDIR/../PythonQt/lib/libPythonQt_QtAll.1.dylib $FWDIR/

install_name_tool -change libPythonQt.1.dylib @executable_path/../Frameworks/libPythonQt.1.dylib $FWDIR/libPythonQt_QtAll.1.dylib
install_name_tool -change libPythonQt.1.dylib @executable_path/../Frameworks/libPythonQt.1.dylib $APPDIR/Prismatik
install_name_tool -change libPythonQt_QtAll.1.dylib @executable_path/../Frameworks/libPythonQt_QtAll.1.dylib $APPDIR/Prismatik

cd $FROMDIR

exit 0
