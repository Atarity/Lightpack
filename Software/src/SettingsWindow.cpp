/*
 * SettingsWindow.cpp
 *
 *  Created on: 26.07.2010
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


#include "SettingsWindow.hpp"
#include "ui_SettingsWindow.h"
#include "LedDeviceFactory.hpp"
#include <QDesktopWidget>
#include <QPlainTextEdit>
#include "WinAPIGrabber.hpp"
#include "WinAPIGrabberEachWidget.hpp"
#include "QtGrabber.hpp"
#include "QtGrabberEachWidget.hpp"
#include "X11Grabber.hpp"
#include "MacOSGrabber.hpp"
#include "D3D9Grabber.hpp"
#include "debug.h"

#include "../../CommonHeaders/COMMANDS.h"

using namespace SettingsScope;

// ----------------------------------------------------------------------------
// Lightpack settings window
// ----------------------------------------------------------------------------

const QString SettingsWindow::DeviceFirmvareVersionUndef = "undef";

// Indexes of supported modes listed in ui->comboBox_Modes and ui->stackedWidget_Modes
const unsigned SettingsWindow::ModeAmbilightIndex = 0;
const unsigned SettingsWindow::ModeMoodLampIndex  = 1;

SettingsWindow::SettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{   
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << this->thread()->currentThreadId();

    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);

    createActions();
    createTrayIcon();

    m_deviceFirmwareVersion = DeviceFirmvareVersionUndef;

    QRect screen = QApplication::desktop()->screenGeometry(this);

    this->setWindowFlags(Qt::Window
                          | Qt::WindowStaysOnTopHint
                          | Qt::CustomizeWindowHint
                          | Qt::WindowCloseButtonHint);
    this->setFocus(Qt::OtherFocusReason);

    // Check windows reserved simbols in profile input name
    QRegExp rx("[^<>:\"/\\|?*]+");
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    ui->comboBox_Profiles->lineEdit()->setValidator(validator);

    m_grabManager = new GrabManager(createGrabber(Settings::getGrabMode()));

    m_aboutDialog = new AboutDialog(this);

    m_speedTest = new SpeedTest();

#ifndef WINAPI_GRAB_SUPPORT
    ui->radioButton_GrabWinAPI->setVisible(false);
    ui->radioButton_GrabWinAPI_EachWidget->setVisible(false);
#endif
#ifndef D3D9_GRAB_SUPPORT
    ui->radioButton_GrabD3D9->setVisible(false);
#endif
#ifndef X11_GRAB_SUPPORT
    ui->radioButton_GrabX11->setVisible(false);
#endif
#ifndef MAC_OS_CG_GRAB_SUPPORT
    ui->radioButton_GrabMacCoreGraphics->setVisible(false);
#endif
#ifndef QT_GRAB_SUPPORT
    ui->radioButton_GrabQt->setVisible(false);
#else
    ui->radioButton_GrabQt->setChecked(true);
#endif
    profilesLoadAll();

    initLanguages();

    initVirtualLeds();

    connectSignalsSlots();

    profileLoadLast();

    loadTranslation(Settings::getLanguage());

    if (Settings::isBacklightOn())
    {
        m_backlightStatus = Backlight::StatusOn;
    } else {
        m_backlightStatus = Backlight::StatusOff;
    }

    emit backlightStatusChanged(m_backlightStatus);

    m_deviceLockStatus = Api::DeviceUnlocked;

    onGrabModeChanged();

    this->adjustSize();
    this->move(screen.width() / 2  - this->width() / 2,
                screen.height() / 2 - this->height() / 2);
    this->resize(this->minimumSize());

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "initialized";
}

void SettingsWindow::connectSignalsSlots()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SLOT(close()));

    // Connect to GrabManager
    connect(ui->spinBox_GrabSlowdown, SIGNAL(valueChanged(int)), m_grabManager, SLOT(setGrabSlowdownMs(int)));
    connect(ui->checkBox_GrabIsAvgColors, SIGNAL(toggled(bool)), m_grabManager, SLOT(setAvgColorsOnAllLeds(bool)));
    connect(ui->spinBox_GrabMinLevelOfSensitivity, SIGNAL(valueChanged(int)), m_grabManager, SLOT(setMinLevelOfSensivity(int)));
    connect(ui->groupBox_GrabShowGrabWidgets, SIGNAL(toggled(bool)), m_grabManager, SLOT(setVisibleLedWidgets(bool)));
    connect(ui->radioButton_Colored, SIGNAL(toggled(bool)), m_grabManager, SLOT(setColoredLedWidgets(bool)));
    connect(ui->radioButton_White, SIGNAL(toggled(bool)), m_grabManager, SLOT(setWhiteLedWidgets(bool)));
    connect(ui->checkBox_USB_SendDataOnlyIfColorsChanges, SIGNAL(toggled(bool)), m_grabManager, SLOT(setUpdateColorsOnlyIfChanges(bool)));
    connect(this, SIGNAL(settingsProfileChanged()), m_grabManager, SLOT(settingsProfileChanged()));

    connect(ui->radioButton_LiquidColorMoodLampMode, SIGNAL(toggled(bool)), this, SLOT(onMoodLamp_LiquidMode_Toggled(bool)));
    connect(ui->horizontalSlider_MoodLampSpeed, SIGNAL(valueChanged(int)), this, SLOT(onMoodLamp_Speed_valueChanged(int)));

    // Main options
    connect(ui->comboBox_LightpackModes, SIGNAL(activated(int)), this, SLOT(onLightpackModes_Activated(int)));
    connect(ui->comboBox_Language, SIGNAL(activated(QString)), this, SLOT(loadTranslation(QString)));
    connect(ui->pushButton_EnableDisableDevice, SIGNAL(clicked()), this, SLOT(switchBacklightOnOff()));

    // Device options
    connect(ui->spinBox_DeviceRefreshDelay, SIGNAL(valueChanged(int)), this, SLOT(onDeviceRefreshDelay_valueChanged(int)));
    connect(ui->spinBox_DeviceSmooth, SIGNAL(valueChanged(int)), this, SLOT(onDeviceSmooth_valueChanged(int)));
    connect(ui->spinBox_DeviceBrightness, SIGNAL(valueChanged(int)), this, SLOT(onDeviceBrightness_valueChanged(int)));
    connect(ui->comboBox_ConnectedDevice, SIGNAL(currentIndexChanged(QString)), this, SLOT(onDeviceConnectedDevice_currentIndexChanged(QString)));
    connect(ui->spinBox_NumberOfLeds, SIGNAL(valueChanged(int)), this, SLOT(onDeviceNumberOfLeds_valueChanged(int)));
    connect(ui->lineEdit_SerialPort, SIGNAL(editingFinished()), this, SLOT(onDeviceSerialPort_editingFinished()));
    connect(ui->comboBox_SerialPortBaudRate, SIGNAL(currentIndexChanged(QString)), this, SLOT(onDeviceSerialPortBaudRate_valueChanged(QString)));
    connect(ui->doubleSpinBox_DeviceGamma, SIGNAL(valueChanged(double)), this, SLOT(onDeviceGammaCorrection_valueChanged(double)));

    // GrabManager to this
    connect(m_grabManager, SIGNAL(ambilightTimeOfUpdatingColors(double)), this, SLOT(refreshAmbilightEvaluated(double)));

    // Open Settings file
    connect(ui->commandLinkButton_OpenSettings, SIGNAL(clicked()), this, SLOT(openCurrentProfile()));

    // Connect profile signals to this slots
    connect(ui->comboBox_Profiles->lineEdit(), SIGNAL(returnPressed()), this, SLOT(profileRename()));
    connect(ui->comboBox_Profiles, SIGNAL(currentIndexChanged(QString)), this, SLOT(profileSwitch(QString)));
    connect(ui->pushButton_ProfileNew, SIGNAL(clicked()), this, SLOT(profileNew()));
    connect(ui->pushButton_ProfileResetToDefault, SIGNAL(clicked()), this, SLOT(profileResetToDefaultCurrent()));
    connect(ui->pushButton_DeleteProfile, SIGNAL(clicked()), this, SLOT(profileDeleteCurrent()));

    connect(this, SIGNAL(settingsProfileChanged()), this, SLOT(settingsProfileChanged_UpdateUI()));
    connect(ui->pushButton_SelectColor, SIGNAL(colorChanged(QColor)), this, SLOT(onMoodLamp_ColorButton_ColorChanged(QColor)));
    connect(ui->checkBox_ExpertModeEnabled, SIGNAL(toggled(bool)), this, SLOT(onCheckBox_ExpertModeEnabled_Toggled(bool)));
    connect(ui->checkBox_SwitchOffAtClosing, SIGNAL(toggled(bool)), this, SLOT(onCheckBox_SwitchOffAtClosing_Toggled(bool)));


    // Dev tab
    connect(ui->pushButton_StartTests, SIGNAL(clicked()), this, SLOT(startTestsClick()));

    connect(ui->radioButton_GrabQt, SIGNAL(toggled(bool)), this, SLOT(onGrabModeChanged()));
    connect(ui->radioButton_GrabQt_EachWidget, SIGNAL(toggled(bool)), this, SLOT(onGrabModeChanged()));
#ifdef WINAPI_GRAB_SUPPORT
    connect(ui->radioButton_GrabWinAPI, SIGNAL(toggled(bool)), this, SLOT(onGrabModeChanged()));
    connect(ui->radioButton_GrabWinAPI_EachWidget, SIGNAL(toggled(bool)), this, SLOT(onGrabModeChanged()));
#endif
#ifdef D3D9_GRAB_SUPPORT
    connect(ui->radioButton_GrabD3D9, SIGNAL(toggled(bool)), this, SLOT(onGrabModeChanged()));
#endif
#ifdef X11_GRAB_SUPPORT
    connect(ui->radioButton_GrabX11, SIGNAL(toggled(bool)), this, SLOT(onGrabModeChanged()));
#endif
#ifdef MAC_OS_CG_GRAB_SUPPORT
    connect(ui->radioButton_GrabMacCoreGraphics, SIGNAL(toggled(bool)), this, SLOT(onGrabModeChanged()));
#endif

    // Connections to signals which will be connected to ILedDevice
    connect(m_grabManager, SIGNAL(updateLedsColors(QList<QRgb>)), this, SIGNAL(updateLedsColors(QList<QRgb>)));

    // Dev tab configure API (port, apikey)
    connect(ui->groupBox_Api, SIGNAL(toggled(bool)), this, SLOT(onGroupBox_EnableApi_Toggled(bool)));
    connect(ui->pushButton_SetApiPort, SIGNAL(clicked()), this, SLOT(onButton_SetApiPort_Clicked()));
    connect(ui->checkBox_IsApiAuthEnabled, SIGNAL(toggled(bool)), this, SLOT(onCheckBox_IsApiAuthEnabled_Toggled(bool)));
    connect(ui->pushButton_GenerateNewApiKey, SIGNAL(clicked()), this, SLOT(onButton_GenerateNewApiKey_Clicked()));

    connect(ui->spinBox_LoggingLevel, SIGNAL(valueChanged(int)), this, SLOT(onLoggingLevel_valueChanged(int)));
}

SettingsWindow::~SettingsWindow()
{    
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    delete m_switchOnBacklightAction;
    delete m_switchOffBacklightAction;
    delete m_settingsAction;
    delete m_aboutAction;
    delete m_quitAction;

    delete m_trayIcon;
    delete m_trayIconMenu;

    delete m_grabManager;

    delete ui;
}

// ----------------------------------------------------------------------------
// Events
// ----------------------------------------------------------------------------

void SettingsWindow::changeEvent(QEvent *e)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << e->type();

    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:

        ui->retranslateUi(this);
        m_switchOnBacklightAction->setText(tr("&Turn on"));
        m_switchOffBacklightAction->setText(tr("&Turn off"));
        m_settingsAction->setText(tr("&Settings"));
        m_aboutAction->setText(tr("&About"));
        m_quitAction->setText(tr("&Quit"));

        m_profilesMenu->setTitle(tr("&Profiles"));

        switch (m_backlightStatus)
        {
        case Backlight::StatusOn:
            m_trayIcon->setToolTip(tr("Enabled profile: %1").arg(ui->comboBox_Profiles->lineEdit()->text()));
            break;
        case Backlight::StatusOff:
            m_trayIcon->setToolTip(tr("Disabled"));
            break;
        case Backlight::StatusDeviceError:
            m_trayIcon->setToolTip(tr("Error with connection device, verbose in logs"));
            break;
        default:
            qWarning() << Q_FUNC_INFO << "m_backlightStatus contains crap =" << m_backlightStatus;
            break;
        }

        setWindowTitle(tr("Lightpack: %1").arg(ui->comboBox_Profiles->lineEdit()->text()));

        ui->comboBox_Language->setItemText(0, tr("System default"));

        break;
    default:
        break;
    }
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if(m_trayIcon->isVisible()){
        // Just hide settings
        hideSettings();
        event->ignore();
    }
}

void SettingsWindow::onCheckBox_SwitchOffAtClosing_Toggled(bool isEnabled)
{
    Settings::setSwitchOffAtClosing(isEnabled);
}

void SettingsWindow::onCheckBox_ExpertModeEnabled_Toggled(bool isEnabled)
{
    Settings::setExpertModeEnabled(isEnabled);
    updateExpertModeWidgetsVisibility();
}

void SettingsWindow::updateExpertModeWidgetsVisibility()
{    
    if(Settings::isExpertModeEnabled()) {
        if (ui->tabWidget->indexOf(ui->tabDevTab) < 0)
            ui->tabWidget->addTab(ui->tabDevTab, tr("Dev tab"));
    } else {
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabDevTab));
    }

    // Minimum level of sensitivity for ambilight mode
    ui->label_MinLevelOfSensitivity->setVisible(Settings::isExpertModeEnabled());
    ui->spinBox_GrabMinLevelOfSensitivity->setVisible(Settings::isExpertModeEnabled());

    // Update device tab widgets depending on the connected device
    updateDeviceTabWidgetsVisibility();
}

void SettingsWindow::updateDeviceTabWidgetsVisibility()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    SupportedDevices::DeviceType connectedDevice = Settings::getConnectedDevice();

    switch (connectedDevice)
    {
    case SupportedDevices::AdalightDevice:
        setDeviceTabWidgetsVisibility(DeviceTab::Adalight);
        setMaximumNumberOfLeds(MaximumNumberOfLeds::Adalight);
        break;

    case SupportedDevices::ArdulightDevice:
        setDeviceTabWidgetsVisibility(DeviceTab::Ardulight);
        setMaximumNumberOfLeds(MaximumNumberOfLeds::Ardulight);
        break;

    case SupportedDevices::AlienFxDevice:
        setDeviceTabWidgetsVisibility(DeviceTab::AlienFx);
        setMaximumNumberOfLeds(MaximumNumberOfLeds::AlienFx);
        break;

    case SupportedDevices::LightpackDevice:
        setDeviceTabWidgetsVisibility(DeviceTab::Lightpack);
        setMaximumNumberOfLeds(getLightpackMaximumNumberOfLeds());
        break;

    case SupportedDevices::VirtualDevice:
        setDeviceTabWidgetsVisibility(DeviceTab::Virtual);
        setMaximumNumberOfLeds(MaximumNumberOfLeds::Virtual);
        // Sync Virtual Leds count with NumberOfLeds field
        initVirtualLeds();
        break;

    default:
        qCritical() << Q_FUNC_INFO << "Fail. Unknown connectedDevice ==" << connectedDevice;
        break;
    }
}

void SettingsWindow::setDeviceTabWidgetsVisibility(DeviceTab::Options options)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << options;

    ui->groupBox_DeviceBrightness->setVisible(options & DeviceTab::Brightness);
    ui->groupBox_DeviceRefreshDelay->setVisible((options & DeviceTab::RefreshDelay) && Settings::isExpertModeEnabled());
    ui->groupBox_DeviceSmoothSlowdown->setVisible(options & DeviceTab::SmoothSlowdown);

    // NumberOfLeds
    ui->label_NumberOfLeds->setVisible(options & DeviceTab::NumberOfLeds);
    ui->spinBox_NumberOfLeds->setVisible(options & DeviceTab::NumberOfLeds);

    // SerialPort
    ui->label_SerialPort->setVisible(options & DeviceTab::SerialPort);
    ui->lineEdit_SerialPort->setVisible(options & DeviceTab::SerialPort);

    // SerialPortBaudRate
    ui->label_SerialPortBaudRate->setVisible(options & DeviceTab::SerialPort);
    ui->comboBox_SerialPortBaudRate->setVisible(options & DeviceTab::SerialPort);

    // Gamma
    ui->label_GammaCorrection->setVisible((options & DeviceTab::Gamma) && Settings::isExpertModeEnabled());
    ui->doubleSpinBox_DeviceGamma->setVisible((options & DeviceTab::Gamma) && Settings::isExpertModeEnabled());

    // Virtual leds
    ui->frame_VirtualLeds->setVisible(options & DeviceTab::VirtualLeds);   
}

void SettingsWindow::syncLedDeviceWithSettingsWindow()
{
    emit updateBrightness(Settings::getDeviceBrightness());
    emit updateSmoothSlowdown(Settings::getDeviceSmooth());
    emit updateGamma(Settings::getDeviceGamma());
}

void SettingsWindow::setMaximumNumberOfLeds(MaximumNumberOfLeds::Devices maximumNumberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << maximumNumberOfLeds;

    ui->spinBox_NumberOfLeds->setMaximum(maximumNumberOfLeds);
}

MaximumNumberOfLeds::Devices SettingsWindow::getLightpackMaximumNumberOfLeds()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if (Settings::getConnectedDevice() == SupportedDevices::LightpackDevice)
    {
        if (m_deviceFirmwareVersion == DeviceFirmvareVersionUndef)
        {
            return MaximumNumberOfLeds::Default;
        }

        bool ok = false;
        double version = m_deviceFirmwareVersion.toDouble(&ok);

        if (!ok)
        {
            qCritical() << Q_FUNC_INFO << "Convert to double fail. Device firmware version =" << m_deviceFirmwareVersion;
            return MaximumNumberOfLeds::Default;
        }

        int majorVersion = (int)version;

        if (majorVersion == 4)
            return MaximumNumberOfLeds::Lightpack4;
        else if (majorVersion == 5)
            return MaximumNumberOfLeds::Lightpack5;
        else if (majorVersion == 6)
            return MaximumNumberOfLeds::Lightpack6;
        else {
            qWarning() << Q_FUNC_INFO << "Device firmware version =" << m_deviceFirmwareVersion;
            return MaximumNumberOfLeds::Default;
        }
    }

    return MaximumNumberOfLeds::Default;
}

void SettingsWindow::onCheckBox_ConnectVirtualDevice_Toggled(bool isEnabled)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isEnabled;

    if (isEnabled){
        Settings::setConnectedDevice(SupportedDevices::VirtualDevice);
    } else {
        // TODO: think about saving last connected device to main config
        Settings::setConnectedDevice(SupportedDevices::DefaultDevice);
    }

    emit recreateLedDevice();
}

void SettingsWindow::onGroupBox_EnableApi_Toggled(bool isEnabled)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isEnabled;

    Settings::setIsApiEnabled(isEnabled);

    emit enableApiServer(isEnabled);
}

void SettingsWindow::onButton_SetApiPort_Clicked()
{
     DEBUG_LOW_LEVEL << Q_FUNC_INFO << ui->lineEdit_ApiPort->text();

     bool ok;
     int port = ui->lineEdit_ApiPort->text().toInt(&ok);

     if (ok)
     {
        Settings::setApiPort(port);
        emit updateApiPort(port);

        ui->lineEdit_ApiPort->setStyleSheet(this->styleSheet());
        ui->lineEdit_ApiPort->setToolTip("");
     } else {
         QString errorMessage = "Convert to 'int' fail";

         ui->lineEdit_ApiPort->setStyleSheet("background-color:red;");
         ui->lineEdit_ApiPort->setToolTip(errorMessage);

         qWarning() << Q_FUNC_INFO << errorMessage << "port:" << ui->lineEdit_ApiPort->text();
     }
}

void SettingsWindow::onButton_GenerateNewApiKey_Clicked()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString generatedApiKey = QUuid::createUuid().toString();

    ui->lineEdit_ApiKey->setText(generatedApiKey);

    Settings::setApiKey(generatedApiKey);
    emit updateApiKey(generatedApiKey);
}

void SettingsWindow::onCheckBox_IsApiAuthEnabled_Toggled(bool isEnabled)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isEnabled;

    Settings::setIsApiAuthEnabled(isEnabled);

    emit enableApiAuth(isEnabled);
}

void SettingsWindow::onLoggingLevel_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    // WARNING: Multithreading bug here with g_debugLevel
    g_debugLevel = value;

    Settings::setDebugLevel(value);
}

// ----------------------------------------------------------------------------
// Backlight On / Off
// ----------------------------------------------------------------------------

void SettingsWindow::setDeviceLockViaAPI(Api::DeviceLockStatus status)
{
    m_deviceLockStatus = status;

    if (m_grabManager == NULL)
        qFatal("%s m_grabManager == NULL", Q_FUNC_INFO);

    if (m_deviceLockStatus == Api::DeviceUnlocked)
    {
        connect(m_grabManager, SIGNAL(updateLedsColors(QList<QRgb>)), this, SIGNAL(updateLedsColors(QList<QRgb>)));
        syncLedDeviceWithSettingsWindow();

    } else { // m_deviceLockStatus == Api::DeviceLocked
        disconnect(m_grabManager, SIGNAL(updateLedsColors(QList<QRgb>)), this, SIGNAL(updateLedsColors(QList<QRgb>)));
    }

    startBacklight();
}

void SettingsWindow::setBacklightStatus(Backlight::Status status)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if (m_backlightStatus != Backlight::StatusDeviceError
            || status == Backlight::StatusOff)
    {
        m_backlightStatus = status;
        emit backlightStatusChanged(m_backlightStatus);
    }   

    startBacklight();
}

void SettingsWindow::backlightOn()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_backlightStatus = Backlight::StatusOn;
    emit backlightStatusChanged(m_backlightStatus);
    startBacklight();
}

void SettingsWindow::backlightOff()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_backlightStatus = Backlight::StatusOff;
    emit backlightStatusChanged(m_backlightStatus);
    startBacklight();
}

void SettingsWindow::switchBacklightOnOff()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    switch (m_backlightStatus)
    {
    case Backlight::StatusOn:
        m_backlightStatus = Backlight::StatusOff;
        break;
    case Backlight::StatusOff:
        m_backlightStatus = Backlight::StatusOn;
        break;
    case Backlight::StatusDeviceError:
        m_backlightStatus = Backlight::StatusOff;
        break;
    default:
        qWarning() << Q_FUNC_INFO << "m_backlightStatus contains crap =" << m_backlightStatus;
        break;
    }

    emit backlightStatusChanged(m_backlightStatus);

    startBacklight();
}

void SettingsWindow::startBacklight()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "m_backlightStatus =" << m_backlightStatus
                    << "m_deviceLockStatus =" << m_deviceLockStatus;

    Settings::setIsBacklightOn(m_backlightStatus == Backlight::StatusOn ||
                               m_backlightStatus == Backlight::StatusDeviceError);

    m_grabManager->updateBacklightState(m_backlightStatus, m_deviceLockStatus);

    if (m_backlightStatus == Backlight::StatusOff)
        emit offLeds();

    updateTrayAndActionStates();
}

void SettingsWindow::updateTrayAndActionStates()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    switch (m_backlightStatus)
    {
    case Backlight::StatusOn:
        ui->pushButton_EnableDisableDevice->setIcon(QIcon(":/icons/off.png"));
        ui->label_EnableDisableDevice->setText(tr("Switch off Lightpack"));
        m_switchOnBacklightAction->setEnabled(false);
        m_switchOffBacklightAction->setEnabled(true);

        if (m_deviceLockStatus == Api::DeviceLocked)
        {
            m_trayIcon->setIcon(QIcon(":/icons/lock.png"));
            m_trayIcon->setToolTip(tr("Device locked via API"));
        } else {
            m_trayIcon->setIcon(QIcon(":/icons/on.png"));
            m_trayIcon->setToolTip(tr("Enabled profile: %1").arg(ui->comboBox_Profiles->lineEdit()->text()));
        }
        break;

    case Backlight::StatusOff:
        ui->pushButton_EnableDisableDevice->setIcon(QIcon(":/icons/on.png"));
        ui->label_EnableDisableDevice->setText(tr("Switch on Lightpack"));
        m_switchOnBacklightAction->setEnabled(true);
        m_switchOffBacklightAction->setEnabled(false);
        m_trayIcon->setIcon(QIcon(":/icons/off.png"));
        m_trayIcon->setToolTip(tr("Disabled"));
        break;

    case Backlight::StatusDeviceError:
        ui->pushButton_EnableDisableDevice->setIcon(QIcon(":/icons/off.png"));
        ui->label_EnableDisableDevice->setText(tr("Switch off Lightpack"));
        m_switchOnBacklightAction->setEnabled(false);
        m_switchOffBacklightAction->setEnabled(true);
        m_trayIcon->setIcon(QIcon(":/icons/error.png"));
        m_trayIcon->setToolTip(tr("Error with connection device, verbose in logs"));
        break;
    default:
        qWarning() << Q_FUNC_INFO << "m_backlightStatus = " << m_backlightStatus;
        break;
    }
}

// ----------------------------------------------------------------------------
// Show grabbed colors in another GUI
// ----------------------------------------------------------------------------

void SettingsWindow::initVirtualLeds()
{
    int virtualLedsCount = ui->spinBox_NumberOfLeds->value();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << virtualLedsCount;

    // Remove all virtual leds from grid layout
    for (int i = 0; i < m_labelsGrabbedColors.count(); i++)
    {
        ui->gridLayout_VirtualLeds->removeWidget(m_labelsGrabbedColors[i]);
        m_labelsGrabbedColors[i]->deleteLater();
    }

    m_labelsGrabbedColors.clear();

    for (int i = 0; i < virtualLedsCount; i++)
    {
        QLabel *label = new QLabel(this);
        label->setText(QString::number(i + 1));
        label->setAlignment(Qt::AlignCenter);
        label->setAutoFillBackground(true);

        m_labelsGrabbedColors.append(label);

        int row = i / 10;
        int col = i % 10;

        ui->gridLayout_VirtualLeds->addWidget(label, row, col);
    }

    ui->frame_VirtualLeds->update();
}

void SettingsWindow::updateVirtualLedsColors(const QList<QRgb> & colors)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if (colors.count() != m_labelsGrabbedColors.count())
    {
        qCritical() << Q_FUNC_INFO << "Fail: colors.count()" << colors.count() << "!=" << "m_labelsGrabbedColors.count()" << m_labelsGrabbedColors.count() << "."
                    << "Cancel updating virtual colors.";
        return;
    }

    for(int i = 0; i < colors.count(); i++)
    {
        QLabel *label = m_labelsGrabbedColors[i];
        QColor color(colors[i]);

        QPalette pal = label->palette();
        pal.setBrush(QPalette::Window, QBrush(color));
        label->setPalette(pal);
    }
}

void SettingsWindow::requestBacklightStatus()
{
    emit resultBacklightStatus(m_backlightStatus);
}

void SettingsWindow::onApiServer_ErrorOnStartListening(QString errorMessage)
{
    ui->lineEdit_ApiPort->setStyleSheet("background-color:red;");
    ui->lineEdit_ApiPort->setToolTip(errorMessage);
}

// ----------------------------------------------------------------------------
// Show / Hide settings and about windows
// ----------------------------------------------------------------------------

void SettingsWindow::showAbout()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QRect screen = QApplication::desktop()->screenGeometry(this);

    emit requestFirmwareVersion();

    m_aboutDialog->move(screen.width() / 2 - m_aboutDialog->width() / 2,
            screen.height() / 2 - m_aboutDialog->height() / 2);

    m_aboutDialog->show();
}

void SettingsWindow::showSettings()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    Lightpack::Mode mode = Settings::getLightpackMode();
    ui->comboBox_LightpackModes->setCurrentIndex((mode == Lightpack::AmbilightMode) ? 0 : 1); // we assume that Lightpack::Mode in same order as comboBox_Modes
    m_grabManager->setVisibleLedWidgets(ui->groupBox_GrabShowGrabWidgets->isChecked() && ui->comboBox_LightpackModes->currentIndex()==0);
    this->show();
}

void SettingsWindow::hideSettings()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_grabManager->setVisibleLedWidgets(false);
    this->hide();
}

// ----------------------------------------------------------------------------
// Public slots
// ----------------------------------------------------------------------------

void SettingsWindow::ledDeviceCallSuccess(bool isSuccess)
{    
    DEBUG_MID_LEVEL << Q_FUNC_INFO << isSuccess << m_backlightStatus;
#if 0
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << this->thread()->currentThreadId();
#endif

    // If Backlight::StatusOff then nothings changed

    if (isSuccess == false)
    {
        if (m_backlightStatus == Backlight::StatusOn)
            m_backlightStatus = Backlight::StatusDeviceError;
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Backlight::StatusDeviceError";
    } else {
        if (m_backlightStatus == Backlight::StatusDeviceError)
            m_backlightStatus = Backlight::StatusOn;
    }
    updateTrayAndActionStates();
}

void SettingsWindow::ledDeviceGetFirmwareVersion(const QString & fwVersion)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << fwVersion;

    if (m_aboutDialog != NULL)
    {
        m_aboutDialog->setFirmwareVersion(fwVersion);
    }

    m_deviceFirmwareVersion = fwVersion;

    if (Settings::getConnectedDevice() == SupportedDevices::LightpackDevice)
        setMaximumNumberOfLeds(getLightpackMaximumNumberOfLeds());
}

void SettingsWindow::refreshAmbilightEvaluated(double updateResultMs)
{    
    DEBUG_MID_LEVEL << Q_FUNC_INFO << updateResultMs;

    double secs = updateResultMs / 1000;
    double hz = 0;

    if(secs != 0){
        hz = 1 / secs;
    }

    ui->label_GrabFrequency_value->setText(QString::number(hz,'f', 2) /* ms to hz */);
}

