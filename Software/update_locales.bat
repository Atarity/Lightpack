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
c:\Qt\4.8.4\bin\lupdate.exe %1 src\src.pro

echo.
echo ===========================================================
echo Generating release translation files for Lightpack project:
echo.
c:\Qt\4.8.4\bin\lrelease.exe src\src.pro

echo.

pause

exit 0

