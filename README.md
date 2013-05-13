Lightpack project with Prismatik flavour
---------

**Table of content:** <br />
&nbsp;&nbsp;[Short description] (https://github.com/Atarity/Lightpack#lightpack-project-with-prismatik-flavour) <br />
&nbsp;&nbsp;[Useful URLs] (https://github.com/Atarity/Lightpack#useful-urls) <br />
&nbsp;&nbsp;[Build Prismatik with Windows] (https://github.com/Atarity/Lightpack#prismatik-build-instructions-for-windows) <br />
&nbsp;&nbsp;[Build with Linux] (https://github.com/Atarity/Lightpack#build-instructions-for-linux) <br />
&nbsp;&nbsp;[Build with OS X] (https://github.com/Atarity/Lightpack#build-instructions-for-os-x) <br />
&nbsp;&nbsp;[Lightpack Firmware building] (https://github.com/Atarity/Lightpack#fimware-building-instructions) <br />


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
* [Embedded python binaries](https://lightpack.googlecode.com/files/python_binaries_win32.zip)
* [Microsoft DirectX SDK](http://www.microsoft.com/en-us/download/details.aspx?id=6812)
* POSIX shell utilities [MSYS for example](http://www.mingw.org/wiki/MSYS). Make sure `PATH` environment variable is set for the utilities (Run &rarr; sysdm.cpl &rarr; Advanced &rarr; Environment Variable &rarr; Edit `PATH` system variable (`C:\MinGW\msys\1.0\bin;` for example), path should points directly on the utilities so utilities are available without any subdirectories)

####Build process:
1. Unpack [embedded python binaries](https://lightpack.googlecode.com/files/python_binaries_win32.zip) to `<repo>/Software/` directory
2. Set all paths in `<repo>/software/build-vars.prf` (`<repo>/software/build-vars.prf.original` is example of such config)
3. Open **PythonQt** project with Qt Creator: `<repo>/sofware/pythonqt/pythonqt.pro` and build the project, this will build `generator` sub-project and generate code for further build, and it will be stopped with error, don't worry, it only needs to run `qmake` to include generated code to the project
4. Make sure you ran `qmake` against whole project and build **PythonQt** one more time
5. Close **PythonQt**, normally it will not be needed anymore, build **Prismatik** project

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
5. Add a rule for **UDEV**. See comments from `<repo>/93-lightpack.rules` for how to do it.

---

###Build instructions for OS X
####Prerequisites:
* Qt SDK (4.8.3+)
* MacOSX 10.7.sdk
* libUSB-1.0 from macports

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
3. Building **PythonQt**: go to `<repo>/Software/PythonQt` in repo master branch and sequently run `qmake -spec macx-g++` and `make all`
4. Build **Prismatik** project

to run Prismatik please make sure PythonQt libs are available for load at runtime 

---

###Fimware build instructions
1. Install [AVR GCC Toolchain] (http://avr-eclipse.sourceforge.net/wiki/index.php/The_AVR_GCC_Toolchain)
2. Install **dfu-programmer** for firmware upload with `$ sudo apt-get install dfu-programmer`
3. Install [Eclipse IDE] (http://www.eclipse.org/downloads/) for C/C++ Developers
4. Install [AVR Eclipse Plugin] (http://avr-eclipse.sourceforge.net/wiki/index.php/Plugin_Download)
5. Create new "Makefile project from existing code"
 * Set "Existing Code Location" to "Lightpack/Firmware/"
 * Set "Project Name" to "Lightpack"
 * Choose AVR-GCC Toolchain and press "finish"
6. Reboot device to bootloader and type `$ cd Lightpack/Firmware/`, then `$ make dfu`

---

Please let us know if you find mistakes, bugs or errors.<br />
Post new issue : http://code.google.com/p/lightpack/issues/list