void SettingsWindow::onDeviceRefreshDelay_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setDeviceRefreshDelay(value);
    emit updateRefreshDelay(Settings::getDeviceRefreshDelay());
}

void SettingsWindow::onDeviceSmooth_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setDeviceSmooth(value);
    emit updateSmoothSlowdown(Settings::getDeviceSmooth());
}

void SettingsWindow::onDeviceBrightness_valueChanged(int percent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << percent;

    Settings::setDeviceBrightness(percent);
    emit updateBrightness(Settings::getDeviceBrightness());
}

void SettingsWindow::onDeviceConnectedDevice_currentIndexChanged(QString value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setConnectedDeviceName(value);

    // Sync connected device and widgets visibility on device tab
    updateDeviceTabWidgetsVisibility();

    // Update number of leds for current selected device
    ui->spinBox_NumberOfLeds->setValue(Settings::getNumberOfLeds(Settings::getConnectedDevice()));

    emit recreateLedDevice();
}

void SettingsWindow::onDeviceNumberOfLeds_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    m_grabManager->setNumberOfLeds(value);

    if (Settings::getConnectedDevice() == SupportedDevices::VirtualDevice)
        initVirtualLeds();

    emit updateApiDeviceNumberOfLeds(value);

    Settings::setNumberOfLeds(Settings::getConnectedDevice(), value);
}

