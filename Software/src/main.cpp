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
#if !defined NOMINMAX
#define NOMINMAX
#endif

#if !defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#ifdef Q_OS_MACX
#import <Foundation/NSProcessInfo.h>
#import <CoreServices/CoreServices.h>
#endif

using namespace std;

unsigned g_debugLevel = SettingsScope::Main::DebugLevelDefault;

class LogWriter {
public:
    enum Level { Debug, Warn, Critical, Fatal, LevelCount };

    LogWriter() { Q_ASSERT(g_logWriter == NULL); }
    ~LogWriter() { Q_ASSERT(g_logWriter == NULL); }

    int initWith(const QString& logsDirPath)
    {
        // Using locale codec for console output in messageHandler(..) function ( cout << qstring.toStdString() )
        QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());

        QDir logsDir(logsDirPath);
        if (logsDir.exists() == false)
        {
            cout << "mkdir " << logsDirPath.toStdString() << endl;
            if (logsDir.mkdir(logsDirPath) == false)
            {
                cerr << "Failed mkdir '" << logsDirPath.toStdString() << "' for logs. Exit." << endl;
                return LightpackApplication::LogsDirecroryCreationFail_ErrorCode;
            }
        }

        if (rotateLogFiles(logsDir) == false)
            cerr << "Failed to rotate old log files." << endl;

        const QString logFilePath = logsDirPath + "/Prismatik.0.log";
        QScopedPointer<QFile> logFile(new QFile(logFilePath));
        if (logFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            m_logStream.setDevice(logFile.take());
            m_logStream << endl;

            const QDateTime currentDateTime(QDateTime::currentDateTime());
            m_logStream << currentDateTime.date().toString("yyyy_MM_dd") << " ";
            m_logStream << currentDateTime.time().toString("hh:mm:ss:zzz") << " Prismatik " << VERSION_STR << endl;
        } else {
            cerr << "Failed to open logs file: '" << logFilePath.toStdString() << "'. Exit." << endl;
            return LightpackApplication::OpenLogsFail_ErrorCode;
        }

        qDebug() << "Logs file:" << logFilePath;

        return LightpackApplication::OK_ErrorCode;
    }

    void writeMessage(const QString& msg, Level level = Debug)
    {
        static const char* s_logLevelNames[] = { "Debug", "Warning", "Critical", "Fatal" };
        Q_ASSERT(level >= Debug && level < LevelCount);
        Q_STATIC_ASSERT(sizeof(s_logLevelNames)/sizeof(s_logLevelNames[0]) == LevelCount);

        const QString timeMark = QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz");
        QMutexLocker locker(&m_mutex);
        cerr << timeMark.toStdString() << s_logLevelNames[level] << ':' << msg.toStdString() << endl;
        m_logStream << timeMark << s_logLevelNames[level] << ':' << msg << endl;
        m_logStream.flush();
    }

    struct ScopedMessageHandler {
        QtMessageHandler m_oldHandler;

        ScopedMessageHandler(LogWriter* logWriter)
            : m_oldHandler(qInstallMessageHandler(&LogWriter::messageHandler))
        {
            LogWriter::g_logWriter = logWriter;
        }

        ~ScopedMessageHandler()
        {
            qInstallMessageHandler(m_oldHandler);
            LogWriter::g_logWriter = NULL;
        }
    };

private:
    static const int StoreLogsLaunches = 5;
    static LogWriter* g_logWriter;

    static void messageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
    {
        static const LogWriter::Level s_msgType2Loglevel[] = {
            LogWriter::Debug, LogWriter::Warn, LogWriter::Critical, LogWriter::Fatal, LogWriter::Debug
        };
        Q_ASSERT(type >= 0 && type < sizeof(s_msgType2Loglevel)/sizeof(s_msgType2Loglevel[0]));
        Q_UNUSED(ctx);
        if (g_logWriter)
            g_logWriter->writeMessage(msg, s_msgType2Loglevel[type]);

        if (type == QtFatalMsg) {
            exit(LightpackApplication::QFatalMessageHandler_ErrorCode);
        }
    }

    static bool rotateLogFiles(const QDir& logsDir) {
        if (!logsDir.exists())
            return false;

        QStringList logFiles = logsDir.entryList(QStringList("Prismatik.?.log"), QDir::Files, QDir::Name);
        // Delete all old files except last |StoreLogsLaunches| files.
        for (int i = logFiles.count() - 1; i >= StoreLogsLaunches - 1; i--)
        {
            if (!QFile::remove(logsDir.absoluteFilePath(logFiles.at(i)))) {
                qCritical() << "Failed to remove log: " << logFiles.at(i);
                return false;
            }
        }

        logFiles = logsDir.entryList(QStringList("Prismatik.?.log"), QDir::Files, QDir::Name);
        Q_ASSERT(logFiles.count() <= StoreLogsLaunches);
        // Move Prismatik.[N].log file to Prismatik.[N+1].log
        for (int i = logFiles.count() - 1; i >= 0; i--)
        {
            const QStringList& splitted = logFiles[i].split('.');
            Q_ASSERT(splitted.length() == 3);

            const int num = splitted.at(1).toInt();
            const QString from = logsDir.absoluteFilePath(logFiles.at(i));
            const QString to = logsDir.absoluteFilePath(QString("Prismatik.") + QString::number(num + 1) + ".log");

            if (QFile::exists(to))
                QFile::remove(to);

            qDebug() << "Rename log: " << from << " to " << to;

            if (!QFile::rename(from, to)) {
                qCritical() << "Failed to rename log: " << from << " to " << to;
                return false;
            }
        }

        return true;
    }

    QTextStream m_logStream;
    QMutex m_mutex;
};

// static
LogWriter* LogWriter::g_logWriter = NULL;

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

int main(int argc, char **argv)
{

#if defined Q_OS_MACX && MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_10_9
    id activity;
    SInt32 version = 0;
    Gestalt( gestaltSystemVersion, &version );
    bool endActivityRequired = false;
    if ( version >= 0x1090 ) {
      activity = [[NSProcessInfo processInfo] beginActivityWithOptions: NSActivityLatencyCritical reason:@"Prismatik is latency-critical app"];
      endActivityRequired = true;
      DEBUG_LOW_LEVEL << "Latency critical activity is started";
    }
#elif defined Q_OS_WIN
    // Mutex used by InnoSetup to detect that app is runnning.
    // http://www.jrsoftware.org/ishelp/index.php?topic=setup_appmutex
    HANDLE appMutex = CreateMutexW(NULL, FALSE, L"LightpackAppMutex");
#endif

    const QString appDirPath = getApplicationDirectoryPath(argv[0]);

    LogWriter logWriter;
    const int logInitResult = logWriter.initWith(appDirPath + "/Logs");
    if (logInitResult != LightpackApplication::OK_ErrorCode) {
        exit(logInitResult);
    }

    LogWriter::ScopedMessageHandler messageHandlerGuard(&logWriter);
    Q_UNUSED(messageHandlerGuard);

    LightpackApplication lightpackApp(argc, argv);
    lightpackApp.setLibraryPaths(QStringList(appDirPath + "/plugins"));
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

#if defined Q_OS_MACX && MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_10_9
    if (endActivityRequired)
      [[NSProcessInfo processInfo] endActivity: activity];
#elif defined Q_OS_WIN
    CloseHandle(appMutex);
#endif

    return returnCode;
}
