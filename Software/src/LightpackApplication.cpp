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
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QHBoxLayout>
#include "ApiServer.hpp"
#include "LightpackPluginInterface.hpp"
#include "PluginsManager.hpp"
#include "wizard/Wizard.hpp"
#include "Plugin.hpp"

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
    setApplicationName("Prismatik");
    setOrganizationName("Pixelkit LLC");
    setApplicationVersion(VERSION_STR);
    setQuitOnLastWindowClosed(false);

    m_applicationDirPath = appDirPath;
    m_noGui = false;

    processCommandLineArguments();

    printVersionsSoftwareQtOS();

    if (!Settings::Initialize(m_applicationDirPath, m_isDebugLevelObtainedFromCmdArgs)
            && !m_noGui) {
        runWizardLoop(false);
    }

    m_isSettingsWindowActive = false;

    if (!m_noGui)
    {
        checkSystemTrayAvailability();

        m_settingsWindow = new SettingsWindow();
        m_settingsWindow->setVisible(false); /* Load to tray */
        m_settingsWindow->createTrayIcon();
        m_settingsWindow->connectSignalsSlots();
        connect(this, SIGNAL(postInitialization()), m_settingsWindow, SLOT(onPostInit()));
//        m_settingsWindow->profileSwitch(Settings::getCurrentProfileName());
        // Process messages from another instances in SettingsWindow
        connect(this, SIGNAL(messageReceived(QString)), m_settingsWindow, SLOT(processMessage(QString)));
        connect(this, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget*,QWidget*)));
    }
    // Register QMetaType for Qt::QueuedConnection
    qRegisterMetaType< QList<QRgb> >("QList<QRgb>");
    qRegisterMetaType< QList<QString> >("QList<QString>");
    qRegisterMetaType<Lightpack::Mode>("Lightpack::Mode");
    qRegisterMetaType<Backlight::Status>("Backlight::Status");
    qRegisterMetaType<DeviceLocked::DeviceLockStatus>("DeviceLocked::DeviceLockStatus");
    qRegisterMetaType< QList<Plugin*> >("QList<Plugin*>");


    if (Settings::isBacklightEnabled())
    {
        m_backlightStatus = Backlight::StatusOn;
    } else {
        m_backlightStatus = Backlight::StatusOff;
    }
    m_deviceLockStatus = DeviceLocked::Unlocked;

    startLedDeviceManager();

    startApiServer();

    initGrabManager();

    if (!m_noGui)
    {
        connect(m_settingsWindow, SIGNAL(backlightStatusChanged(Backlight::Status)), this, SLOT(setStatusChanged(Backlight::Status)));
        m_settingsWindow->startBacklight();
    }

    this->settingsChanged();

//    handleConnectedDeviceChange(settings()->getConnectedDevice());

    startPluginManager();

    emit postInitialization();
}

void LightpackApplication::runWizardLoop(bool isInitFromSettings)
{
    Wizard *w = new Wizard(isInitFromSettings);
    connect(w, SIGNAL(finished(int)), this, SLOT(quitFromWizard(int)));
    w->setWindowFlags(Qt::WindowStaysOnTopHint);
    w->show();
    this->exec();
    delete w;
}

#ifdef Q_OS_WIN
HWND LightpackApplication::getMainWindowHandle() {
    // to get HWND sometimes needed to activate window
//    winFocus(m_settingsWindow, true);
    return reinterpret_cast<HWND>(m_settingsWindow->winId());
}

bool LightpackApplication::winEventFilter ( MSG * msg, long * result ) {
    Q_UNUSED(result);

    const unsigned char POWER_RESUME  = 0x01;
    const unsigned char POWER_SUSPEND = 0x02;
    static unsigned char processed = 0x00;
    if ( WM_POWERBROADCAST == msg->message ) {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "WM_POWERBROADCAST "  <<  msg->wParam << " " << processed;
        switch(msg->wParam) {
        case PBT_APMRESUMEAUTOMATIC :
            if ( !(POWER_RESUME & processed) ) {
                m_ledDeviceManager->switchOnLeds();
                processed = POWER_RESUME;
                return true;
            }
            break;
        case PBT_APMSUSPEND :
            if ( !(POWER_SUSPEND & processed) ) {
                m_ledDeviceManager->switchOffLeds();
                processed = POWER_SUSPEND;
                return true;
            }
            break;
        }
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "hwnd = " << msg->hwnd;
    }
    return false;
}
#endif

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

