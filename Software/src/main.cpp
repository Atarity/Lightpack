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
#include <QFileInfo>
#include <QMetaType>
#include <iostream>

#include "LightpackApplication.hpp"
#include "Settings.hpp"
#include "version.h"
#include "debug.h"

#include "SettingsWizard.hpp"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_MACX
#import <Foundation/NSProcessInfo.h>
#import <CoreServices/CoreServices.h>
#endif

using namespace std;

static const int StoreLogsLaunches = 5;

unsigned g_debugLevel = SettingsScope::Main::DebugLevelDefault;
QTextStream m_logStream;
QMutex m_mutex;

QString getApplicationDirectoryPath(const char * firstCmdArgument)
{    
    QFileInfo fileInfo(firstCmdArgument);
    QString appDirPath = fileInfo.absoluteDir().absolutePath();

    QString lightpackMainConfPath = appDirPath + "/main.conf";

    cout << lightpackMainConfPath.toStdString() << endl;

    if (QFile::exists(lightpackMainConfPath))
    {
        /// Portable version
        /// Store logs and settings in application directory

        cout << "Portable version" << endl;
    }
    else
    {
        /// Unportable version
        /// Store logs and settings in home directory of the current user

        cout << "Unportable version" << endl;

        QString home = QDir::homePath();
        QString normalizedHome = home.endsWith("/") ? home.left(home.size() - 1) : home;

#       ifdef Q_OS_WIN
        appDirPath = normalizedHome  + "/Prismatik";
#       else
        appDirPath = normalizedHome + "/.Prismatik";
#       endif

        QDir dir(appDirPath);
        if (dir.exists() == false)
        {
            cout << "mkdir " << appDirPath.toStdString() << endl;
            if (dir.mkdir(appDirPath) == false)
            {
                cerr << "Failed mkdir '" << appDirPath.toStdString() << "' for application generated stuff. Exit." << endl;
                exit(LightpackApplication::AppDirectoryCreationFail_ErrorCode);
            }
        }
    }

    cout << "Application directory: " << appDirPath.toStdString() << endl;

    return appDirPath;
}

void openLogsFile(const QString & appDirPath)
{
    QString logsDirPath = appDirPath + "/Logs";

    QDir logsDir(logsDirPath);
    if (logsDir.exists() == false)
    {
        cout << "mkdir " << logsDirPath.toStdString() << endl;
        if (logsDir.mkdir(logsDirPath) == false)
        {
            cerr << "Failed mkdir '" << logsDirPath.toStdString() << "' for logs. Exit." << endl;
            exit(LightpackApplication::LogsDirecroryCreationFail_ErrorCode);
        }
    }

    QString logFilePath = logsDirPath + "/Prismatik.0.log";

    QStringList logFiles = logsDir.entryList(QStringList("Prismatik.?.log"), QDir::Files, QDir::Name);

    for (int i = logFiles.count() - 1; i >= 0; i--)
    {
        QString num = logFiles[i].split('.').at(1);
        QString from = logsDirPath + "/" + QString("Prismatik.") + num + ".log";
        QString to = logsDirPath + "/" + QString("Prismatik.") + QString::number(num.toInt() + 1) + ".log";

        if (i >= StoreLogsLaunches - 1)
        {
            QFile::remove(from);
            continue;
        }

        if (QFile::exists(to))
            QFile::remove(to);

        qDebug() << "Rename log:" << from << "to" << to;

        bool ok = QFile::rename(from, to);
        if (!ok)
            qCritical() << "Fail rename log:" << from << "to" << to;
    }

    QFile *logFile = new QFile(logFilePath);

    if (logFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        m_logStream.setDevice(logFile);
        m_logStream << endl;
        m_logStream << QDateTime::currentDateTime().date().toString("yyyy_MM_dd") << " ";
        m_logStream << QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz") << " Prismatik " << VERSION_STR << endl;
    } else {
        cerr << "Failed to open logs file: '" << logFilePath.toStdString() << "'. Exit." << endl;
        exit(LightpackApplication::OpenLogsFail_ErrorCode);
    }

    qDebug() << "Logs file:" << logFilePath;
}

void messageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    Q_UNUSED(ctx);
    QMutexLocker locker(&m_mutex);

    QString out = QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz") + " ";
    switch (type) {
    case QtDebugMsg:
        out.append("Debug: " + msg);
        cout << out.toStdString() << endl;
        break;
    case QtWarningMsg:
        out.append("Warning: " + msg);
        cerr << out.toStdString() << endl;
        break;
    case QtCriticalMsg:
        out.append("Critical: " + msg);
        cerr << out.toStdString() << endl;
        break;
    case QtFatalMsg:
        cerr << "Fatal: " << msg.toStdString() << endl;
        cerr.flush();

        m_logStream << "Fatal: " << msg << endl;
        m_logStream.flush();

        QApplication::exit(LightpackApplication::QFatalMessageHandler_ErrorCode);
    }

    m_logStream << out << endl;
    m_logStream.flush();
    cerr.flush();
    cout.flush();
}

int main(int argc, char **argv)
{

#ifdef Q_OS_MACX
    id activity;
    SInt32 version = 0;
    Gestalt( gestaltSystemVersion, &version );
    bool endActivityRequired = false;
    if ( version >= 0x1090 ) {
      activity = [[NSProcessInfo processInfo] beginActivityWithOptions: NSActivityLatencyCritical reason:@"Prismatik is latency-critical app"];
      endActivityRequired = true;
      DEBUG_LOW_LEVEL << "Latency critical activity is started";
    }
#endif

#   ifdef Q_OS_WIN
    CreateMutex(NULL, FALSE, L"LightpackAppMutex");
#   endif
    // Using locale codec for console output in messageHandler(..) function ( cout << qstring.toStdString() )
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());

    QString appDirPath = getApplicationDirectoryPath(argv[0]);

    LightpackApplication lightpackApp(argc, argv);


    QStringList paths = QStringList() << getApplicationDirectoryPath(argv[0]) + "/plugins";
    lightpackApp.setLibraryPaths(paths);

    openLogsFile(appDirPath);

    qInstallMessageHandler(messageHandler);

    lightpackApp.initializeAll(appDirPath);

    if (lightpackApp.isRunning())
    {
        lightpackApp.sendMessage("Application already running");

        qWarning() << "Application already running";
        exit(0);
    }

    Q_INIT_RESOURCE(LightpackResources);

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << lightpackApp.thread()->currentThreadId();

    qDebug() << "Start main event loop: lightpackApp.exec();";

    int returnCode = lightpackApp.exec();

#ifdef Q_OS_MACX
    if (endActivityRequired)
      [[NSProcessInfo processInfo] endActivity: activity];
#endif

    return returnCode;
}
