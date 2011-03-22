/*
 * main.cpp
 *
 *  Created on: 21.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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


#include <QtGui>
#include <iostream>
#include "mainwindow.h"

#include "settings.h"
#include "version.h"
#include "ambilightusb.h"
#include "desktop.h"

#include <sys/time.h>
#include "time.h"

#include <QFileInfo>

using namespace std;


#include "debug.h"
unsigned debugLevel = 0;


QTextStream logStream;

QString logWhileWindowNotInitialized = "";

static void showHelpMessage()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Project  : Lightpack \n");
    fprintf(stderr, "Author   : Mike Shatohin \n");
    fprintf(stderr, "Version  : %s\n", VERSION_STR);
#ifdef HG_REVISION
    fprintf(stderr, "Revision : %s \n", HG_REVISION);
#endif
    fprintf(stderr, "Site     : lightpack.googlecode.com \n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Build with Qt version %s \n", QT_VERSION_STR);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options: \n");
    fprintf(stderr, "  --off    - send 'off leds' cmd to device \n");
    fprintf(stderr, "  --help   - show this help \n");
    fprintf(stderr, "  --debug_high  - maximum verbose level of debug output");
    fprintf(stderr, "  --debug_mid   - middle debug level");
    fprintf(stderr, "  --debug_low   - low debug level, DEFAULT");
    fprintf(stderr, "  --debug_zero  - minimum debug output");
    fprintf(stderr, "\n");
}

bool openLogFile(const QString & filePath)
{
    QFile *logFile = new QFile(filePath);
    QIODevice::OpenMode openFileAppendOrTruncateFlag = QIODevice::Append;
    QFileInfo info(filePath);
    if(info.size() > 1*1024*1024){
        cout << "Log file size > 1 Mb. I'm going to clear it. Now!" << endl;
        openFileAppendOrTruncateFlag = QIODevice::Truncate;
    }
    if(logFile->open(QIODevice::WriteOnly | openFileAppendOrTruncateFlag | QIODevice::Text)){
        logStream.setDevice(logFile);
        logStream << endl;
        logStream << QDateTime::currentDateTime().date().toString("yyyy_MM_dd") << " ";
        logStream << QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz") << " Lightpack sw" << VERSION_STR << endl;
    }else{
        cerr << "Error open file for write: " << filePath.toStdString() << endl;
        return false;
    }
    return true;
}

void messageOutput(QtMsgType type, const char *msg)
{
    QString out = QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz") + " ";
    switch (type) {
    case QtDebugMsg:        
        out.append("Debug: " + QString::fromLocal8Bit(msg));
        cout << out.toStdString() << endl;
        break;
    case QtWarningMsg:
        out.append("Warning: " + QString::fromLocal8Bit(msg));
        cerr << out.toStdString() << endl;
        break;
    case QtCriticalMsg:
        out.append("Critical: " + QString::fromLocal8Bit(msg));
        cerr << out.toStdString() << endl;
        break;
    case QtFatalMsg:
        cerr << "Fatal: " << msg << endl;
        cerr.flush();

        logStream << "Fatal: " << msg << endl;
        logStream.flush();

        abort();
    }    
    logStream << out << endl;
    logStream.flush();
    cerr.flush();
    cout.flush();
}


int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Lightpack");
    QApplication::setOrganizationName("Lightpack");
    QApplication::setApplicationVersion(VERSION_STR);

    // Using locale codec for console output in messageOutput(..) function ( cout << qstring.toStdString() )
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    qInstallMsgHandler(messageOutput);

#ifdef PORTABLE_VERSION
    // Find application directory
    QString applicationDirPath( argv[0] );
    QFileInfo fileInfo( applicationDirPath );
    applicationDirPath = fileInfo.absoluteDir().absolutePath();
    cout << "Application directory: " << applicationDirPath.toStdString() << endl;
#else

#   ifdef Q_WS_WIN
    QString applicationDirPath = QDir::homePath() + "/Lightpack";
#   else
    QString applicationDirPath = QDir::homePath() + "/.Lightpack";
#   endif

    QDir dir( applicationDirPath );
    if(dir.exists() == false){
        cout << "mkdir " << applicationDirPath.toStdString() << endl;
        if(dir.mkdir( applicationDirPath ) == false){
            cerr << "Failed mkdir '" << applicationDirPath.toStdString() << "' for application generated stuff. Exit." << endl;
            return 3;
        }
    }
#endif

    QString logFilePath = applicationDirPath + "/Lightpack.log";
    if(openLogFile(logFilePath)){
        qDebug() << "Logs file: " << logFilePath;
    }else{
        cerr << "Log file '" << logFilePath.toStdString() << "' didn't opened. Exit." << endl;
        return 2;
    }

    // Default debug level
    debugLevel = Debug::LowLevel;

    if(argc > 1){
        if(strcmp(argv[1], "--off") == 0){
            AmbilightUsb ambilight_usb;
            ambilight_usb.offLeds();
            return 0;
        }else if(strcmp(argv[1], "--debug_high") == 0){
            debugLevel = Debug::HighLevel;
        }else if(strcmp(argv[1], "--debug_mid") == 0){
            debugLevel = Debug::MidLevel;
        }else if(strcmp(argv[1], "--debug_low") == 0){
            debugLevel = Debug::LowLevel;
        }else if(strcmp(argv[1], "--debug_zero") == 0){
            debugLevel = Debug::ZeroLevel;
        }else{
            showHelpMessage();
            return 1;
        }
    }

    QString debugLevelStr = "";
    switch( debugLevel ){
    case Debug::HighLevel:  debugLevelStr = "High"; break;
    case Debug::MidLevel:   debugLevelStr = "Mid"; break;
    case Debug::LowLevel:   debugLevelStr = "Low"; break;
    case Debug::ZeroLevel:  debugLevelStr = "Zero"; break;
    }
    qDebug() << "Debug level" << debugLevelStr;


    if(debugLevel > 0){
        qDebug() << "Build with Qt verison:" << QT_VERSION_STR;
        qDebug() << "Qt version currently in use:" << qVersion();

#ifdef Q_WS_WIN
        switch( QSysInfo::windowsVersion() ){
        case QSysInfo::WV_NT:       qDebug() << "Windows NT (operating system version 4.0)"; break;
        case QSysInfo::WV_2000:     qDebug() << "Windows 2000 (operating system version 5.0)"; break;
        case QSysInfo::WV_XP:       qDebug() << "Windows XP (operating system version 5.1)"; break;
        case QSysInfo::WV_2003:     qDebug() << "Windows Server 2003, Windows Server 2003 R2, Windows Home Server, Windows XP Professional x64 Edition (operating system version 5.2)"; break;
        case QSysInfo::WV_VISTA:    qDebug() << "Windows Vista, Windows Server 2008 (operating system version 6.0)"; break;
        case QSysInfo::WV_WINDOWS7: qDebug() << "Windows 7, Windows Server 2008 R2 (operating system version 6.1)"; break;
        default:                    qDebug() << "Unknown windows version:" << QSysInfo::windowsVersion();
        }
#endif
        }


    // Update desktop widht and height
    Desktop::Initialize();

    // Open last used profile, if profile doesn't exists it will be created
    // Desktop::Width and Desktop::Height should be initialized
    Settings::Initialize( applicationDirPath );

    Q_INIT_RESOURCE(LightpackResources);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, "Lightpack",
                              "I couldn't detect any system tray on this system.");
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow *window = new MainWindow();   /* Create MainWindow */
    window->setVisible(false);   /* And load to tray. */

    window->startAmbilight();

    qDebug() << "Start main event loop: app.exec();";

    return app.exec();
}