void LightpackApplication::setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus status, QList<QString> modules)
{
    Q_UNUSED(modules);
    m_deviceLockStatus = status;

    if (m_grabManager == NULL)
        qFatal("%s m_grabManager == NULL", Q_FUNC_INFO);
    startBacklight();
}

void LightpackApplication::startBacklight()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "m_backlightStatus =" << m_backlightStatus
                    << "m_deviceLockStatus =" << m_deviceLockStatus;

    connect(settings(), SIGNAL(moodLampColorChanged(QColor)), m_moodlampManager, SLOT(setCurrentColor(QColor)));
    connect(settings(), SIGNAL(moodLampSpeedChanged(int)),    m_moodlampManager, SLOT(setLiquidModeSpeed(int)));
    connect(settings(), SIGNAL(moodLampLiquidModeChanged(bool)),    m_moodlampManager, SLOT(setLiquidMode(bool)));
//    connect(settings(), SIGNAL(profileLoaded(const QString &)), m_moodlampManager, SLOT(settingsProfileChanged(const QString &)));

    bool isBacklightEnabled = (m_backlightStatus == Backlight::StatusOn || m_backlightStatus == Backlight::StatusDeviceError);
    bool isCanStart = (isBacklightEnabled && m_deviceLockStatus == DeviceLocked::Unlocked);

    m_pluginInterface->resultBacklightStatus(m_backlightStatus);

    Settings::setIsBacklightEnabled(isBacklightEnabled);

    const Lightpack::Mode lightpackMode = Settings::getLightpackMode();
    switch (lightpackMode)
    {
    case Lightpack::AmbilightMode:
        m_grabManager->start(isCanStart);
        m_moodlampManager->start(false);
        break;

    case Lightpack::MoodLampMode:
        m_grabManager->start(false);
        m_moodlampManager->start(isCanStart);
        break;

    default:
        qWarning() << Q_FUNC_INFO << "lightpackMode unsupported value =" << lightpackMode;
        break;
    }

    if (m_backlightStatus == Backlight::StatusOff)
        m_ledDeviceManager->switchOffLeds();
    else
        m_ledDeviceManager->switchOnLeds();


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
void LightpackApplication::onFocusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(now);
    Q_UNUSED(old);
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << (this->activeWindow() != NULL ? this->activeWindow()->metaObject()->className() : "null");
    if(!m_isSettingsWindowActive && (this->activeWindow() == m_settingsWindow)) {
        m_settingsWindow->onFocus();
        m_isSettingsWindowActive = true;
    } else {
        if(m_isSettingsWindowActive && (this->activeWindow() == NULL)) {
            m_settingsWindow->onBlur();
            m_isSettingsWindowActive = false;
        }
    }
}

void LightpackApplication::quitFromWizard(int result)
{
    Q_UNUSED(result);
    quit();
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
        else if (arguments().at(i) == "--wizard")
        {
            bool isInitFromSettings = Settings::Initialize(m_applicationDirPath, false);
            runWizardLoop(isInitFromSettings);
        }
        else if (arguments().at(i) == "--off")
        {
            LedDeviceLightpack lightpackDevice;
            lightpackDevice.switchOffLeds();
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
    fprintf(stderr, "Project  : Prismatik \n");
    fprintf(stderr, "Author   : Mike Shatohin \n");
    fprintf(stderr, "Version  : %s\n", VERSION_STR);
#ifdef GIT_REVISION
    fprintf(stderr, "Revision : %s\n", GIT_REVISION);
#endif
    fprintf(stderr, "Site     : lightpack.googlecode.com \n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Build with Qt version %s\n", QT_VERSION_STR);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options: \n");
    fprintf(stderr, "  --nogui       - no GUI (console mode) \n");
    fprintf(stderr, "  --wizard      - run settings wizard first \n");
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
#       ifdef GIT_REVISION
        qDebug() << "Prismatik:" << VERSION_STR << "rev." << GIT_REVISION;
#       else
        qDebug() << "Prismatik:" << VERSION_STR;
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

        QThread::sleep(1);
    }
#   endif

    if (QSystemTrayIcon::isSystemTrayAvailable() == false)
    {
        QMessageBox::critical(0, "Prismatik", "I couldn't detect any system tray on this system.");
        qFatal("%s %s", Q_FUNC_INFO, "I couldn't detect any system tray on this system.");
    }
}

