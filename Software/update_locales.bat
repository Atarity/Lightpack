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
C:\Qt\2010.05\qt\bin\lupdate.exe Lightpack.pro

echo.
echo ===========================================================
echo Generating release translation files for Lightpack project:
echo.
C:\Qt\2010.05\qt\bin\lrelease.exe Lightpack.pro

echo.

pause

exit 0

