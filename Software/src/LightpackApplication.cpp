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
using namespace SettingsScope;

LightpackApplication::LightpackApplication(int &argc, char **argv)
    : QtSingleApplication(argc, argv)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
}

void LightpackApplication::initializeAll(const QString & appDirPath)
{
    setApplicationName("Lightpack");
    setOrganizationName("Lightpack");
    setApplicationVersion(VERSION_STR);
    setQuitOnLastWindowClosed(false);

    m_applicationDirPath = appDirPath;

    processCommandLineArguments();

    printVersionsSoftwareQtOS();

    Settings::Initialize(m_applicationDirPath, m_isDebugLevelObtainedFromCmdArgs);

    checkSystemTrayAvailability();

    m_settingsWindow = new SettingsWindow();
    m_settingsWindow->setVisible(false); /* Load to tray */

    // Process messages from another instances in SettingsWindow
    connect(this, SIGNAL(messageReceived(QString)), m_settingsWindow, SLOT(processMessage(QString)));

    // Register QMetaType for Qt::QueuedConnection
    qRegisterMetaType< QList<QRgb> >("QList<QRgb>");
    qRegisterMetaType<Backlight::Status>("Backlight::Status");
    qRegisterMetaType<Api::DeviceLockStatus>("Api::DeviceLockStatus");

    startLedDeviceFactory();

    startApiServer();

    connect(m_settingsWindow, SIGNAL(backlightStatusChanged(Backlight::Status)), this, SLOT(backlightStatusChanged(Backlight::Status)));

    m_settingsWindow->startBacklight();
}

void LightpackApplication::backlightStatusChanged(Backlight::Status status)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << status;

    switch (status)
    {
    case Backlight::StatusOn:
    case Backlight::StatusDeviceError:
        connectApiServerAndLedDeviceSignalsSlots();
        break;

    case Backlight::StatusOff:
        disconnectApiServerAndLedDeviceSignalsSlots();
        emit clearColorBuffers();
        break;

    default:
        qWarning() << Q_FUNC_INFO << "status contains crap =" << status;
        break;
    }
}

void LightpackApplication::processCommandLineArguments()
{
    g_debugLevel = SettingsScope::Main::DebugLevelDefault;

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

    if (m_isDebugLevelObtainedFromCmdArgs)
    {
        qDebug() << "Debug level" << g_debugLevel;
    }
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
#       ifdef HG_REVISION
        qDebug() << "Lightpack:" << VERSION_STR << "rev." << HG_REVISION;
#       else
        qDebug() << "Lightpack:" << VERSION_STR;
#       endif

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
#   ifdef Q_OS_LINUX
    // When you add lightpack in the Startup in Ubuntu (10.04), tray starts later than the application runs.
    // Check availability tray every second for 20 seconds.
    for (int i = 0; i < 20; i++)
    {
        if (QSystemTrayIcon::isSystemTrayAvailable())
            break;

        sleep(1);
    }
#   endif

    if (QSystemTrayIcon::isSystemTrayAvailable() == false)
    {
        QMessageBox::critical(0, "Lightpack", "I couldn't detect any system tray on this system.");
        qFatal("%s %s", Q_FUNC_INFO, "I couldn't detect any system tray on this system.");
    }
}

void LightpackApplication::startApiServer()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Start API server";

    m_isApiServerConnectedToLedDeviceSignalsSlots = false;

    m_apiServer = new ApiServer();
    m_apiServerThread = new QThread();

    connect(this, SIGNAL(clearColorBuffers()), m_apiServer, SIGNAL(clearColorBuffers()));

    connect(m_settingsWindow, SIGNAL(enableApiServer(bool)),    m_apiServer, SLOT(enableApiServer(bool)));
    connect(m_settingsWindow, SIGNAL(enableApiAuth(bool)),      m_apiServer, SLOT(enableApiAuth(bool)));
    connect(m_settingsWindow, SIGNAL(updateApiKey(QString)),    m_apiServer, SLOT(updateApiKey(QString)));
    connect(m_settingsWindow, SIGNAL(updateApiPort(int)),       m_apiServer, SLOT(updateApiPort(int)));    
    connect(m_settingsWindow, SIGNAL(resultBacklightStatus(Backlight::Status)), m_apiServer, SLOT(resultBacklightStatus(Backlight::Status)));

    connect(m_settingsWindow, SIGNAL(updateApiDeviceNumberOfLeds(int)), m_apiServer, SIGNAL(updateApiDeviceNumberOfLeds(int)));

    connect(m_apiServer, SIGNAL(errorOnStartListening(QString)), m_settingsWindow, SLOT(onApiServer_ErrorOnStartListening(QString)));
    connect(m_apiServer, SIGNAL(requestBacklightStatus()),       m_settingsWindow, SLOT(requestBacklightStatus()));

    connect(m_apiServer, SIGNAL(updateProfile(QString)),                        m_settingsWindow, SLOT(profileSwitch(QString)));
    connect(m_apiServer, SIGNAL(updateStatus(Backlight::Status)),               m_settingsWindow, SLOT(setBacklightStatus(Backlight::Status)));
    connect(m_apiServer, SIGNAL(updateDeviceLockStatus(Api::DeviceLockStatus)), m_settingsWindow, SLOT(setDeviceLockViaAPI(Api::DeviceLockStatus)));

    if (Settings::isBacklightEnabled())
    {
        connectApiServerAndLedDeviceSignalsSlots();
    }

    m_apiServer->firstStart();

    m_apiServer->moveToThread(m_apiServerThread);
    m_apiServerThread->start();
}