void SettingsWindow::onDeviceSerialPort_editingFinished()
{
    QString serialPort = ui->lineEdit_SerialPort->text();
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << serialPort;

    Settings::setSerialPortName(serialPort);

    if (Settings::isConnectedDeviceUsesSerialPort())
        emit recreateLedDevice();
}

void SettingsWindow::onDeviceSerialPortBaudRate_valueChanged(QString value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setSerialPortBaudRate(value);

    if (Settings::isConnectedDeviceUsesSerialPort())
        emit recreateLedDevice();
}

void SettingsWindow::onDeviceGammaCorrection_valueChanged(double value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    Settings::setDeviceGamma(value);
    emit updateGamma(Settings::getDeviceGamma());
}

// ----------------------------------------------------------------------------

void SettingsWindow::openFile(const QString &filePath)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString filePrefix = "file://";

#ifdef Q_WS_WIN
    filePrefix = "file:///";
#endif

    QDesktopServices::openUrl(QUrl(filePrefix + filePath, QUrl::TolerantMode));
}

// ----------------------------------------------------------------------------
// Profiles
// ----------------------------------------------------------------------------

void SettingsWindow::openCurrentProfile()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    openFile(Settings::getCurrentProfilePath());
}

void SettingsWindow::profileRename()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    //
    // Press <Enter> in profile edit line will perform renaming it
    // also update settings for usable configuration LED coefs
    //
    QString configName = ui->comboBox_Profiles->currentText().trimmed();
    ui->comboBox_Profiles->setItemText(ui->comboBox_Profiles->currentIndex(), configName);

    if(configName == ""){
        return;
    }

    Settings::renameCurrentProfile(configName);

    this->setFocus(Qt::OtherFocusReason);

    updateUiFromSettings();
    emit settingsProfileChanged();
}