void LightpackApplication::startApiServer()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Start API server";

    m_isApiServerConnectedToLedDeviceSignalsSlots = false;

    m_apiServer = new ApiServer();
    m_apiServer->setInterface(m_pluginInterface);
    m_apiServerThread = new QThread();

    connect(this, SIGNAL(clearColorBuffers()), m_apiServer, SIGNAL(clearColorBuffers()));

    connect(settings(), SIGNAL(apiServerEnabledChanged(bool)),  m_apiServer, SLOT(enableApiServer(bool)));
    connect(settings(), SIGNAL(apiKeyChanged(const QString &)), m_apiServer, SLOT(updateApiKey(const QString &)));
    connect(settings(), SIGNAL(apiPortChanged(int)),            m_apiServer, SLOT(updateApiPort(int)));

    connect(settings(), SIGNAL(lightpackNumberOfLedsChanged(int)), m_apiServer, SIGNAL(updateApiDeviceNumberOfLeds(int)));
    connect(settings(), SIGNAL(adalightNumberOfLedsChanged(int)),  m_apiServer, SIGNAL(updateApiDeviceNumberOfLeds(int)));
    connect(settings(), SIGNAL(ardulightNumberOfLedsChanged(int)), m_apiServer, SIGNAL(updateApiDeviceNumberOfLeds(int)));
    connect(settings(), SIGNAL(virtualNumberOfLedsChanged(int)),   m_apiServer, SIGNAL(updateApiDeviceNumberOfLeds(int)));

    if (!m_noGui)
    {
        connect(m_apiServer, SIGNAL(errorOnStartListening(QString)), m_settingsWindow, SLOT(onApiServer_ErrorOnStartListening(QString)));
    }

    connect(m_ledDeviceManager, SIGNAL(setColors_VirtualDeviceCallback(QList<QRgb>)), m_pluginInterface,    SLOT(updateColors(QList<QRgb>)), Qt::QueuedConnection);

    if (Settings::isBacklightEnabled())
    {
        connectApiServerAndLedDeviceSignalsSlots();
    }

    m_apiServer->firstStart();

    m_apiServer->moveToThread(m_apiServerThread);
    m_apiServerThread->start();
}

void LightpackApplication::startLedDeviceManager()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    m_ledDeviceManager = new LedDeviceManager();
    m_LedDeviceManagerThread = new QThread();

