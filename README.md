Lightpack project with Prismatik flavour
---------

**Lightpack** is an fully open-source and simple hardware implementation of the backlight for any computer. It's USB content-driving ambient lighting system.

**Prismatik** is an open-source software we buid to control Lightpack device. It grabs screen, analize picture,
calculate resulting colors and provide soft and gentle lighting with Lightpack device. Moreother, you can 
handle another devices with Prismatik such as Adalight, Ardulight or even Alienware LightFX system.

#####Main features:
* Fully open-source under GPLv3 (hardware, software, firmware)
* Cross-platform GUI (Qt)
* USB HID (no need to install any drivers)
* The device is simple to build (just Do-It-Yourself) 

#####Useful URLs:
* [Project mothership] (http://code.google.com/p/lightpack/)
* [Binary downloads] (http://code.google.com/p/lightpack/downloads/list)
* Wiki with DIY and documentation [ENG] (http://code.google.com/p/light-pack/w/list) / [RUS] (http://code.google.com/p/lightpack/w/list)
* [Post new issue] (http://code.google.com/p/lightpack/issues/list)
* [Team] (http://code.google.com/p/lightpack/people/list)

---

###Prismatik build instructions for Windows
####Prerequisites:
* [Qt SDK](http://qt-project.org/downloads)
* [Python 2.7](http://python.org/download)
* [Microsoft DirectX SDK](http://www.microsoft.com/en-us/download/details.aspx?id=6812)
* POSIX shell utilities [MSYS for example](http://www.mingw.org/wiki/MSYS). Make sure `PATH` environment variable is set for the utilities (Run &rarr; sysdm.cpl &rarr; Advanced &rarr; Environment Variable &rarr; Edit `PATH` system variable (`C:\MinGW\msys\1.0\bin;` for example), path should points directly on the utilities so utilities are available without any subdirectories)

####Build process:

1. Set all paths in `<repo>/software/build-vars.prf` (`<repo>/software/build-vars.prf.original` is example of such config)
2. Open **PythonQt** project with Qt Creator: `<repo>/sofware/pythonqt/pythonqt.pro` and build the project, this will build `generator` sub-project and generate code for further build, and it will be stopped with error, don't worry, it only needs to run `qmake` to include generated code to the project
3. Make sure you ran `qmake` against whole project and build **PythonQt** one more time
4. Close **PythonQt**, normally it will not be needed anymore, build **Prismatik** project

---

###Build instructions for Linux
####Prerequisites:
You will need the following packages, usually all of them are in distro's repository:
* gtk2-engines-pixbuf
* python2.7-dev
* libqt4-dev
* g++
* libusb-dev
* libudev-dev

####Build process:
1. Set version of python in `<repo>/Software/build-vars.prf` (`<repo>/Software/build-vars.prf.original` is example of such config)
2. Make sure you have `QTDIR` environment variable set
3. Go to `<repo>/Software/PythonQt` and run `build.sh`
4. Build **Prismatik** project

---

###Build instructions for OS X
####Prerequisites:
* Qt SDK (4.8.3+)
* MacOSX 10.7.sdk
* Qt libs (macports.org)
* LibUSB

###### Whole dependencies list for Prismatik 5.9.1:
* QtCore.framework
* QtGui.framework
* QtNetwork.framework
* QtOpenGL.framework
* QtSql.framework
* QtSvg.framework
* QtUiTools.framework
* QtWebKit.framework
* QtXml.framework
* QtXmlPatterns.framework
* libPythonQt_QtAll.1.dylib
* libPythonQt.1.dylib
* libsqlite3.0.dylib
* libusb-1.0.0.dylib
* libz.1.dylib

####Build process:
1. Download and unpack 4.8.3+ **Qt SDK** from www.qt-project.org
2. Configure and build **Qt** with sequence of commands: `./configure -opensource -nomake examples -nomake tests -no-webkit`, then `make -j 4`, and `make install`. It will take some time
3. Building **PythonQt**: go to `/Software/PythonQt` in repo master branch and sequently run `qmake` and `make all`
4. Copy libs we build at step 3 from folder **"lib"** into `/usr/lib/`
5. Build **Prismatik** project 