void SettingsWindow::profileSwitch(const QString & configName)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << configName;

    int index = ui->comboBox_Profiles->findText(configName);

    if (index < 0)
    {
        qCritical() << Q_FUNC_INFO << "Fail find text:" << configName << "in profiles combobox";
        return;
    }

    ui->comboBox_Profiles->setCurrentIndex(index);


    Settings::loadOrCreateProfile(configName);

    this->setFocus(Qt::OtherFocusReason);

    // Update settings
    updateUiFromSettings();
    emit settingsProfileChanged();
}

// Slot for switch profiles by tray menu
void SettingsWindow::profileTraySwitch()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    for(int i=0; i < m_profilesMenu->actions().count(); i++){
        QAction * action = m_profilesMenu->actions().at(i);
        if(action->isChecked()){
            if(action->text() != ui->comboBox_Profiles->currentText()){
                DEBUG_LOW_LEVEL << Q_FUNC_INFO << "switch to" << action->text();
                profileSwitchCombobox(action->text());
                return;
            }
        }else{
            if(action->text() == ui->comboBox_Profiles->currentText()){
                DEBUG_LOW_LEVEL << Q_FUNC_INFO << "set checked" << action->text();
                action->setChecked(true);
            }
        }
    }
}

void SettingsWindow::profileSwitchCombobox(QString profile)
{
    int index = ui->comboBox_Profiles->findText(profile);
    ui->comboBox_Profiles->setCurrentIndex(index);
}

