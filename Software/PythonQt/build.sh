#!/bin/sh

if [ "x$QTDIR" = "x" ]; then
    echo "QTDIR variable is empty"
    exit 1
fi

qmake -recursive PythonQt.pro
cd generator
make
./pythonqt_generator --include-paths=/usr/include/qt4
if [ $? -eq 0 ]; then
   cd ..
   qmake -recursive PythonQt.pro && make
else
   echo "error occured while generating the code"
fi
