rem
rem update_locales.bat
rem
rem    Created on: 24.02.2011
rem        Author: Mike Shatohin (brunql)
rem   Description: Updating translation files for Lightpack project
rem           URL: lightpack.googlecode.com
rem

echo off && cls

echo.
echo =================================================
echo Updating translation files for Lightpack project:
echo.
echo executing C:\QtSDK\Desktop\Qt\4.8.1\mingw\bin\lupdate.exe %1 src\src.pro
echo.
D:\QtSDK\Desktop\Qt\4.7.4\mingw\bin\lupdate.exe -no-obsolete %1 src\src.pro

echo.
echo ===========================================================
echo Generating release translation files for Lightpack project:
echo.
D:\QtSDK\Desktop\Qt\4.7.4\mingw\bin\lrelease.exe src\src.pro

echo.

pause

exit 0

