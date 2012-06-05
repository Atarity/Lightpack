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
#include "LightpackPluginInterface.hpp"
#include "version.h"

#include <unistd.h>
#include <stdio.h>
#include <iostream>


using namespace std;
using namespace SettingsScope;

struct QtMetaObject : private QObject
{
public:
    static const QMetaObject *get()
        { return &static_cast<QtMetaObject*>(0)->staticQtMetaObject; }
};

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
    m_noGui = false;

    processCommandLineArguments();

    printVersionsSoftwareQtOS();

    Settings::Initialize(m_applicationDirPath, m_isDebugLevelObtainedFromCmdArgs);

    if (!m_noGui)
    {
        checkSystemTrayAvailability();

        m_settingsWindow = new SettingsWindow();
        m_settingsWindow->setVisible(false); /* Load to tray */
        m_settingsWindow->createTrayIcon();
        m_settingsWindow->connectSignalsSlots();
        // Process messages from another instances in SettingsWindow
        connect(this, SIGNAL(messageReceived(QString)), m_settingsWindow, SLOT(processMessage(QString)));
}
    // Register QMetaType for Qt::QueuedConnection
    qRegisterMetaType< QList<QRgb> >("QList<QRgb>");
    qRegisterMetaType< QList<QRgb> >("QList<PyPlugin*>");
    qRegisterMetaType<Lightpack::Mode>("Lightpack::Mode");
    qRegisterMetaType<Backlight::Status>("Backlight::Status");
    qRegisterMetaType<DeviceLocked::DeviceLockStatus>("DeviceLocked::DeviceLockStatus");

    if (Settings::isBacklightEnabled())
    {
        m_backlightStatus = Backlight::StatusOn;
    } else {
        m_backlightStatus = Backlight::StatusOff;
    }
    m_deviceLockStatus = DeviceLocked::Unlocked;

    startLedDeviceFactory();

    startApiServer();

    startGrabManager();

    startPluginManager();

    this->settingsChanged();
    startBacklight();

    if (!m_noGui)
    {
        connect(m_settingsWindow, SIGNAL(backlightStatusChanged(Backlight::Status)), this, SLOT(setStatusChanged(Backlight::Status)));
        m_settingsWindow->startBacklight();
    }
}



void LightpackApplication::setStatusChanged(Backlight::Status status)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << status;
     m_backlightStatus = status;
     startBacklight();
}

void LightpackApplication::setBacklightChanged(Lightpack::Mode mode)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << mode;
    Settings::setLightpackMode(mode);
    if (!m_noGui)
         m_settingsWindow->setModeChanged(mode);
    startBacklight();
}

void LightpackApplication::setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus status)
{
    m_deviceLockStatus = status;

    if (m_grabManager == NULL)
        qFatal("%s m_grabManager == NULL", Q_FUNC_INFO);
    startBacklight();
}

void LightpackApplication::startBacklight()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "m_backlightStatus =" << m_backlightStatus
                    << "m_deviceLockStatus =" << m_deviceLockStatus;

    bool isBacklightEnabled = (m_backlightStatus == Backlight::StatusOn || m_backlightStatus == Backlight::StatusDeviceError);
    bool isCanStart = (isBacklightEnabled && m_deviceLockStatus == DeviceLocked::Unlocked);

    m_pluginInterface->resultBacklightStatus(m_backlightStatus);

    Settings::setIsBacklightEnabled(isBacklightEnabled);

    switch (Settings::getLightpackMode())
    {
    case Lightpack::AmbilightMode:
        m_grabManager->start(isCanStart);
        m_moodlampManager->start(false);
        break;

    case Lightpack::MoodLampMode:
        m_grabManager->start(false);
        m_moodlampManager->start(isCanStart);
        break;
    }

    if (m_backlightStatus == Backlight::StatusOff)
        m_ledDeviceFactory->offLeds();

    switch (m_backlightStatus)
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
        qWarning() << Q_FUNC_INFO << "status contains crap =" << m_backlightStatus;
        break;
    }
}

