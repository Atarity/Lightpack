This private Lightpack software repo is for R&amp;D
=========
To build whole project for Windows including PythonQt (for plugins) and DirectX hooks (for grabbing), are required:
* <a href="http://qt-project.org/downloads">Qt SDK</a>
* <a href="http://python.org/download/">Python 2.7</a>
* <a href="http://www.microsoft.com/en-us/download/details.aspx?id=6812">Microsoft DirectX SDK</a>
* POSIX shell utilities <a href="http://www.mingw.org/wiki/MSYS">MSYS for example</a>. Make sure PATH environment variable is set for the utilities (Run > sysdm.cpl > Advanced > Environment Variable > Edit 'Path' system variable, path should points directly on the utilites so utilities is available without any subdirectories)

Build process (for Windows):
-----------
1. Set all paths in <code>\<repo>/software/build-vars.prf</code> (<code>\<repo>/software/build-vars.prf.original</code> is example of such config)
2. Open <b>PythonQt</b> project with Qt Creator: <code>\<repo>/sofware/pythonqt/pythonqt.pro</code> and build the project, this will build <b>generator</b> sub-project and generate code for further build, and it will be stopped with error, don't worry, it only needs to run <code>qmake</code> to include generated code to the project
3. Make sure you ran <code>qmake</code> against whole project and build <b>PythonQt</b> one more time
4. Close <b>PythonQt</b>, normally it will not be needed anymore, build <b>Lightpack</b> project
