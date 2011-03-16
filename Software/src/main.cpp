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

QTextStream logStream;

// Make it global for access in messageOutput() for returning logs to window
MainWindow * window;

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

    if(window != NULL){
        if(logWhileWindowNotInitialized != ""){
            logWhileWindowNotInitialized.truncate( logWhileWindowNotInitialized.length() - 1);
            window->appendLogsLine(logWhileWindowNotInitialized);
            logWhileWindowNotInitialized = "";
        }
        window->appendLogsLine(out);
    }else{
        logWhileWindowNotInitialized.append(out + "\n");
    }
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
    // Find directory of the application
    QString applicationDirPath( argv[0] );
    QFileInfo fileInfo( applicationDirPath );
    applicationDirPath = fileInfo.absoluteDir().absolutePath();
    cout << "Application directory: " << applicationDirPath.toStdString() << endl;
#else
    QString applicationDirPath = QDir::homePath() + "/.Lightpack";
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

    qDebug() << "Build with Qt verison:" << QT_VERSION_STR;
    qDebug() << "Qt version currently in use:" << qVersion();


    if(argc > 1){
        if(strcmp(argv[1], "--off") == 0){
            AmbilightUsb ambilight_usb;
            ambilight_usb.offLeds();
            return 0;
        }else{
            showHelpMessage();
            return 1;
        }
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


    window = new MainWindow();   /* Create MainWindow */
    window->setVisible(false);   /* And load to tray. */
    window->setLogsFilePath(logFilePath);

    window->startAmbilight();

    // Don't touch me!!!
    qDebug() << "call app.exec();";

    return app.exec();
}