void SettingsWindow::profileNew()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString profileName = tr("New profile");

    if(ui->comboBox_Profiles->findText(profileName) != -1){
        int i = 1;
        while(ui->comboBox_Profiles->findText(profileName +" "+ QString::number(i)) != -1){
            i++;
        }
        profileName += + " " + QString::number(i);
    }

    ui->comboBox_Profiles->insertItem(0, profileName);
    ui->comboBox_Profiles->setCurrentIndex(0);

    ui->comboBox_Profiles->lineEdit()->selectAll();
    ui->comboBox_Profiles->lineEdit()->setFocus(Qt::MouseFocusReason);
}

void SettingsWindow::profileResetToDefaultCurrent()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    Settings::resetDefaults();
    // Update settings
    updateUiFromSettings();
    emit settingsProfileChanged();
}

void SettingsWindow::profileDeleteCurrent()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if(ui->comboBox_Profiles->count() <= 1){
        qWarning() << "void MainWindow::profileDeleteCurrent(): profiles count ==" << ui->comboBox_Profiles->count();
        return;
    }

    // Delete settings file
    Settings::removeCurrentProfile();
    // Remove from combobox
    ui->comboBox_Profiles->removeItem(ui->comboBox_Profiles->currentIndex());
}

void SettingsWindow::profilesLoadAll()
{
    QStringList profiles = Settings::findAllProfiles();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "found profiles:" << profiles;

    for (int i = 0; i < profiles.count(); i++)
    {
        if (ui->comboBox_Profiles->findText(profiles.at(i)) == -1)
            ui->comboBox_Profiles->addItem(profiles.at(i));
    }
}