void LightpackApplication::processCommandLineArguments()
{
    g_debugLevel = SettingsScope::Main::DebugLevelDefault;

    m_isDebugLevelObtainedFromCmdArgs = false;

    for (int i = 1; i < arguments().count(); i++)
    {
        if (arguments().at(i) == "--nogui")
        {
            m_noGui = true;
            DEBUG_LOW_LEVEL <<  "Application running no_GUI mode";
        }
        else if (arguments().at(i) == "--off")
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
    fprintf(stderr, "  --nogui         - no GUI (console mode) \n");
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

    if (!m_noGui)
    {
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
    connect(m_apiServer, SIGNAL(updateDeviceLockStatus(DeviceLocked::DeviceLockStatus)), m_settingsWindow, SLOT(setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus)));
    }
    else
    {
        connect(m_apiServer, SIGNAL(updateProfile(QString)),                        this, SLOT(profileSwitch(QString)));
        connect(m_apiServer, SIGNAL(updateStatus(Backlight::Status)),               this, SLOT(setStatusChanged(Backlight::Status)));
        connect(m_apiServer, SIGNAL(requestBacklightStatus()),       this, SLOT(requestBacklightStatus()));
    }

    connect(m_apiServer, SIGNAL(updateBacklight(Lightpack::Mode)) , this, SLOT(setBacklightChanged(Lightpack::Mode)));
    connect(m_ledDeviceFactory, SIGNAL(setColors_VirtualDeviceCallback(QList<QRgb>)), m_apiServer,    SLOT(updateColors(QList<QRgb>)), Qt::QueuedConnection);
    connect(m_apiServer, SIGNAL(updateDeviceLockStatus(DeviceLocked::DeviceLockStatus)), this, SLOT(setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus)));


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
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    m_ledDeviceFactory = new LedDeviceFactory();
    m_ledDeviceFactoryThread = new QThread();

     if (!m_noGui)
     {
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
}
    m_ledDeviceFactory->moveToThread(m_ledDeviceFactoryThread);
    m_ledDeviceFactoryThread->start();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "end";

}

void LightpackApplication::startGrabManager()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    m_grabManager = new GrabManager(NULL);
    m_moodlampManager = new MoodLampManager(NULL);

    // Connections to signals which will be connected to ILedDevice
    if (!m_noGui)
    {
        // Connect to GrabManager
        connect(m_settingsWindow, SIGNAL(settingsChanged()), this, SLOT(settingsChanged()));
        connect(m_settingsWindow, SIGNAL(showLedWidgets(bool)), this, SLOT(showLedWidgets(bool)));
        connect(m_settingsWindow, SIGNAL(setColoredLedWidget(bool)), this, SLOT(setColoredLedWidget(bool)));

        connect(m_settingsWindow, SIGNAL(settingsProfileChanged()), m_grabManager, SLOT(settingsProfileChanged()));
        // GrabManager to this
        connect(m_grabManager, SIGNAL(ambilightTimeOfUpdatingColors(double)), m_settingsWindow, SLOT(refreshAmbilightEvaluated(double)));

        // Connect to MoodLampManager
        connect(m_settingsWindow, SIGNAL(settingsProfileChanged()), m_moodlampManager, SLOT(settingsProfileChanged()));

        connect(m_grabManager, SIGNAL(updateLedsColors(QList<QRgb>)), m_settingsWindow, SIGNAL(updateLedsColors(QList<QRgb>)));
        connect(m_moodlampManager, SIGNAL(updateLedsColors(QList<QRgb>)), m_settingsWindow, SIGNAL(updateLedsColors(QList<QRgb>)));
    }
    else
    {
        connect(m_grabManager, SIGNAL(updateLedsColors(const QList<QRgb> &)),    m_ledDeviceFactory, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
        connect(m_moodlampManager, SIGNAL(updateLedsColors(const QList<QRgb> &)),    m_ledDeviceFactory, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
    }
    connect(m_grabManager, SIGNAL(ambilightTimeOfUpdatingColors(double)), m_apiServer, SLOT(refreshAmbilightEvaluated(double)));
    connect(m_grabManager,SIGNAL(changeScreen(QRect)),m_apiServer,SLOT(refreshScreenRect(QRect)));



}

void LightpackApplication::startPluginManager()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    consolePlugin = NULL;
    m_pluginInterface = new LightpackPluginInterface(NULL);
    m_pluginManager = new PluginManager(NULL);
    m_PluginThread = new QThread();
    QWidget* settingsBox = NULL;
     if (!m_noGui)
         settingsBox = m_settingsWindow->getSettingBox();

    m_pluginManager->init(m_pluginInterface,settingsBox);

    connect(m_pluginInterface, SIGNAL(updateDeviceLockStatus(DeviceLocked::DeviceLockStatus)), this, SLOT(setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus)));
    connect(m_pluginInterface, SIGNAL(updateLedsColors(const QList<QRgb> &)),    m_ledDeviceFactory, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
    connect(m_pluginInterface, SIGNAL(updateDeviceLockStatus(DeviceLocked::DeviceLockStatus)), m_apiServer, SLOT(setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus)));
    connect(m_apiServer, SIGNAL(updateDeviceLockStatus(DeviceLocked::DeviceLockStatus)), m_pluginInterface, SLOT(setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus)));

    connect(m_pluginInterface, SIGNAL(updateLedsColors(QList<QRgb>)), m_ledDeviceFactory, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
    connect(m_pluginInterface, SIGNAL(updateGamma(double)),           m_ledDeviceFactory, SLOT(setGamma(double)), Qt::QueuedConnection);
    connect(m_pluginInterface, SIGNAL(updateBrightness(int)),         m_ledDeviceFactory, SLOT(setBrightness(int)), Qt::QueuedConnection);
    connect(m_pluginInterface, SIGNAL(updateSmooth(int)),             m_ledDeviceFactory, SLOT(setSmoothSlowdown(int)), Qt::QueuedConnection);
    connect(m_pluginInterface, SIGNAL(requestBacklightStatus()),       this, SLOT(requestBacklightStatus()));

     if (!m_noGui)
     {
         connect(m_pluginInterface, SIGNAL(updateLedsColors(QList<QRgb>)), m_settingsWindow, SIGNAL(updateLedsColors(QList<QRgb>)));
         connect(m_pluginInterface, SIGNAL(updateDeviceLockStatus(DeviceLocked::DeviceLockStatus)), m_settingsWindow, SLOT(setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus)));
         connect(m_pluginInterface, SIGNAL(updateProfile(QString)),                        m_settingsWindow, SLOT(profileSwitch(QString)));
         connect(m_pluginInterface, SIGNAL(updateStatus(Backlight::Status)),               m_settingsWindow, SLOT(setBacklightStatus(Backlight::Status)));


         connect(m_pluginManager,SIGNAL(updatePlugin(QList<PyPlugin*>)),m_settingsWindow,SLOT(updatePlugin(QList<PyPlugin*>)));
         connect(m_settingsWindow,SIGNAL(getPluginConsole()),this,SLOT(getConsole()));
         connect(m_settingsWindow,SIGNAL(reloadPlugins()),m_pluginManager,SLOT(loadPlugins()));
     }
     else
     {
         connect(m_pluginInterface, SIGNAL(updateProfile(QString)),                        this, SLOT(profileSwitch(QString)));
         connect(m_pluginInterface, SIGNAL(updateStatus(Backlight::Status)),               this, SLOT(setStatusChanged(Backlight::Status)));
     }

     m_pluginManager->loadPlugins();

     m_pluginManager->moveToThread(m_PluginThread);
     m_PluginThread->start();


   // getConsole();

}
void LightpackApplication::getConsole()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    if (consolePlugin== NULL)
        consolePlugin =  m_pluginManager->getConsole(NULL);
    else
        consolePlugin->activateWindow();
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