//    connect(settings(), SIGNAL(connectedDeviceChanged(const SupportedDevices::DeviceType)), this, SLOT(handleConnectedDeviceChange(const SupportedDevices::DeviceType)), Qt::DirectConnection);
//    connect(settings(), SIGNAL(adalightSerialPortNameChanged(QString)),               m_ledDeviceManager, SLOT(recreateLedDevice()), Qt::DirectConnection);
//    connect(settings(), SIGNAL(adalightSerialPortBaudRateChanged(QString)),           m_ledDeviceManager, SLOT(recreateLedDevice()), Qt::DirectConnection);
//    connect(m_settingsWindow, SIGNAL(updateLedsColors(const QList<QRgb> &)),          m_ledDeviceManager, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
    m_pluginInterface = new LightpackPluginInterface(NULL);

    connect(m_pluginInterface, SIGNAL(updateDeviceLockStatus(DeviceLocked::DeviceLockStatus, QList<QString>)), this, SLOT(setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus, QList<QString>)));
    connect(m_pluginInterface, SIGNAL(updateLedsColors(const QList<QRgb> &)),    m_ledDeviceManager, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
    connect(m_pluginInterface, SIGNAL(updateGamma(double)),           m_ledDeviceManager, SLOT(setGamma(double)), Qt::QueuedConnection);
    connect(m_pluginInterface, SIGNAL(updateBrightness(int)),         m_ledDeviceManager, SLOT(setBrightness(int)), Qt::QueuedConnection);
    connect(m_pluginInterface, SIGNAL(updateSmooth(int)),             m_ledDeviceManager, SLOT(setSmoothSlowdown(int)), Qt::QueuedConnection);
    connect(m_pluginInterface, SIGNAL(requestBacklightStatus()),       this, SLOT(requestBacklightStatus()));
//    connect(m_pluginInterface, SIGNAL(changeDevice(QString)),        m_settingsWindow , SLOT(setDevice(QString)));
//    connect(m_pluginInterface, SIGNAL(updateCountLeds(int)),        m_settingsWindow , SLOT(updateUiFromSettings()));
//    connect(m_pluginInterface, SIGNAL(updateCountLeds(int)),        this , SLOT(numberOfLedsChanged(int)));

    if (!m_noGui)
    {
        connect(m_pluginInterface, SIGNAL(updateLedsColors(const QList<QRgb> &)),    m_settingsWindow, SLOT(updateVirtualLedsColors(QList<QRgb>)));
        connect(m_pluginInterface, SIGNAL(updateDeviceLockStatus(DeviceLocked::DeviceLockStatus, QList<QString>)), m_settingsWindow, SLOT(setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus, QList<QString>)));
        connect(m_pluginInterface, SIGNAL(updateProfile(QString)),                        m_settingsWindow, SLOT(profileSwitch(QString)));
        connect(m_pluginInterface, SIGNAL(updateStatus(Backlight::Status)),               m_settingsWindow, SLOT(setBacklightStatus(Backlight::Status)));
    }
    else
    {
        connect(m_pluginInterface, SIGNAL(updateProfile(QString)),                        this, SLOT(profileSwitch(QString)));
        connect(m_pluginInterface, SIGNAL(updateStatus(Backlight::Status)),               this, SLOT(setStatusChanged(Backlight::Status)));
    }

    connect(settings(), SIGNAL(deviceColorDepthChanged(int)),       m_ledDeviceManager, SLOT(setColorDepth(int)), Qt::QueuedConnection);
    connect(settings(), SIGNAL(deviceSmoothChanged(int)),           m_ledDeviceManager, SLOT(setSmoothSlowdown(int)), Qt::QueuedConnection);
    connect(settings(), SIGNAL(deviceRefreshDelayChanged(int)),     m_ledDeviceManager, SLOT(setRefreshDelay(int)), Qt::QueuedConnection);
    connect(settings(), SIGNAL(deviceGammaChanged(double)),         m_ledDeviceManager, SLOT(setGamma(double)), Qt::QueuedConnection);
    connect(settings(), SIGNAL(deviceBrightnessChanged(int)),       m_ledDeviceManager, SLOT(setBrightness(int)), Qt::QueuedConnection);
//    connect(settings(), SIGNAL(deviceColorSequenceChanged(QString)),m_ledDeviceManager, SLOT(setColorSequence(QString)), Qt::QueuedConnection);
    connect(settings(), SIGNAL(luminosityThresholdChanged(int))      ,m_ledDeviceManager, SLOT(setLuminosityThreshold(int)), Qt::QueuedConnection);
    connect(settings(), SIGNAL(minimumLuminosityEnabledChanged(bool)),m_ledDeviceManager, SLOT(setMinimumLuminosityEnabled(bool)), Qt::QueuedConnection);
    connect(settings(), SIGNAL(ledCoefBlueChanged(int,double))  ,m_ledDeviceManager, SLOT(updateWBAdjustments()), Qt::QueuedConnection);
    connect(settings(), SIGNAL(ledCoefRedChanged(int,double))   ,m_ledDeviceManager, SLOT(updateWBAdjustments()), Qt::QueuedConnection);
    connect(settings(), SIGNAL(ledCoefGreenChanged(int,double)) ,m_ledDeviceManager, SLOT(updateWBAdjustments()), Qt::QueuedConnection);