void SettingsWindow::profileLoadLast()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    ui->comboBox_Profiles->setCurrentIndex(ui->comboBox_Profiles->findText(Settings::getLastProfileName()));

    // Update settings
    updateUiFromSettings();
    emit settingsProfileChanged();
}

void SettingsWindow::settingsProfileChanged_UpdateUI()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    setWindowTitle(tr("Lightpack: %1").arg(ui->comboBox_Profiles->lineEdit()->text()));

    if (m_backlightStatus == Backlight::StatusOn)
        m_trayIcon->setToolTip(tr("Enabled profile: %1").arg(ui->comboBox_Profiles->lineEdit()->text()));

    if(ui->comboBox_Profiles->count() > 1){
        ui->pushButton_DeleteProfile->setEnabled(true);
    }else{
        ui->pushButton_DeleteProfile->setEnabled(false);
    }

    profileTraySync();
}

// Syncronize profiles from combobox with tray menu
void SettingsWindow::profileTraySync()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QAction *profileAction;

    m_profilesMenu->clear();

    for(int i=0; i < ui->comboBox_Profiles->count(); i++){
        profileAction = new QAction(ui->comboBox_Profiles->itemText(i), this);
        profileAction->setCheckable(true);
        if(i == ui->comboBox_Profiles->currentIndex()){
            profileAction->setChecked(true);
        }
        m_profilesMenu->addAction(profileAction);
        connect(profileAction, SIGNAL(triggered()), this, SLOT(profileTraySwitch()));
    }
}

// ----------------------------------------------------------------------------
// Translate GUI
// ----------------------------------------------------------------------------

void SettingsWindow::initLanguages()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    ui->comboBox_Language->clear();
    ui->comboBox_Language->addItem(tr("System default"));
    ui->comboBox_Language->addItem("English");
    ui->comboBox_Language->addItem("Russian");

    int langIndex = 0; // "System default"
    QString langSaved = Settings::getLanguage();
    if(langSaved != "<System>"){
        langIndex = ui->comboBox_Language->findText(langSaved);
    }
    ui->comboBox_Language->setCurrentIndex(langIndex);

    m_translator = NULL;
}

void SettingsWindow::loadTranslation(const QString & language)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << language;

    Settings::setLanguage(language);

    QString locale = QLocale::system().name();

    // add translation to Lightpack.pro TRANSLATIONS
    // lupdate Lightpack.pro
    // open linguist and translate application
    // lrelease Lightpack.pro
    // add new language to LightpackResources.qrc :/translations/
    // add new language to MainWindow::initLanguages() function
    // and only when all this done - append new line
    // locale - name of translation binary file form resources: %locale%.qm
    if(ui->comboBox_Language->currentIndex() == 0 /* System */){
        qDebug() << "System locale" << locale;
        Settings::setLanguage(SettingsScope::Main::LanguageDefault);
    }
    else if (language == "English") locale = "en_EN"; // :/translations/en_EN.qm
    else if (language == "Russian") locale = "ru_RU"; // :/translations/ru_RU.qm
    // append line for new language/locale here
    else {
        qWarning() << "Language" << language << "not found. Set to default" << SettingsScope::Main::LanguageDefault;
        qDebug() << "System locale" << locale;

        Settings::setLanguage(SettingsScope::Main::LanguageDefault);
    }

    QString pathToLocale = QString(":/translations/") + locale;

    if(m_translator != NULL){
        qApp->removeTranslator(m_translator);
        delete m_translator;
        m_translator = NULL;
    }

    if(locale == "en_EN" /* default no need to translate */){
        qDebug() << "Translation removed, using default locale" << locale;
        return;
    }

    m_translator = new QTranslator();
    if(m_translator->load(pathToLocale)){
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Load translation for locale" << locale;
        qApp->installTranslator(m_translator);
    }else{
        qWarning() << "Fail load translation for locale" << locale << "pathToLocale" << pathToLocale;
    }
}

