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

SpeedTest::SpeedTest() : QObject()
{
}


void SpeedTest::start()
{
    QString filePath = Settings::getApplicationDirPath() + "/speedtest.csv";

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
#   ifdef Q_WS_WIN
    switch( QSysInfo::windowsVersion() ){
    case QSysInfo::WV_NT:       resultStream << "WinNT (4.0); "; break;
    case QSysInfo::WV_2000:     resultStream << "Win2000 (5.0); "; break;
    case QSysInfo::WV_XP:       resultStream << "WinXP (5.1); "; break;
    case QSysInfo::WV_2003:     resultStream << "WinServer2003 (5.2); "; break;
    case QSysInfo::WV_VISTA:    resultStream << "WinVista, WinServer2008 (6.0); "; break;
    case QSysInfo::WV_WINDOWS7: resultStream << "Win7 (6.1); "; break;
    default:                    resultStream << "Unknown: " << QSysInfo::windowsVersion() << "; ";
    }
#   endif

    resultStream << endl;

    resultStream << "Date; ";
    resultStream << "Time; ";
    resultStream << "Soft ver; ";

    resultStream << "GrabQt; AARRGGBB; ";

#   ifdef Q_WS_WIN
    resultStream << "GrabWinAPI; AARRGGBB; ";
#   endif

    resultStream << endl;
}

void SpeedTest::startTests()
{
    QTime time;

    resultStream << QDateTime::currentDateTime().date().toString("yyyy.MM.dd") << "; ";
    resultStream << QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz") << "; ";
    resultStream << "sw" << VERSION_STR << "; ";

    // Main screen geometry
    QRect screenRect = QApplication::desktop()->screenGeometry();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Screen Rect:" << screenRect;

    QRgb lastResultRgb = 0;

    const int testTimes = 10;
    const int ledsCount = 10;

    time.start();
    for(int test = 0; test < testTimes; test++){
        for(int led = 0; led < ledsCount; led++){
            lastResultRgb = GrabQt::getColor(
                    screenRect.x(),
                    screenRect.y(),
                    screenRect.width(),
                    screenRect.height());
        }
    }
    resultStream << time.elapsed() << "; ";
    resultStream << hex << lastResultRgb << dec << "; ";

    lastResultRgb = 0;


#   ifdef Q_WS_WIN
    // Desktop HWND == 0
    GrabWinAPI::findScreenOnNextCapture( 0 );
    // re-Allocate memory
    GrabWinAPI::captureScreen();

    time.start();
    for(int test = 0; test < testTimes; test++){
        GrabWinAPI::captureScreen();
        for(int led = 0; led < ledsCount; led++){
            lastResultRgb = GrabWinAPI::getColor(
                    screenRect.x(),
                    screenRect.y(),
                    screenRect.width(),
                    screenRect.height());
        }
    }
    resultStream << time.elapsed() << "; ";
    resultStream << hex << lastResultRgb << dec << "; ";
#   endif



    resultStream << endl;
}

