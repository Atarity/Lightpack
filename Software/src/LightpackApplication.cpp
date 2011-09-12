/*
 * LightpackApplication.cpp
 *
 *  Created on: 06.09.2011
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

#include "LightpackApplication.hpp"
#include "LedDeviceLightpack.hpp"
#include "version.h"

#include <unistd.h>
#include <stdio.h>
#include <iostream>
using namespace std;

LightpackApplication::LightpackApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
    setApplicationName("Lightpack");
    setOrganizationName("Lightpack");
    setApplicationVersion(VERSION_STR);
    setQuitOnLastWindowClosed(false);

    processCommandLineArguments();

    printVersionsSoftwareQtOS();

    Settings::Initialize(m_applicationDirPath, m_isDebugLevelObtainedFromCmdArgs);

    checkSystemTrayAvailability();

    m_settingsWindow = new SettingsWindow();   /* Create MainWindow */
    m_settingsWindow->setVisible(false);   /* And load to tray. */

    // Register QMetaType for Qt::QueuedConnection
    qRegisterMetaType< QList<QRgb> >("QList<QRgb>");
    qRegisterMetaType<Backlight::Status>("Backlight::Status");

    startLedDeviceFactory();

    startApiServer();

    m_settingsWindow->startBacklight();
}

void LightpackApplication::processCommandLineArguments()
{
    g_debugLevel = DEBUG_LEVEL_DEFAULT;

    m_isDebugLevelObtainedFromCmdArgs = false;

    for (int i = 1; i < arguments().count(); i++)
    {
        if (arguments().at(i) == "--off")
        {
            LedDeviceLightpack lightpackDevice;
            lightpackDevice.offLeds();
            exit(OK_ErrorCode);
        }
        else if (arguments().at(i) =="--debug-high")
        {
            g_debugLevel = Debug::HighLevel;
            m_isDebugLevelObtainedFromCmdArgs = true;
        }
        else if (arguments().at(i) =="--debug-mid")
        {
            g_debugLevel = Debug::MidLevel;
            m_isDebugLevelObtainedFromCmdArgs = true;

        }
        else if (arguments().at(i) =="--debug-low")
        {
            g_debugLevel = Debug::LowLevel;
            m_isDebugLevelObtainedFromCmdArgs = true;

        }
        else if (arguments().at(i) =="--debug-zero")
        {
            g_debugLevel = Debug::ZeroLevel;
            m_isDebugLevelObtainedFromCmdArgs = true;
        } else {
            qDebug() << "Wrong argument:" << arguments().at(i);
            printHelpMessage();
            exit(WrongCommandLineArgument_ErrorCode);
        }
    }
    qDebug() << "Debug level" << g_debugLevel;
}

void LightpackApplication::printHelpMessage() const
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Project  : Lightpack \n");
    fprintf(stderr, "Author   : Mike Shatohin \n");
    fprintf(stderr, "Version  : %s\n", VERSION_STR);
#ifdef HG_REVISION
    fprintf(stderr, "Revision : %s\n", HG_REVISION);
#endif
    fprintf(stderr, "Site     : lightpack.googlecode.com \n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Build with Qt version %s\n", QT_VERSION_STR);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options: \n");
    fprintf(stderr, "  --off         - send 'off leds' cmd to device \n");
    fprintf(stderr, "  --help        - show this help \n");
    fprintf(stderr, "  --debug-high  - maximum verbose level of debug output\n");
    fprintf(stderr, "  --debug-mid   - middle debug level\n");
    fprintf(stderr, "  --debug-low   - low debug level, DEFAULT\n");
    fprintf(stderr, "  --debug-zero  - minimum debug output\n");
    fprintf(stderr, "\n");
}

#ifdef Q_OS_LINUX
//char * getSystemOutput(const char * cmd)
//{
//    /*
//     * TODO: http://stackoverflow.com/questions/3852587/how-to-get-stdout-from-a-qprocess
//     */
//}
#endif

void LightpackApplication::printVersionsSoftwareQtOS() const
{
    if (g_debugLevel > 0)
    {
        qDebug() << "Lightpack:" << VERSION_STR;
        qDebug() << "Build with Qt verison:" << QT_VERSION_STR;
        qDebug() << "Qt version currently in use:" << qVersion();

#       ifdef Q_OS_WIN
        switch(QSysInfo::windowsVersion()){
        case QSysInfo::WV_NT:       qDebug() << "Windows NT (operating system version 4.0)"; break;
        case QSysInfo::WV_2000:     qDebug() << "Windows 2000 (operating system version 5.0)"; break;
        case QSysInfo::WV_XP:       qDebug() << "Windows XP (operating system version 5.1)"; break;
        case QSysInfo::WV_2003:     qDebug() << "Windows Server 2003, Windows Server 2003 R2, Windows Home Server, Windows XP Professional x64 Edition (operating system version 5.2)"; break;
        case QSysInfo::WV_VISTA:    qDebug() << "Windows Vista, Windows Server 2008 (operating system version 6.0)"; break;
        case QSysInfo::WV_WINDOWS7: qDebug() << "Windows 7, Windows Server 2008 R2 (operating system version 6.1)"; break;
        default:                    qDebug() << "Unknown windows version:" << QSysInfo::windowsVersion();
        }
#       elif defined(Q_OS_LINUX)
        //        $ lsb_release -a
        //        No LSB modules are available.
        //        Distributor ID:	Ubuntu
        //        Description:	Ubuntu 10.04.3 LTS
        //        Release:	10.04
        //        Codename:	lucid
//        QString linuxDistrVersion = getSystemOutput("lsb_release -a");
//        QRegExp rx("Description:\\s+([^\\n]+)");
//        if (rx.indexIn(linuxDistrVersion) != -1)
//        {
//            linuxDistrVersion = rx.cap(1);
//            qDebug() << "Linux distribution version (lsb_release -a):" << linuxDistrVersion;
//        }
#       elif defined(Q_OS_MAC)
        qDebug() << "Mac OS";
#       else
        qDebug() << "Unknown operation system";
#       endif
    }
}

