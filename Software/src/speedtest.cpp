/*
 * speedtest.cpp
 *
 *  Created on: 4.04.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  Lightpack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Lightpack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QApplication>
#include <QDesktopWidget>
#include <QTime>

#include <iostream>

#include "speedtest.h"
#include "settings.h"
#include "version.h"
#include "grab_api.h"

using namespace std;

#ifdef Q_WS_WIN
#include <windows.h>

typedef int (*DWM_IS_COMPOSITION_ENABLED)(BOOL *);
DWM_IS_COMPOSITION_ENABLED DwmIsCompositionEnabled;

#endif /* Q_WS_WIN */

// Hacks for create aligned string from variable X with specified LENGHT
#define ALIGN_LEFT( X, LENGHT ) QVariant( X ).toString().append(QString(LENGHT - QVariant( X ).toString().length(), ' '))
#define ALIGN_RIGHT( X, LENGHT ) QVariant( X ).toString().prepend(QString(LENGHT - QVariant( X ).toString().length(), ' '))

#define ALIGNL10( X ) ALIGN_LEFT( X, 10 )
#define ALIGNL12( X ) ALIGN_LEFT( X, 12 )
#define ALIGNL14( X ) ALIGN_LEFT( X, 14 )

#define ALIGNR10( X ) ALIGN_RIGHT( X, 10 )
#define ALIGNR12( X ) ALIGN_RIGHT( X, 12 )
#define ALIGNR14( X ) ALIGN_RIGHT( X, 14 )


#define CSV_SEPARATOR   ", "


SpeedTest::SpeedTest() : QObject()
{

}


void SpeedTest::start()
{
    QString filePath = Settings::getApplicationDirPath() + "/SpeedTest.csv";

    resultFile.setFileName( filePath );

    bool IsFileExists = false;
    if( resultFile.exists() ){
        IsFileExists = true;
    }

    if(resultFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
        resultStream.setDevice(&resultFile);

        if( IsFileExists == false ){
            printHeader();
        }

        startTests();

    }else{
        qWarning() << Q_FUNC_INFO << "Can't open file:" << filePath;
    }
}


void SpeedTest::printHeader()
{
    resultStream << ALIGNL10("Date")        << CSV_SEPARATOR;
    resultStream << ALIGNL12("Time")        << CSV_SEPARATOR;
    resultStream << ALIGNL10("Lightpack")    << CSV_SEPARATOR;
    resultStream << ALIGNL10("GrabQt")      << CSV_SEPARATOR;

#   ifdef Q_WS_WIN
    resultStream << ALIGNL10("GrabWinAPI")  << CSV_SEPARATOR;
    resultStream << ALIGNL10("Precision")   << CSV_SEPARATOR;
    resultStream << ALIGNL10("Windows")     << CSV_SEPARATOR;
#   endif /* Q_WS_WIN */

    resultStream << endl;
}

void SpeedTest::startTests()
{
    QTime time;

    resultStream << ALIGNL10( QDateTime::currentDateTime().date().toString("yyyy.MM.dd")) << CSV_SEPARATOR;
    resultStream << QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz") << CSV_SEPARATOR;
    resultStream << ALIGNR10( "sw" VERSION_STR ) << CSV_SEPARATOR;

    // Main screen geometry
    QRect screenRect = QApplication::desktop()->screenGeometry();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Screen Rect:" << screenRect;

    const int testTimes = 5;
    const int ledsCount = 10;

    time.start();
    for(int test = 0; test < testTimes; test++){
        for(int led = 0; led < ledsCount; led++){
            GrabQt::getColor(
                    screenRect.x(),
                    screenRect.y(),
                    screenRect.width(),
                    screenRect.height());
        }
    }
    resultStream << ALIGNR10( time.elapsed() ) << CSV_SEPARATOR;

#   ifdef Q_WS_WIN
    // Desktop HWND == 0
    GrabWinAPI::findScreenOnNextCapture( 0 );
    // re-Allocate memory
    GrabWinAPI::captureScreen();

    time.start();
    for(int test = 0; test < testTimes; test++){
        GrabWinAPI::captureScreen();
        for(int led = 0; led < ledsCount; led++){
            GrabWinAPI::getColor(
                    screenRect.x(),
                    screenRect.y(),
                    screenRect.width(),
                    screenRect.height());
        }
    }
    resultStream << ALIGNR10( time.elapsed() ) << CSV_SEPARATOR;
    resultStream << ALIGNR10( GrabWinAPI::getGrabPrecision() ) << CSV_SEPARATOR;

    // Windows version
    switch( QSysInfo::windowsVersion() ){
    case QSysInfo::WV_NT:       resultStream << "WinNT (4.0)"; break;
    case QSysInfo::WV_2000:     resultStream << "Win2000 (5.0)"; break;
    case QSysInfo::WV_XP:       resultStream << "WinXP (5.1)"; break;
    case QSysInfo::WV_2003:     resultStream << "WinServer2003 (5.2)"; break;
    case QSysInfo::WV_VISTA:    resultStream << "WinVista, WinServer2008 (6.0)"; break;
    case QSysInfo::WV_WINDOWS7: resultStream << "Win7 (6.1)"; break;
    default:                    resultStream << "Unknown: " << QSysInfo::windowsVersion();
    }
    resultStream << CSV_SEPARATOR;



    //
    // Eval WinAPI function DwmIsCompositionEnabled to test enabled Aero
    //
    DEBUG_LOW_LEVEL << "Load library dwmapi.dll to test enabled Aero";

    HINSTANCE hDll;
    hDll = LoadLibrary(L"dwmapi.dll");
    if(hDll == NULL){
        qWarning() << "Error loading win32 dll: dwmapi.dll";
    }else{
        DwmIsCompositionEnabled = (DWM_IS_COMPOSITION_ENABLED) GetProcAddress(hDll,"DwmIsCompositionEnabled");
        if(DwmIsCompositionEnabled != NULL){
            BOOL result = false;
            (*DwmIsCompositionEnabled)(&result);
            if(result){
                resultStream << "Aero (DWM Composition Enabled)" << CSV_SEPARATOR;
            }
        }else{
            qWarning() << "Error:" << GetLastError();
        }
    }

#   endif /* Q_WS_WIN */



    resultStream << endl;
}