void LightpackApplication::profileSwitch(const QString & configName)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << configName;
    Settings::loadOrCreateProfile(configName);
    this->settingsChanged();
}

void LightpackApplication::settingsChanged()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

     m_pluginInterface->changeProfile(Settings::getCurrentProfileName());

     m_grabManager->setGrabber(Settings::getGrabberType());
     m_grabManager->setSlowdownTime(Settings::getGrabSlowdown());
     m_grabManager->setMinLevelOfSensivity(Settings::getGrabMinimumLevelOfSensitivity());
      m_grabManager->setAvgColorsOnAllLeds(Settings::isGrabAvgColorsEnabled());

      int numOfLeds = Settings::getNumberOfLeds(Settings::getConnectedDevice());
      m_grabManager->setNumberOfLeds(numOfLeds);
      m_moodlampManager->setNumberOfLeds(numOfLeds);
      m_pluginInterface->setNumberOfLeds(numOfLeds);

      m_grabManager->setSendDataOnlyIfColorsChanged(Settings::isSendDataOnlyIfColorsChanges());
      m_moodlampManager->setSendDataOnlyIfColorsChanged(Settings::isSendDataOnlyIfColorsChanges());

      m_moodlampManager->setCurrentColor(Settings::getMoodLampColor());
      m_moodlampManager->setLiquidModeSpeed(Settings::getMoodLampSpeed());
      m_moodlampManager->setLiquidMode(Settings::isMoodLampLiquidMode());

      bool isBacklightEnabled = Settings::isBacklightEnabled();
      bool isCanStart =(isBacklightEnabled && m_deviceLockStatus == DeviceLocked::Unlocked);

      switch (Settings::getLightpackMode())
      {
      case Lightpack::AmbilightMode:
          m_grabManager->start(isCanStart);
          m_moodlampManager->start(false);
          break;

      case Lightpack::MoodLampMode:
          m_grabManager->start(false);
          m_moodlampManager->start(isCanStart);
          break;
      }

       //Force update colors on device for start ping device
      m_grabManager->reset();
      m_moodlampManager->reset();

}

void LightpackApplication::showLedWidgets(bool visible)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << visible;
    m_grabManager->setVisibleLedWidgets(visible);
}

void LightpackApplication::setColoredLedWidget(bool colored)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << colored;
    m_grabManager->setColoredLedWidgets(colored);
    m_grabManager->setWhiteLedWidgets(!colored);
}

void LightpackApplication::requestBacklightStatus()
{
    m_apiServer->resultBacklightStatus(m_backlightStatus);
    m_pluginInterface->resultBacklightStatus(m_backlightStatus);
}