void SettingsWindow::onGrabModeChanged()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "GrabMode" << getGrabMode();
    IGrabber * grabber = createGrabber(getGrabMode());
    Settings::setGrabMode(getGrabMode());
    m_grabManager->setGrabber(grabber);
}

IGrabber * SettingsWindow::createGrabber(Grab::Mode grabMode)
{
    switch (grabMode)
    {
#ifdef Q_WS_X11
    case Grab::X11GrabMode:
        return new X11Grabber();
#endif
#ifdef Q_WS_WIN
    case Grab::WinAPIGrabMode:
        return new WinAPIGrabber();

    case Grab::WinAPIEachWidgetGrabMode:
        return new WinAPIGrabberEachWidget();

    case Grab::D3D9GrabMode:
        return new D3D9Grabber();
#endif

#ifdef MAC_OS
    case Grab::MacCoreGraphicsGrabMode:
        return new MacOSGrabber();
#endif

    case Grab::QtEachWidgetGrabMode:
        return new QtGrabberEachWidget();

    default:
        return new QtGrabber();
    }
}

// ----------------------------------------------------------------------------
// Start grab speed tests
// ----------------------------------------------------------------------------

void SettingsWindow::startTestsClick()
{
    QString saveText = ui->pushButton_StartTests->text();
    ui->pushButton_StartTests->setText("Please wait...");
    ui->pushButton_StartTests->repaint(); // update right now

    // While testing this function freezes GUI
    m_speedTest->start();

    ui->pushButton_StartTests->setText(saveText);
}

// ----------------------------------------------------------------------------
// Create tray icon and actions
// ----------------------------------------------------------------------------

void SettingsWindow::createActions()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_switchOnBacklightAction = new QAction(QIcon(":/icons/on.png"), tr("&Turn on"), this);
    m_switchOnBacklightAction->setIconVisibleInMenu(true);
    connect(m_switchOnBacklightAction, SIGNAL(triggered()), this, SLOT(backlightOn()));

    m_switchOffBacklightAction = new QAction(QIcon(":/icons/off.png"), tr("&Turn off"), this);
    m_switchOffBacklightAction->setIconVisibleInMenu(true);
    connect(m_switchOffBacklightAction, SIGNAL(triggered()), this, SLOT(backlightOff()));


    m_profilesMenu = new QMenu(tr("&Profiles"), this);
    m_profilesMenu->setIcon(QIcon(":/icons/profiles.png"));
    m_profilesMenu->clear();

    m_settingsAction = new QAction(QIcon(":/icons/settings.png"), tr("&Settings"), this);
    m_settingsAction->setIconVisibleInMenu(true);
    connect(m_settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

    m_aboutAction = new QAction(QIcon(":/icons/about.png"), tr("&About"), this);
    m_aboutAction->setIconVisibleInMenu(true);
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

    m_quitAction = new QAction(tr("&Quit"), this);
    connect(m_quitAction, SIGNAL(triggered()), this, SLOT(quit()));
}

void SettingsWindow::createTrayIcon()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_trayIconMenu = new QMenu(this);
    m_trayIconMenu->addAction(m_switchOnBacklightAction);
    m_trayIconMenu->addAction(m_switchOffBacklightAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addMenu(m_profilesMenu);
    m_trayIconMenu->addAction(m_settingsAction);
    m_trayIconMenu->addAction(m_aboutAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_quitAction);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayIconMenu);
    m_trayIcon->setIcon(QIcon(":/icons/off.png"));
    m_trayIcon->show();
}

// ----------------------------------------------------------------------------
// Process icon click event
// ----------------------------------------------------------------------------

void SettingsWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    switch (reason)
    {
    case QSystemTrayIcon::DoubleClick:
        switch (m_backlightStatus)
        {
        case Backlight::StatusOff:
            backlightOn();
            break;
        case Backlight::StatusOn:
        case Backlight::StatusDeviceError:
            backlightOff();
            break;
        default:
            qWarning() << Q_FUNC_INFO << "m_backlightStatus = " << m_backlightStatus;
            break;
        }
        break;

    case QSystemTrayIcon::MiddleClick:
        if (this->isVisible())
        {
            hideSettings();
        } else {
            showSettings();
        }
        break;


#   ifdef Q_WS_WIN
    case QSystemTrayIcon::Context:
        // Hide the tray after losing focus
        //
        // In Linux (Ubuntu 10.04) this code grab keyboard input and
        // not give it back to the text editor after close application with
        // "Quit" button in the tray menu
        m_trayIconMenu->activateWindow();
        break;
#   endif


    default:
        ;
    }
}

void SettingsWindow::updateUiFromSettings()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    Lightpack::Mode mode = Settings::getLightpackMode();
    switch (mode)
    {
    case Lightpack::AmbilightMode:
        ui->comboBox_LightpackModes->setCurrentIndex(ModeAmbilightIndex);
        break;
    case Lightpack::MoodLampMode:
        ui->comboBox_LightpackModes->setCurrentIndex(ModeMoodLampIndex);
        break;
    default:
        qCritical() << "Invalid value! mode =" << mode;
        break;
    }

    ui->checkBox_ExpertModeEnabled->setChecked          (Settings::isExpertModeEnabled());

    ui->checkBox_SwitchOffAtClosing->setChecked         (Settings::isSwitchOffAtClosing());

    ui->checkBox_USB_SendDataOnlyIfColorsChanges->setChecked            (Settings::isUSB_SendDataOnlyIfColorsChanges());


    ui->checkBox_GrabIsAvgColors->setChecked            (Settings::isGrabAvgColorsOn());
    ui->spinBox_GrabSlowdown->setValue                  (Settings::getGrabSlowdown());
    ui->spinBox_GrabMinLevelOfSensitivity->setValue     (Settings::getGrabMinimumLevelOfSensitivity());

    ui->radioButton_LiquidColorMoodLampMode->setChecked (Settings::isMoodLampLiquidMode());
    ui->radioButton_ConstantColorMoodLampMode->setChecked(!Settings::isMoodLampLiquidMode());
    ui->pushButton_SelectColor->setColor                (Settings::getMoodLampColor());
    ui->horizontalSlider_MoodLampSpeed->setValue        (Settings::getMoodLampSpeed());

    ui->spinBox_NumberOfLeds->setValue                  (Settings::getNumberOfLeds(Settings::getConnectedDevice()));
    ui->horizontalSlider_DeviceRefreshDelay->setValue   (Settings::getDeviceRefreshDelay());
    ui->horizontalSlider_DeviceBrightness->setValue     (Settings::getDeviceBrightness());
    ui->horizontalSlider_DeviceSmooth->setValue         (Settings::getDeviceSmooth());
    ui->doubleSpinBox_DeviceGamma->setValue             (Settings::getDeviceGamma());
    ui->lineEdit_SerialPort->setText                    (Settings::getSerialPortName());

    initConnectedDeviceComboBox();
    initSerialPortBaudRateComboBox();

    ui->groupBox_Api->setChecked                        (Settings::isApiEnabled());
    ui->lineEdit_ApiPort->setText                       (QString::number(Settings::getApiPort()));
    ui->checkBox_IsApiAuthEnabled->setChecked           (Settings::isApiAuthEnabled());
    ui->lineEdit_ApiKey->setText                        (Settings::getApiAuthKey());
    ui->spinBox_LoggingLevel->setValue                  (g_debugLevel);

    switch (Settings::getGrabMode())
    {
#ifdef WINAPI_GRAB_SUPPORT
    case Grab::WinAPIGrabMode:
        ui->radioButton_GrabWinAPI->setChecked(true);
        break;
#endif
#ifdef D3D9_GRAB_SUPPORT
    case Grab::D3D9GrabMode:
        ui->radioButton_GrabD3D9->setChecked(true);
        break;
#endif
#ifdef X11_GRAB_SUPPORT
    case Grab::X11GrabMode:
        ui->radioButton_GrabX11->setChecked(true);
        break;
#endif
#ifdef MAC_OS_CG_GRAB_SUPPORT
    case Grab::MacCoreGraphicsGrabMode:
        ui->radioButton_GrabMacCoreGraphics->setChecked(true);
        break;
#endif
    default:
        ui->radioButton_GrabQt->setChecked(true);
    }

    onLightpackModes_Activated(ui->comboBox_LightpackModes->currentIndex());
    onMoodLamp_LiquidMode_Toggled(ui->radioButton_LiquidColorMoodLampMode->isChecked());
    updateExpertModeWidgetsVisibility();
    onGrabModeChanged();
}

