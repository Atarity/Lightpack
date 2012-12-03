This private Lightpack software repo is for R&amp;D
---------

###Build instructions for Windows
####Prerequisites:
* [Qt SDK](http://qt-project.org/downloads)
* [Python 2.7](http://python.org/download)
* [Microsoft DirectX SDK](http://www.microsoft.com/en-us/download/details.aspx?id=6812)
* POSIX shell utilities [MSYS for example](http://www.mingw.org/wiki/MSYS). Make sure `PATH` environment variable is set for the utilities (Run &rarr; sysdm.cpl &rarr; Advanced &rarr; Environment Variable &rarr; Edit `PATH` system variable (`C:\MinGW\msys\1.0\bin;` for example), path should points directly on the utilities so utilities are available without any subdirectories)

####Build process:

1. Set all paths in `<repo>/software/build-vars.prf` (`<repo>/software/build-vars.prf.original` is example of such config)
2. Open **PythonQt** project with Qt Creator: `<repo>/sofware/pythonqt/pythonqt.pro` and build the project, this will build `generator` sub-project and generate code for further build, and it will be stopped with error, don't worry, it only needs to run `qmake` to include generated code to the project
3. Make sure you ran `qmake` against whole project and build **PythonQt** one more time
4. Close **PythonQt**, normally it will not be needed anymore, build **Lightpack** project

---

###Build instructions for Linux
####Prerequisites:
You will need the following packages, usually all of them are in distro's repository:
* `gtk2-engines-pixbuf`
* `python2.7-dev`
* `libqt4-dev`
* `g++`
* `libusb-dev`
* `libudev-dev`

####Build process:
1. Set version of python in `<repo>/Software/build-vars.prf` (`<repo>/Software/build-vars.prf.original` is example of such config)
2. Make sure you have `QTDIR` environment variable set
3. Go to `<repo>/Software/PythonQt` and run `build.sh`
4. Build **Lightpack** project