void LightpackApplication::startLedDeviceFactory()
{
    m_ledDeviceFactory = new LedDeviceFactory();
    m_ledDeviceFactoryThread = new QThread();

    connect(m_settingsWindow, SIGNAL(recreateLedDevice()),                      m_ledDeviceFactory, SLOT(recreateLedDevice()), Qt::DirectConnection);
    connect(m_settingsWindow, SIGNAL(updateLedsColors(const QList<QRgb> &)),    m_ledDeviceFactory, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);

    connect(m_settingsWindow, SIGNAL(offLeds()),                    m_ledDeviceFactory, SLOT(offLeds()), Qt::QueuedConnection);
    connect(m_settingsWindow, SIGNAL(updateColorDepth(int)),        m_ledDeviceFactory, SLOT(setColorDepth(int)), Qt::QueuedConnection);
    connect(m_settingsWindow, SIGNAL(updateSmoothSlowdown(int)),    m_ledDeviceFactory, SLOT(setSmoothSlowdown(int)), Qt::QueuedConnection);
    connect(m_settingsWindow, SIGNAL(updateRefreshDelay(int)),      m_ledDeviceFactory, SLOT(setRefreshDelay(int)), Qt::QueuedConnection);
    connect(m_settingsWindow, SIGNAL(updateGamma(double)),          m_ledDeviceFactory, SLOT(setGamma(double)), Qt::QueuedConnection);
    connect(m_settingsWindow, SIGNAL(updateBrightness(int)),        m_ledDeviceFactory, SLOT(setBrightness(int)), Qt::QueuedConnection);
    connect(m_settingsWindow, SIGNAL(requestFirmwareVersion()),     m_ledDeviceFactory, SLOT(requestFirmwareVersion()), Qt::QueuedConnection);
    connect(m_settingsWindow, SIGNAL(settingsProfileChanged()),     m_ledDeviceFactory, SLOT(updateDeviceSettings()), Qt::QueuedConnection);

    connect(m_ledDeviceFactory, SIGNAL(openDeviceSuccess(bool)),    m_settingsWindow, SLOT(ledDeviceOpenSuccess(bool)), Qt::QueuedConnection);
    connect(m_ledDeviceFactory, SIGNAL(ioDeviceSuccess(bool)),      m_settingsWindow, SLOT(ledDeviceCallSuccess(bool)), Qt::QueuedConnection);
    connect(m_ledDeviceFactory, SIGNAL(firmwareVersion(QString)),   m_settingsWindow, SLOT(ledDeviceFirmwareVersionResult(QString)), Qt::QueuedConnection);
    connect(m_ledDeviceFactory, SIGNAL(setColors_VirtualDeviceCallback(QList<QRgb>)), m_settingsWindow, SLOT(updateVirtualLedsColors(QList<QRgb>)), Qt::QueuedConnection);

    m_ledDeviceFactory->moveToThread(m_ledDeviceFactoryThread);
    m_ledDeviceFactoryThread->start();
}

void LightpackApplication::connectApiServerAndLedDeviceSignalsSlots()
{
    if (m_isApiServerConnectedToLedDeviceSignalsSlots == false)
    {
        connect(m_apiServer, SIGNAL(updateLedsColors(QList<QRgb>)), m_ledDeviceFactory, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
        connect(m_apiServer, SIGNAL(updateGamma(double)),           m_ledDeviceFactory, SLOT(setGamma(double)), Qt::QueuedConnection);
        connect(m_apiServer, SIGNAL(updateBrightness(int)),         m_ledDeviceFactory, SLOT(setBrightness(int)), Qt::QueuedConnection);
        connect(m_apiServer, SIGNAL(updateSmooth(int)),             m_ledDeviceFactory, SLOT(setSmoothSlowdown(int)), Qt::QueuedConnection);
        m_isApiServerConnectedToLedDeviceSignalsSlots = true;
    }
}

void LightpackApplication::disconnectApiServerAndLedDeviceSignalsSlots()
{
    if (m_isApiServerConnectedToLedDeviceSignalsSlots == true)
    {
        disconnect(m_apiServer, SIGNAL(updateLedsColors(QList<QRgb>)),  m_ledDeviceFactory, SLOT(setColors(QList<QRgb>)));
        disconnect(m_apiServer, SIGNAL(updateGamma(double)),            m_ledDeviceFactory, SLOT(setGamma(double)));
        disconnect(m_apiServer, SIGNAL(updateBrightness(int)),          m_ledDeviceFactory, SLOT(setBrightness(int)));
        disconnect(m_apiServer, SIGNAL(updateSmooth(int)),              m_ledDeviceFactory, SLOT(setSmoothSlowdown(int)));
        m_isApiServerConnectedToLedDeviceSignalsSlots = false;
    }
}

void LightpackApplication::commitData(QSessionManager &sessionManager)
{
    Q_UNUSED(sessionManager);

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Off leds before quit";

    if (m_ledDeviceFactory != NULL)
    {
        // Disable signals with new colors
        disconnect(m_settingsWindow, SIGNAL(updateLedsColors(QList<QRgb>)),  m_ledDeviceFactory, SLOT(setColors(QList<QRgb>)));
        disconnect(m_apiServer, SIGNAL(updateLedsColors(QList<QRgb>)),  m_ledDeviceFactory, SLOT(setColors(QList<QRgb>)));

        // Process all currently pending signals
        QApplication::processEvents(QEventLoop::AllEvents, 1000);

        // Send signal and process it
        m_ledDeviceFactory->offLeds();
        QApplication::processEvents(QEventLoop::AllEvents, 1000);
    }
}