void LightpackApplication::checkSystemTrayAvailability() const
{
    if (QSystemTrayIcon::isSystemTrayAvailable() == false)
    {
        QMessageBox::critical(0, "Lightpack", "I couldn't detect any system tray on this system.");
        qFatal("%s %s", Q_FUNC_INFO, "I couldn't detect any system tray on this system.");
    }
}

void LightpackApplication::startApiServer()
{
    if (Settings::isEnabledApi())
    {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Start API server";

        int port = Settings::getApiPort();

        m_apiServer = new ApiServer();
        m_apiServerThread = new QThread();

        connect(m_apiServer, SIGNAL(updateLedsColors(QList<QRgb>)), m_ledDeviceFactory, SLOT(setColorsIfDeviceAvailable(QList<QRgb>)), Qt::QueuedConnection);
        connect(m_apiServer, SIGNAL(updateSmooth(int)), m_ledDeviceFactory, SIGNAL(setSmoothSlowdown(int)), Qt::QueuedConnection);

        connect(m_apiServer, SIGNAL(requestBacklightStatus()), m_settingsWindow, SLOT(requestBacklightStatus()));
        connect(m_settingsWindow, SIGNAL(resultBacklightStatus(Backlight::Status)), m_apiServer, SLOT(resultBacklightStatus(Backlight::Status)));

//        m_apiServer->ApiKey = Settings::getApiKey();

        if (!m_apiServer->listen(QHostAddress::Any, port)) {
            QString errorStr = tr("API server unable to start (port: %1): %2.").arg(port).arg(m_apiServer->errorString());

            QMessageBox::critical(0, tr("API Server"), errorStr);
            qCritical() << Q_FUNC_INFO << errorStr;
        }

        m_apiServer->moveToThread(m_apiServerThread);
        m_apiServerThread->start();
    }
}

void LightpackApplication::startLedDeviceFactory()
{
    m_ledDeviceFactory = new LedDeviceFactory();
    m_ledDeviceFactoryThread = new QThread();

    connect(m_settingsWindow, SIGNAL(recreateLedDevice()), m_ledDeviceFactory, SLOT(recreateLedDevice()), Qt::DirectConnection);
    connect(m_settingsWindow, SIGNAL(updateLedsColors(const QList<QRgb> &)), m_ledDeviceFactory, SLOT(setColorsIfDeviceAvailable(QList<QRgb>)), Qt::QueuedConnection);

    connect(m_settingsWindow, SIGNAL(updateColorDepth(int)), m_ledDeviceFactory, SIGNAL(setColorDepth(int)), Qt::QueuedConnection);
    connect(m_settingsWindow, SIGNAL(updateSmoothSlowdown(int)), m_ledDeviceFactory, SIGNAL(setSmoothSlowdown(int)), Qt::QueuedConnection);
    connect(m_settingsWindow, SIGNAL(updateTimerOptions(int,int)), m_ledDeviceFactory, SIGNAL(setTimerOptions(int,int)), Qt::QueuedConnection);
    connect(m_settingsWindow, SIGNAL(requestFirmwareVersion()), m_ledDeviceFactory, SIGNAL(requestFirmwareVersion()), Qt::QueuedConnection);

    connect(m_ledDeviceFactory, SIGNAL(openDeviceSuccess(bool)), m_settingsWindow, SLOT(ledDeviceCallSuccess(bool)), Qt::QueuedConnection);
    connect(m_ledDeviceFactory, SIGNAL(ioDeviceSuccess(bool)), m_settingsWindow, SLOT(ledDeviceCallSuccess(bool)), Qt::QueuedConnection);
    connect(m_ledDeviceFactory, SIGNAL(firmwareVersion(QString)), m_settingsWindow, SLOT(ledDeviceGetFirmwareVersion(QString)), Qt::QueuedConnection);

    m_ledDeviceFactory->moveToThread(m_ledDeviceFactoryThread);
    m_ledDeviceFactoryThread->start();
}
