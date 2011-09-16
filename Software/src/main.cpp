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

using namespace std;

unsigned g_debugLevel = DEBUG_LEVEL_DEFAULT;
QTextStream m_logStream;
QMutex m_mutex;

QString createApplicationDirectory(const char * firstCmdArgument)
{
    QString appDirPath = QString(firstCmdArgument);

#ifdef PORTABLE_VERSION
    // Find application directory
    QFileInfo fileInfo(appDirPath);
    appDirPath = fileInfo.absoluteDir().absolutePath();
    cout << "Application directory: " << appDirPath.toStdString() << endl;

#else

#   ifdef Q_WS_WIN
    appDirPath = QDir::homePath() + "/Lightpack";
#   else
    appDirPath = QDir::homePath() + "/.Lightpack";
#   endif

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
#endif

    return appDirPath;
}

void openLogsFile(const QString & appDirPath)
{
    QString logFilePath = appDirPath + "/Lightpack.log";

    QFile *logFile = new QFile(logFilePath);
    QIODevice::OpenMode openFileAppendOrTruncateFlag = QIODevice::Append;
    QFileInfo info(logFilePath);
    if(info.size() > 1*1024*1024){
        cout << "Log file size > 1 Mb. I'm going to clear it. Now!" << endl;
        openFileAppendOrTruncateFlag = QIODevice::Truncate;
    }
    if(logFile->open(QIODevice::WriteOnly | openFileAppendOrTruncateFlag | QIODevice::Text)){
        m_logStream.setDevice(logFile);
        m_logStream << endl;
        m_logStream << QDateTime::currentDateTime().date().toString("yyyy_MM_dd") << " ";
        m_logStream << QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz") << " Lightpack sw" << VERSION_STR << endl;
    }else{
        cerr << "Failed to open logs file: '" << logFilePath.toStdString() << "'. Exit." << endl;
        exit(LightpackApplication::OpenLogsFail_ErrorCode);
    }

    qDebug() << "Logs file: " << logFilePath;
}

void messageHandler(QtMsgType type, const char *msg)
{
    QMutexLocker locker(&m_mutex);

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

        m_logStream << "Fatal: " << msg << endl;
        m_logStream.flush();

        exit(LightpackApplication::QFatalMessageHandler_ErrorCode);
    }
    m_logStream << out << endl;
    m_logStream.flush();
    cerr.flush();
    cout.flush();
}

int main(int argc, char **argv)
{
    // Using locale codec for console output in messageHandler(..) function ( cout << qstring.toStdString() )
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    QString appDirPath = createApplicationDirectory(argv[0]);

    openLogsFile(appDirPath);

    qInstallMsgHandler(messageHandler);

    LightpackApplication lightpackApp(appDirPath, argc, argv);

    Q_INIT_RESOURCE(LightpackResources);


    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << lightpackApp.thread()->currentThreadId();

    qDebug() << "Start main event loop: lightpackApp.exec();";

    return lightpackApp.exec();
}