//    connect(settingsObj, SIGNAL(settingsProfileChanged()),       m_ledDeviceManager, SLOT(updateDeviceSettings()), Qt::QueuedConnection);


    if (!m_noGui)
    {
        connect(m_settingsWindow, SIGNAL(requestFirmwareVersion()),       m_ledDeviceManager, SLOT(requestFirmwareVersion()), Qt::QueuedConnection);
        connect(m_settingsWindow, SIGNAL(switchOffLeds()),                m_ledDeviceManager, SLOT(switchOffLeds()), Qt::QueuedConnection);
        connect(m_settingsWindow, SIGNAL(switchOnLeds()),                 m_ledDeviceManager, SLOT(switchOnLeds()), Qt::QueuedConnection);
        connect(m_ledDeviceManager, SIGNAL(openDeviceSuccess(bool)),    m_settingsWindow, SLOT(ledDeviceOpenSuccess(bool)), Qt::QueuedConnection);
        connect(m_ledDeviceManager, SIGNAL(ioDeviceSuccess(bool)),      m_settingsWindow, SLOT(ledDeviceCallSuccess(bool)), Qt::QueuedConnection);
        connect(m_ledDeviceManager, SIGNAL(firmwareVersion(QString)),   m_settingsWindow, SLOT(ledDeviceFirmwareVersionResult(QString)), Qt::QueuedConnection);
        connect(m_ledDeviceManager, SIGNAL(setColors_VirtualDeviceCallback(QList<QRgb>)), m_settingsWindow, SLOT(updateVirtualLedsColors(QList<QRgb>)), Qt::QueuedConnection);
    }
    m_ledDeviceManager->moveToThread(m_LedDeviceManagerThread);
    m_LedDeviceManagerThread->start();
    QMetaObject::invokeMethod(m_ledDeviceManager, "init", Qt::QueuedConnection);

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "end";

}

void LightpackApplication::startPluginManager()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_pluginManager = new PluginsManager(NULL);

    connect(this, SIGNAL(destroyed()),m_pluginManager, SLOT(StopPlugins()));

    if (!m_noGui)
    {
        connect(m_settingsWindow,SIGNAL(reloadPlugins()),m_pluginManager,SLOT(reloadPlugins()));
        connect(m_pluginManager,SIGNAL(updatePlugin(QList<Plugin*>)),m_settingsWindow,SLOT(updatePlugin(QList<Plugin*>)), Qt::QueuedConnection);
        connect(m_pluginManager,SIGNAL(updatePlugin(QList<Plugin*>)),m_pluginInterface,SLOT(updatePlugin(QList<Plugin*>)), Qt::QueuedConnection);
    }

    m_pluginManager->LoadPlugins(QString(Settings::getApplicationDirPath() + "Plugins"));
    m_pluginManager->StartPlugins();

    //m_PluginThread = new QThread();
    //m_pluginManager->moveToThread(m_PluginThread);
    //m_PluginThread->start();

}


void LightpackApplication::initGrabManager()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    m_grabManager = new GrabManager(NULL);
    m_moodlampManager = new MoodLampManager(NULL);

    m_moodlampManager->initFromSettings();

    connect(settings(), SIGNAL(grabberTypeChanged(const Grab::GrabberType &)), m_grabManager, SLOT(onGrabberTypeChanged(const Grab::GrabberType &)), Qt::QueuedConnection);
    connect(settings(), SIGNAL(grabSlowdownChanged(int)), m_grabManager, SLOT(onGrabSlowdownChanged(int)), Qt::QueuedConnection);
    connect(settings(), SIGNAL(grabAvgColorsEnabledChanged(bool)), m_grabManager, SLOT(onGrabAvgColorsEnabledChanged(bool)), Qt::QueuedConnection);

    connect(settings(), SIGNAL(profileLoaded(const QString &)),        m_grabManager, SLOT(settingsProfileChanged(const QString &)), Qt::QueuedConnection);
    connect(settings(), SIGNAL(currentProfileInited(const QString &)), m_grabManager, SLOT(settingsProfileChanged(const QString &)), Qt::QueuedConnection);
