This private Lightpack software repo is for R&amp;D
=========
To build whole project including PythonQt (for plugins) and DirectX hooks (for grabbing), are required:
* <a href="http://qt-project.org/downloads">Qt SDK</a>
* <a href="http://python.org/download/">Python 2.7</a>
* <a href="http://www.microsoft.com/en-us/download/details.aspx?id=6812">Microsoft DirectX SDK</a> (Windows only)
* POSIX shell utilities <a href="http://www.mingw.org/wiki/MSYS">MSYS for example</a> and the PATH environment variable pointing to them (Windows only)

Build process:
-----------
1. Open <b>PythonQt</b> project with Qt Creator: <code>\<repo>/sofware/pythonqt/pythonqt.pro</code> , proceed to <code>src/python/python.prf</code> and set actual paths to your Python installation (by default <code>C:/Python27</code>);
2. Build <b>Generator</b> sub-project only (right-click to the generator node &rarr; build);
3. Copy resulting file <code>pythonqt_generator.exe</code> to <code>\<repo>/software/pythonqt/generator</code>;
4. Copy the following Qt libraries, from for example (depends on Qt version), <code>\<QtSDK>/Desktop/Qt/4.7.4/mingw/bin</code> to <code>\<repo>/software/pythonqt/generator/</code>:
 * QtCore4.dll
 * QtXml4.dll
 * mingwm10.dll
 * libgcc_s_dw2-1.dll;
5. Setup environment variables, like this (execute from command-line): <code>set QTDIR=C:/QtSDK/Desktop/Qt/4.7.4/mingw</code>;
6. Run <b>pythonqt_generator.exe</b> from command-line;
7. In Qt Creator run qmake for whole <b>PythonQt</b> project to include generated code to solution (right-click to the PythonQt node &rarr; qmake);
8. Build "src" sub-project (right-click to the src node &rarr; build);
9. Copy all <b>.a</b> files from <code>\<pyhonqt_builddir>/lib/</code> to <code>\<repo>/software/pythonqt/lib</code>;
10. Build whole <b>PythonQt</b>;
11. Repeat step 8;
12. (Windows only) Create <code>\<repo>/software/src/src.pri</code> based on <code>\<repo>/software/src/src.pri.original</code> and set Microsoft DirectX SDK path in it. Double chek you escape path slashes to <code>/</code>;
13. Build <b>Lightpack</b> project;
14. Copy the following Qt libraries, from for example (depends on Qt version), <code>QtSDK/Desktop/Qt/4.7.4/mingw/bin</code> to <code>\<lightpack_builddir>/src/bin/</code>:
 * QtCore4.dll
 * QtXml4.dll
 * QtGui4.dll
 * QtNetwork4.dll
 * mingwm10.dll
 * libgcc_s_dw2-1.dll;
15. Copy all <b>.dll</b> from <code>\<pythonqt_builddir>/lib/</code> to <code>\<lightpack_buildder>/src/bin/</code>;
16. Run project.