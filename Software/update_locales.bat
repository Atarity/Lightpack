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
echo executing C:\Qt\4.8.5\bin\lupdate.exe %1 -pro src\src.pro
echo.
C:\Qt\4.8.5\bin\lupdate.exe %1 -pro src\src.pro

echo.
echo ===========================================================
echo Generating release translation files for Lightpack project:
echo.
C:\Qt\4.8.5\bin\lrelease.exe src\src.pro

echo.

pause

exit 0