Grab::Mode SettingsWindow::getGrabMode()
{
#ifdef X11_GRAB_SUPPORT
    if (ui->radioButton_GrabX11->isChecked()) {
        return Grab::X11GrabMode;
    }
#endif
#ifdef WINAPI_GRAB_SUPPORT
    if (ui->radioButton_GrabWinAPI->isChecked()) {
        return Grab::WinAPIGrabMode;
    }
    if (ui->radioButton_GrabWinAPI_EachWidget->isChecked()) {
        return Grab::WinAPIEachWidgetGrabMode;
    }
#endif
#ifdef D3D9_GRAB_SUPPORT
    if (ui->radioButton_GrabD3D9->isChecked()) {
        return Grab::D3D9GrabMode;
    }
#endif
#ifdef MAC_OS_CG_GRAB_SUPPORT
    if (ui->radioButton_GrabMacCoreGraphics->isChecked()) {
        return Grab::MacCoreGraphicsGrabMode;
    }
#endif

    if (ui->radioButton_GrabQt_EachWidget->isChecked()) {
        return Grab::QtEachWidgetGrabMode;
    }

    return Grab::QtGrabMode;
}

// ----------------------------------------------------------------------------
// Quit application
// ----------------------------------------------------------------------------

void SettingsWindow::quit()
{
    if (ui->checkBox_SwitchOffAtClosing->isChecked())
    {
        emit offLeds();
        QApplication::processEvents(QEventLoop::AllEvents, 1000);
    }

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "trayIcon->hide();";

    m_trayIcon->hide();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "QApplication::quit();";

    QApplication::quit();
}

void SettingsWindow::onLightpackModes_Activated(int index)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << index;

    switch (index)
    {
    case ModeAmbilightIndex:
        m_grabManager->switchMode(Lightpack::AmbilightMode);

        ui->stackedWidget_LightpackModes->setCurrentIndex(ModeAmbilightIndex);
        m_grabManager->setVisibleLedWidgets(ui->groupBox_GrabShowGrabWidgets->isChecked() && this->isVisible());

        Settings::setLightpackMode(Lightpack::AmbilightMode);
        break;

    case ModeMoodLampIndex:
        m_grabManager->switchMode(Lightpack::MoodLampMode);

        ui->stackedWidget_LightpackModes->setCurrentIndex(ModeMoodLampIndex);
        m_grabManager->setVisibleLedWidgets(false);

        Settings::setLightpackMode(Lightpack::MoodLampMode);
        break;
    }
}

void SettingsWindow::onMoodLamp_ColorButton_ColorChanged(QColor color)
{
    m_grabManager->setBackLightColor(color);
}

void SettingsWindow::onMoodLamp_Speed_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;
    Settings::setMoodLampSpeed(value);

    m_grabManager->setMoodLampSpeed(Settings::getMoodLampSpeed());
}

void SettingsWindow::onMoodLamp_LiquidMode_Toggled(bool checked)
{
    Settings::setMoodLampLiquidMode(checked);    
    if(!checked)
    {
        //constant mode
        ui->pushButton_SelectColor->setEnabled(true);       
        ui->horizontalSlider_MoodLampSpeed->setEnabled(false);
        ui->label_MoodLampSpeed->setEnabled(false);

        m_grabManager->setMoodLampLiquidMode(false);
    } else {
        //liquid mode
        ui->pushButton_SelectColor->setEnabled(false);
        ui->horizontalSlider_MoodLampSpeed->setEnabled(true);
        ui->label_MoodLampSpeed->setEnabled(true);

        m_grabManager->setMoodLampLiquidMode(true);
    }
}

void SettingsWindow::initConnectedDeviceComboBox()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString deviceName = Settings::getConnectedDeviceName();

    ui->comboBox_ConnectedDevice->clear();

    // NOTE: This line emit's signal currentIndex_Changed()
    ui->comboBox_ConnectedDevice->addItems(Settings::getSupportedDevices());

    int index = ui->comboBox_ConnectedDevice->findText(deviceName);

    if (index < 0)
    {
        qCritical() << Q_FUNC_INFO << "Just fail. Supported devices"
                    << Settings::getSupportedDevices() << "doesn't contains connected device:" << deviceName;
        index = 0;
    }
    ui->comboBox_ConnectedDevice->setCurrentIndex( index );
}

void SettingsWindow::initSerialPortBaudRateComboBox()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString baudrate = Settings::getSerialPortBaudRate();

    ui->comboBox_SerialPortBaudRate->clear();

    // NOTE: This line emit's signal currentIndex_Changed()
    ui->comboBox_SerialPortBaudRate->addItems(Settings::getSupportedSerialPortBaudRates());

    int index = ui->comboBox_SerialPortBaudRate->findText(baudrate);

    if (index < 0)
    {
        qCritical() << Q_FUNC_INFO << "Just another fail. Serial port supported baud rates"
                    << Settings::getSupportedSerialPortBaudRates() << "doesn't contains baud rate:" << baudrate;
        index = 0;
    }
    ui->comboBox_SerialPortBaudRate->setCurrentIndex( index );
}

