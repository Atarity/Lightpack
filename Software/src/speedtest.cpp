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

#include <iostream>
using namespace std;

#include "speedtest.h"
#include "settings.h"
#include "version.h"
#include "grab_api.h"


#ifdef Q_WS_WIN
#   include <windows.h>
#endif /* Q_WS_WIN */



// Hacks for create aligned string from variable X with specified LENGHT
#define ALIGN_RIGHT( LENGHT, X ) QVariant( X ).toString().prepend(QString(LENGHT - QVariant( X ).toString().length(), ' '))
#define ALIGNR3( X ) ALIGN_RIGHT( 3, X )
#define ALIGNR5( X ) ALIGN_RIGHT( 5, X )



#define CSV_SEPARATOR   ", "


// For accurate assessment of the dynamics of productivity growth,
// we must be sure that these constants do not change in the future.
/*static*/ const int SpeedTest::TestTimes = 5;
/*static*/ const int SpeedTest::LedsCount = 8;
/*static*/ const int SpeedTest::LedWidth  = 150;
/*static*/ const int SpeedTest::LedHeight = 150;



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
    resultStream << "Date"                      << CSV_SEPARATOR;
    resultStream << "Time"                      << CSV_SEPARATOR;
    resultStream << "Lightpack version"         << CSV_SEPARATOR;
    resultStream << "GrabQt FullScreen"         << CSV_SEPARATOR;
#   ifdef Q_WS_WIN
    resultStream << "GrabWinAPI FullScreen"     << CSV_SEPARATOR;
#   endif /* Q_WS_WIN */

    resultStream << "GrabQt LedsDefaults"       << CSV_SEPARATOR;
#   ifdef Q_WS_WIN
    resultStream << "GrabWinAPI LedsDefaults"   << CSV_SEPARATOR;
    resultStream << "Windows"                   << CSV_SEPARATOR;
#   endif /* Q_WS_WIN */

    resultStream << endl;
}

void SpeedTest::startTests()
{
    resultStream << QDateTime::currentDateTime().date().toString("yyyy.MM.dd")   << CSV_SEPARATOR;
    resultStream << QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz") << CSV_SEPARATOR;
    resultStream << "sw" VERSION_STR << CSV_SEPARATOR;

    // Main screen geometry
    QRect screenRect = QApplication::desktop()->screenGeometry();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Screen Rect:" << screenRect;


    //
    // Grab full screen TestTimes times for each LED from LedsCount
    //
    testFullScreenGrabSpeed();

    //
    // Grab default led postion and size (LedWidth x LedHeight) TestTimes times
    //
    testDefaultLedWidgetsGrabSpeed();


#   ifdef Q_WS_WIN

    //
    // Print windows version
    //
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
    // Aero enabled? Eval WinAPI function DwmIsCompositionEnabled for check it.
    //
    DEBUG_LOW_LEVEL << "Load library dwmapi.dll to test enabled Aero";


    typedef int (*DWM_IS_COMPOSITION_ENABLED)(BOOL *);
    DWM_IS_COMPOSITION_ENABLED DwmIsCompositionEnabled;


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


void SpeedTest::testFullScreenGrabSpeed()
{
    // Main screen geometry
    QRect screenRect = QApplication::desktop()->screenGeometry();

    //
    // Grab full screen via Qt grabWindow
    //
    time.start();
    for(int test = 0; test < TestTimes; test++){
        for(int led = 0; led < LedsCount; led++){
            GrabQt::getColor(
                    screenRect.x(),
                    screenRect.y(),
                    screenRect.width(),
                    screenRect.height());
        }
    }
    resultStream << ALIGNR5( time.elapsed() ) << CSV_SEPARATOR;




#   ifdef Q_WS_WIN
    //
    // Grab full screen via WinAPI BitBlt
    //

    // Initialize buffers, Desktop HWND == 0
    GrabWinAPI::findScreenOnNextCapture( 0 );
    // First time after findScreenOnNextCapture() call, captureScreen() will
    // reallocate memory for pixels buffer
    GrabWinAPI::captureScreen();

    time.start();
    for(int test = 0; test < TestTimes; test++){
        GrabWinAPI::captureScreen();
        for(int led = 0; led < LedsCount; led++){
            GrabWinAPI::getColor(
                    screenRect.x(),
                    screenRect.y(),
                    screenRect.width(),
                    screenRect.height());
        }
    }
    resultStream << ALIGNR5( time.elapsed() ) << CSV_SEPARATOR;

#   endif /* Q_WS_WIN */

}


void SpeedTest::testDefaultLedWidgetsGrabSpeed()
{
    // Main screen geometry
    QRect screenRect = QApplication::desktop()->screenGeometry();

    // Default leds size and position
    QList<QRect> ledsRects;

    for(int ledIndex=0; ledIndex < LedsCount; ledIndex++){
        QRect ledRect;
        if(ledIndex < 4 /* TODO: see below */){
            ledRect.setX(0);
        }else{
            ledRect.setX(screenRect.width() - LedWidth);
        }

        // TODO: rewitre this stuff! I think this should be more independent from count of leds, same in settings
        switch( ledIndex ){
        case 0:
        case 1: ledRect.setY(screenRect.height() / 2 - 2*LedHeight);  break;
        case 2:
        case 3: ledRect.setY(screenRect.height() / 2 - LedHeight);  break;
        case 4:
        case 5: ledRect.setY(screenRect.height() / 2 );  break;
        case 6:
        case 7: ledRect.setY(screenRect.height() / 2 + LedHeight);  break;
        }

        ledRect.setWidth( LedWidth );
        ledRect.setHeight( LedHeight );

        ledsRects << ledRect;
    }


    //
    // Grab led widget via Qt grabWindow
    //
    time.start();
    for(int test = 0; test < TestTimes; test++){
        for(int led = 0; led < LedsCount; led++){
            GrabQt::getColor(
                    ledsRects[ led ].x(),
                    ledsRects[ led ].y(),
                    ledsRects[ led ].width(),
                    ledsRects[ led ].height());
        }
    }
    resultStream << ALIGNR5( time.elapsed() ) << CSV_SEPARATOR;




#   ifdef Q_WS_WIN
    //
    // Grab led widget via WinAPI BitBlt
    //
    time.start();
    for(int test = 0; test < TestTimes; test++){
        GrabWinAPI::captureScreen();
        for(int led = 0; led < LedsCount; led++){
            GrabWinAPI::getColor(
                    ledsRects[ led ].x(),
                    ledsRects[ led ].y(),
                    ledsRects[ led ].width(),
                    ledsRects[ led ].height());
        }
    }
    resultStream << ALIGNR5( time.elapsed() ) << CSV_SEPARATOR;

#   endif /* Q_WS_WIN */
}