//    connect(settings(), SIGNAL(profileLoaded(const QString &)),        m_moodlampManager, SLOT(settingsProfileChanged(const QString &)), Qt::QueuedConnection);
//    connect(settings(), SIGNAL(currentProfileInited(const QString &)), m_moodlampManager, SLOT(settingsProfileChanged(const QString &)), Qt::QueuedConnection);
    // Connections to signals which will be connected to ILedDevice
    if (!m_noGui)
    {
//        connect(m_settingsWindow, SIGNAL(settingsChanged()), this, SLOT(settingsChanged()));

        connect(m_settingsWindow, SIGNAL(showLedWidgets(bool)), this, SLOT(showLedWidgets(bool)));
        connect(m_settingsWindow, SIGNAL(setColoredLedWidget(bool)), this, SLOT(setColoredLedWidget(bool)));

        // GrabManager to this
        connect(m_grabManager, SIGNAL(ambilightTimeOfUpdatingColors(double)), m_settingsWindow, SLOT(refreshAmbilightEvaluated(double)));
    }

    connect(m_grabManager, SIGNAL(updateLedsColors(const QList<QRgb> &)),    m_ledDeviceManager, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
    connect(m_moodlampManager, SIGNAL(updateLedsColors(const QList<QRgb> &)),    m_ledDeviceManager, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
    connect(m_grabManager, SIGNAL(updateLedsColors(const QList<QRgb> &)), m_pluginInterface, SLOT(updateColors(const QList<QRgb> &)), Qt::QueuedConnection);
    connect(m_moodlampManager, SIGNAL(updateLedsColors(const QList<QRgb> &)), m_pluginInterface, SLOT(updateColors(const QList<QRgb> &)), Qt::QueuedConnection);
    connect(m_grabManager, SIGNAL(ambilightTimeOfUpdatingColors(double)), m_pluginInterface, SLOT(refreshAmbilightEvaluated(double)));
    connect(m_grabManager,SIGNAL(changeScreen(QRect)),m_pluginInterface,SLOT(refreshScreenRect(QRect)));

}

void LightpackApplication::connectApiServerAndLedDeviceSignalsSlots()
{
    if (m_isApiServerConnectedToLedDeviceSignalsSlots == false)
    {
        m_isApiServerConnectedToLedDeviceSignalsSlots = true;
    }
}

void LightpackApplication::disconnectApiServerAndLedDeviceSignalsSlots()
{
    if (m_isApiServerConnectedToLedDeviceSignalsSlots == true)
    {
        m_isApiServerConnectedToLedDeviceSignalsSlots = false;
    }
}

void LightpackApplication::commitData(QSessionManager &sessionManager)
{
    Q_UNUSED(sessionManager);

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Off leds before quit";

    if (m_ledDeviceManager != NULL)
    {
        // Disable signals with new colors
        disconnect(m_settingsWindow, SIGNAL(updateLedsColors(QList<QRgb>)),  m_ledDeviceManager, SLOT(setColors(QList<QRgb>)));
        disconnect(m_apiServer, SIGNAL(updateLedsColors(QList<QRgb>)),  m_ledDeviceManager, SLOT(setColors(QList<QRgb>)));

        // Process all currently pending signals
        QApplication::processEvents(QEventLoop::AllEvents, 1000);

        // Send signal and process it
        m_ledDeviceManager->switchOffLeds();
        QApplication::processEvents(QEventLoop::AllEvents, 1000);
    }
}

void LightpackApplication::profileSwitch(const QString & configName)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << configName;
    Settings::loadOrCreateProfile(configName);
}

void LightpackApplication::settingsChanged()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_pluginInterface->changeProfile(Settings::getCurrentProfileName());

    m_grabManager->onSendDataOnlyIfColorsEnabledChanged(Settings::isSendDataOnlyIfColorsChanges());

    m_moodlampManager->setSendDataOnlyIfColorsChanged(Settings::isSendDataOnlyIfColorsChanges());

    m_moodlampManager->setCurrentColor(Settings::getMoodLampColor());
    m_moodlampManager->setLiquidModeSpeed(Settings::getMoodLampSpeed());
    m_moodlampManager->setLiquidMode(Settings::isMoodLampLiquidMode());

    bool isBacklightEnabled = Settings::isBacklightEnabled();
    bool isCanStart =(isBacklightEnabled && m_deviceLockStatus == DeviceLocked::Unlocked);

//    numberOfLedsChanged(Settings::getNumberOfLeds(Settings::getConnectedDevice()));

    switch (Settings::getLightpackMode())
    {
    case Lightpack::AmbilightMode:
        m_grabManager->start(isCanStart);
        m_moodlampManager->start(false);
        break;

    default:
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
    //m_apiServer->resultBacklightStatus(m_backlightStatus);
    m_pluginInterface->resultBacklightStatus(m_backlightStatus);
}

void LightpackApplication::free()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_moodlampManager->start(false);
    m_grabManager->start(false);
    m_pluginManager->StopPlugins();

    QApplication::processEvents(QEventLoop::AllEvents, 1000);

    delete m_pluginManager;
    delete m_moodlampManager;
    delete m_grabManager;
    delete m_ledDeviceManager;

    m_pluginManager = NULL;
    m_moodlampManager = NULL;
    m_grabManager = NULL;
    m_ledDeviceManager = NULL;

    QApplication::processEvents(QEventLoop::AllEvents, 1000);
}
