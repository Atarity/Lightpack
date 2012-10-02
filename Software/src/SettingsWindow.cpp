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

#include <QtAlgorithms>

#include "SettingsWindow.hpp"
#include "ui_SettingsWindow.h"

#include "Settings.hpp"
#include "SpeedTest.hpp"
#include "ColorButton.hpp"
#include "LedDeviceManager.hpp"
#include "enums.hpp"
#include "debug.h"

#include "plugins/PyPlugin.h"

#include "hotkeys/qkeysequencewidget/src/qkeysequencewidget.h"
#include "hotkeys/globalshortcut/globalshortcutmanager.h"

using namespace SettingsScope;

// ----------------------------------------------------------------------------
// Lightpack settings window
// ----------------------------------------------------------------------------

const QString SettingsWindow::DeviceFirmvareVersionUndef = "undef";
const QString SettingsWindow::LightpackDownloadsPageUrl = "http://code.google.com/p/lightpack/downloads/list";

// Indexes of supported modes listed in ui->comboBox_Modes and ui->stackedWidget_Modes
const unsigned SettingsWindow::AmbilightModeIndex = 0;
const unsigned SettingsWindow::MoodLampModeIndex  = 1;

SettingsWindow::SettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow),
    m_deviceFirmwareVersion(DeviceFirmvareVersionUndef)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << this->thread()->currentThreadId();

    m_trayIcon = NULL;

    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);

    createActions();

    setWindowFlags(Qt::Window |
                   Qt::WindowStaysOnTopHint |
                   Qt::CustomizeWindowHint |
                   Qt::WindowCloseButtonHint | Qt::WindowContextHelpButtonHint);
    setFocus(Qt::OtherFocusReason);

    // Check windows reserved simbols in profile input name
    QRegExpValidator *validatorProfileName = new QRegExpValidator(QRegExp("[^<>:\"/\\|?*]*"), this);
    ui->comboBox_Profiles->lineEdit()->setValidator(validatorProfileName);

    QRegExpValidator *validatorApiKey = new QRegExpValidator(QRegExp("[a-zA-Z0-9{}_-]*"), this);
    ui->lineEdit_ApiKey->setValidator(validatorApiKey);

    m_speedTest = new SpeedTest();

//    ui->listWidget->setViewMode(QListView::IconMode);
//    ui->listWidget->setIconSize(QSize(64, 64));
//    ui->listWidget->setMovement(QListView::Static);
//    ui->listWidget->setMaximumWidth(110);
//    ui->listWidget->setMinimumWidth(110);
//    ui->listWidget->setSpacing(12);
//    ui->listWidget->setCurrentRow(0);
//    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
// hide main tabbar
    QTabBar* tabBar=qFindChild<QTabBar*>(ui->tabWidget);
    tabBar->hide();

// hide device options tabbar
    tabBar=qFindChild<QTabBar*>(ui->tabDevices);
    tabBar->hide();

    connect(ui->listWidget,
            SIGNAL(currentRowChanged(int)),
            this, SLOT(changePage(int)));

    labelProfile = new QLabel(statusBar());
    labelProfile->setStyleSheet("margin-bottom: 5px");
    labelDevice = new QLabel(statusBar());
    labelDevice->setStyleSheet("margin-bottom: 5px");
    labelFPS  = new QLabel(statusBar());
    labelFPS->setStyleSheet("margin-bottom: 5px");

    statusBar()->setSizeGripEnabled(false);
    statusBar()->addWidget(labelProfile, 1);
    statusBar()->addWidget(labelDevice, 1);
    statusBar()->addWidget(labelFPS, 1);

    // Request firmware version of the device to show message about update the firmware
    QTimer::singleShot(1000, this, SIGNAL(requestFirmwareVersion()));

    profilesLoadAll();

    initGrabbersRadioButtonsVisibility();
    initLanguages();
    initVirtualLeds(Settings::getNumberOfLeds(SupportedDevices::DeviceTypeVirtual));
    initConnectedDeviceComboBox();
    initSerialPortBaudRateComboBox();

    setupHotkeys();

    profileLoadLast();

    loadTranslation(Settings::getLanguage());

    if (Settings::isBacklightEnabled())
    {
        m_backlightStatus = Backlight::StatusOn;
    } else {
        m_backlightStatus = Backlight::StatusOff;
    }

    emit backlightStatusChanged(m_backlightStatus);

    m_deviceLockStatus = DeviceLocked::Unlocked;
    m_lightpackMode = Settings::getLightpackMode();

    onGrabberChanged();


    adjustSizeAndMoveCenter();   

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "initialized";
}

void SettingsWindow::changePage(int page)
{
      DEBUG_LOW_LEVEL << Q_FUNC_INFO << page;
    if (page > ui->tabWidget->count()-1)
        ui->tabWidget->setCurrentIndex(page-1);
    else
        ui->tabWidget->setCurrentIndex(page);
}

SettingsWindow::~SettingsWindow()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    delete m_switchOnBacklightAction;
    delete m_switchOffBacklightAction;
    delete m_settingsAction;
    delete m_quitAction;

    delete m_trayIcon;
    delete m_trayIconMenu;


    delete ui;
}

void SettingsWindow::connectSignalsSlots()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if (m_trayIcon!=NULL)
    {
        connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onTrayIcon_Activated(QSystemTrayIcon::ActivationReason)));
        connect(m_trayIcon, SIGNAL(messageClicked()), this, SLOT(onTrayIcon_MessageClicked()));
    }
    DEBUG_MID_LEVEL << Q_FUNC_INFO << "tr";

    connect(ui->spinBox_GrabSlowdown, SIGNAL(valueChanged(int)), this, SLOT(onGrabSlowdown_valueChanged(int)));
    connect(ui->spinBox_GrabMinLevelOfSensitivity, SIGNAL(valueChanged(int)), this, SLOT(onGrabMinLevelOfSensivity_valueChanged(int)));
    connect(ui->checkBox_GrabIsAvgColors, SIGNAL(toggled(bool)), this, SLOT(onGrabIsAvgColors_toggled(bool)));

    connect(ui->groupBox_GrabShowGrabWidgets, SIGNAL(toggled(bool)), this, SLOT( onShowLedWidgets_Toggled(bool)));
    connect(ui->radioButton_Colored, SIGNAL(toggled(bool)), this, SLOT(onSetColoredLedWidgets()));
    connect(ui->radioButton_White, SIGNAL(toggled(bool)), this, SLOT(onSetWhiteLedWidgets()));

    connect(ui->radioButton_LiquidColorMoodLampMode, SIGNAL(toggled(bool)), this, SLOT(onMoodLampLiquidMode_Toggled(bool)));
    connect(ui->horizontalSlider_MoodLampSpeed, SIGNAL(valueChanged(int)), this, SLOT(onMoodLampSpeed_valueChanged(int)));

    // Main options
    connect(ui->comboBox_LightpackModes, SIGNAL(activated(int)), this, SLOT(onLightpackModes_Activated(int)));
    connect(ui->comboBox_Language, SIGNAL(activated(QString)), this, SLOT(loadTranslation(QString)));
    connect(ui->pushButton_EnableDisableDevice, SIGNAL(clicked()), this, SLOT(switchBacklightOnOff()));

    // Device options
    connect(ui->spinBox_DeviceRefreshDelay, SIGNAL(valueChanged(int)), this, SLOT(onDeviceRefreshDelay_valueChanged(int)));
    connect(ui->spinBox_DeviceSmooth, SIGNAL(valueChanged(int)), this, SLOT(onDeviceSmooth_valueChanged(int)));
    connect(ui->spinBox_DeviceBrightness, SIGNAL(valueChanged(int)), this, SLOT(onDeviceBrightness_valueChanged(int)));
    connect(ui->spinBox_DeviceColorDepth, SIGNAL(valueChanged(int)), this, SLOT(onDeviceColorDepth_valueChanged(int)));
    connect(ui->comboBox_ConnectedDevice, SIGNAL(currentIndexChanged(QString)), this, SLOT(onDeviceConnectedDevice_currentIndexChanged(QString)));
    connect(ui->lineEdit_AdalightSerialPort, SIGNAL(editingFinished()), this, SLOT(onDeviceSerialPort_editingFinished()));
    connect(ui->comboBox_AdalightSerialPortBaudRate, SIGNAL(currentIndexChanged(QString)), this, SLOT(onDeviceSerialPortBaudRate_valueChanged(QString)));
    connect(ui->lineEdit_ArdulightSerialPort, SIGNAL(editingFinished()), this, SLOT(onDeviceSerialPort_editingFinished()));
    connect(ui->comboBox_ArdulightSerialPortBaudRate, SIGNAL(currentIndexChanged(QString)), this, SLOT(onDeviceSerialPortBaudRate_valueChanged(QString)));
    connect(ui->doubleSpinBox_DeviceGamma, SIGNAL(valueChanged(double)), this, SLOT(onDeviceGammaCorrection_valueChanged(double)));
    connect(ui->horizontalSlider_GammaCorrection, SIGNAL(valueChanged(int)), this, SLOT(onSliderDeviceGammaCorrection_valueChanged(int)));
    connect(ui->checkBox_SendDataOnlyIfColorsChanges, SIGNAL(toggled(bool)), this, SLOT(onDeviceSendDataOnlyIfColorsChanged_toggled(bool)));
    connect(ui->comboBox_AdalightColorSequence, SIGNAL(currentIndexChanged(QString)), this, SLOT(onColorSequence_valueChanged(QString)));
    connect(ui->comboBox_ArdulightColorSequence, SIGNAL(currentIndexChanged(QString)), this, SLOT(onColorSequence_valueChanged(QString)));
    connect(ui->spinBox_LightpackNumberOfLeds, SIGNAL(valueChanged(int)), this, SLOT(onLightpackNumberOfLeds_valueChanged(int)));
    connect(ui->spinBox_AdalightNumberOfLeds, SIGNAL(valueChanged(int)), this, SLOT(onAdalightNumberOfLeds_valueChanged(int)));
    connect(ui->spinBox_ArdulightNumberOfLeds, SIGNAL(valueChanged(int)), this, SLOT(onArdulightNumberOfLeds_valueChanged(int)));
    connect(ui->spinBox_VirtualNumberOfLeds, SIGNAL(valueChanged(int)), this, SLOT(onVirtualNumberOfLeds_valueChanged(int)));

    // Open Settings file
    connect(ui->commandLinkButton_OpenSettings, SIGNAL(clicked()), this, SLOT(openCurrentProfile()));

    // Connect profile signals to this slots    
    connect(ui->comboBox_Profiles->lineEdit(), SIGNAL(editingFinished()) /* or returnPressed() */, this, SLOT(profileRename()));
    connect(ui->comboBox_Profiles, SIGNAL(currentIndexChanged(QString)), this, SLOT(profileSwitch(QString)));
//    connect(Settings::settingsSingleton(), SIGNAL(profileLoaded(const QString &)), this, SLOT(profileSwitch(QString)));
    connect(ui->pushButton_ProfileNew, SIGNAL(clicked()), this, SLOT(profileNew()));
    connect(ui->pushButton_ProfileResetToDefault, SIGNAL(clicked()), this, SLOT(profileResetToDefaultCurrent()));
    connect(ui->pushButton_DeleteProfile, SIGNAL(clicked()), this, SLOT(profileDeleteCurrent()));

//    connect(Settings::settingsSingleton(), SIGNAL(profileLoaded(const QString &)), this, SLOT(settingsProfileChanged_UpdateUI(const QString &)));
    connect(ui->pushButton_SelectColor, SIGNAL(colorChanged(QColor)), this, SLOT(onMoodLampColor_changed(QColor)));
    connect(ui->checkBox_ExpertModeEnabled, SIGNAL(toggled(bool)), this, SLOT(onExpertModeEnabled_Toggled(bool)));
    connect(ui->checkBox_SwitchOffAtClosing, SIGNAL(toggled(bool)), this, SLOT(onSwitchOffAtClosing_Toggled(bool)));    

    // Dev tab
    connect(ui->pushButton_StartTests, SIGNAL(clicked()), this, SLOT(startTestsClick()));

    connect(ui->checkBox_EnableDx1011Capture, SIGNAL(toggled(bool)), this, SLOT(onGrabberChanged()));
    connect(ui->radioButton_GrabQt, SIGNAL(toggled(bool)), this, SLOT(onGrabberChanged()));
    connect(ui->radioButton_GrabQt_EachWidget, SIGNAL(toggled(bool)), this, SLOT(onGrabberChanged()));
#ifdef WINAPI_GRAB_SUPPORT
    connect(ui->radioButton_GrabWinAPI, SIGNAL(toggled(bool)), this, SLOT(onGrabberChanged()));
    connect(ui->radioButton_GrabWinAPI_EachWidget, SIGNAL(toggled(bool)), this, SLOT(onGrabberChanged()));
#endif
#ifdef D3D9_GRAB_SUPPORT
    connect(ui->radioButton_GrabD3D9, SIGNAL(toggled(bool)), this, SLOT(onGrabberChanged()));
#endif
#ifdef X11_GRAB_SUPPORT
    connect(ui->radioButton_GrabX11, SIGNAL(toggled(bool)), this, SLOT(onGrabberChanged()));
#endif
#ifdef MAC_OS_CG_GRAB_SUPPORT
    connect(ui->radioButton_GrabMacCoreGraphics, SIGNAL(toggled(bool)), this, SLOT(onGrabberChanged()));
#endif
#ifdef D3D10_GRAB_SUPPORT
    connect(ui->checkBox_EnableDx1011Capture, SIGNAL(toggled(bool)), this, SLOT(onDx1011CaptureEnabledChanged(bool)));
#endif


    // Dev tab configure API (port, apikey)
    connect(ui->groupBox_Api, SIGNAL(toggled(bool)), this, SLOT(onEnableApi_Toggled(bool)));
    connect(ui->lineEdit_ApiPort, SIGNAL(editingFinished()), this, SLOT(onSetApiPort_Clicked()));
    //connect(ui->checkBox_IsApiAuthEnabled, SIGNAL(toggled(bool)), this, SLOT(onIsApiAuthEnabled_Toggled(bool)));
    connect(ui->pushButton_GenerateNewApiKey, SIGNAL(clicked()), this, SLOT(onGenerateNewApiKey_Clicked()));
    connect(ui->lineEdit_ApiKey, SIGNAL(editingFinished()), this, SLOT(onApiKey_EditingFinished()));

    connect(ui->spinBox_LoggingLevel, SIGNAL(valueChanged(int)), this, SLOT(onLoggingLevel_valueChanged(int)));
    connect(ui->checkBox_PingDeviceEverySecond, SIGNAL(toggled(bool)), this, SLOT(onPingDeviceEverySecond_Toggled(bool)));

    // HotKeys
    connect(m_keySequenceWidget, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(setOnOffHotKey(QKeySequence)));
    connect(m_keySequenceWidget, SIGNAL(keySequenceCleared()), this, SLOT(clearOnOffHotKey()));

    //Plugins
    connect(ui->list_Plugins,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(on_list_Plugins_clicked(QListWidgetItem*)));
    connect(ui->pushButton_ConsolePlugin,SIGNAL(clicked()),this,SLOT(viewPluginConsole()));
    connect(ui->pushButton_UpPriority, SIGNAL(clicked()), this, SLOT(MoveUpPlugin()));
    connect(ui->pushButton_DownPriority, SIGNAL(clicked()), this, SLOT(MoveDownPlugin()));
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
        m_quitAction->setText(tr("&Quit"));

        m_profilesMenu->setTitle(tr("&Profiles"));

        m_keySequenceWidget->setNoneText(tr("Undefined key"));
        m_keySequenceWidget->setShortcutName(tr("On/Off light:"));

        if (m_trayIcon!=NULL)
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

        updateTrayAndActionStates();

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

void SettingsWindow::onSwitchOffAtClosing_Toggled(bool isEnabled)
{
    Settings::setSwitchOffAtClosing(isEnabled);
}

void SettingsWindow::onExpertModeEnabled_Toggled(bool isEnabled)
{
    Settings::setExpertModeEnabled(isEnabled);
    updateExpertModeWidgetsVisibility();
}

void SettingsWindow::updateExpertModeWidgetsVisibility()
{    
    if(Settings::isExpertModeEnabled()) {
        if (ui->tabWidget->indexOf(ui->tabExperimental) < 0)
            ui->tabWidget->insertTab(4,ui->tabExperimental, tr("Dev tab"));
            ui->listWidget->setItemHidden(ui->listWidget->item(4),false);
    } else {
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabExperimental));
        ui->listWidget->setItemHidden(ui->listWidget->item(4),true);
    }

    // Minimum level of sensitivity for ambilight mode
    ui->label_MinLevelOfSensitivity->setVisible(Settings::isExpertModeEnabled());
    ui->spinBox_GrabMinLevelOfSensitivity->setVisible(Settings::isExpertModeEnabled());
    ui->groupBox_HotKeys->setVisible(Settings::isExpertModeEnabled());

    ui->pushButton_ConsolePlugin->setVisible(Settings::isExpertModeEnabled());

    // Update device tab widgets depending on the connected device
    updateDeviceTabWidgetsVisibility();
}

void SettingsWindow::updateDeviceTabWidgetsVisibility()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    SupportedDevices::DeviceType connectedDevice = Settings::getConnectedDevice();

    switch (connectedDevice)
    {
    case SupportedDevices::DeviceTypeAdalight:
        ui->groupBox_DeviceSpecificOptions->show();
        ui->tabDevices->setCurrentWidget(ui->tabDeviceAdalight);
        setDeviceTabWidgetsVisibility(DeviceTab::Adalight);
        break;

    case SupportedDevices::DeviceTypeArdulight:
        ui->groupBox_DeviceSpecificOptions->show();
        ui->tabDevices->setCurrentWidget(ui->tabDeviceArdulight);
        setDeviceTabWidgetsVisibility(DeviceTab::Ardulight);
        break;

    case SupportedDevices::DeviceTypeAlienFx:
        ui->groupBox_DeviceSpecificOptions->hide();
        setDeviceTabWidgetsVisibility(DeviceTab::AlienFx);
        break;

    case SupportedDevices::DeviceTypeLightpack:
        ui->groupBox_DeviceSpecificOptions->show();
        ui->tabDevices->setCurrentWidget(ui->tabDeviceLightpack);
        setDeviceTabWidgetsVisibility(DeviceTab::Lightpack);
        break;

    case SupportedDevices::DeviceTypeVirtual:
        ui->groupBox_DeviceSpecificOptions->show();
        ui->tabDevices->setCurrentWidget(ui->tabDeviceVirtual);
        setDeviceTabWidgetsVisibility(DeviceTab::Virtual);
        // Sync Virtual Leds count with NumberOfLeds field
        initVirtualLeds(Settings::getNumberOfLeds(SupportedDevices::DeviceTypeVirtual));
        break;

    default:
        qCritical() << Q_FUNC_INFO << "Fail. Unknown connectedDevice ==" << connectedDevice;
        break;
    }
}

void SettingsWindow::setDeviceTabWidgetsVisibility(DeviceTab::Options options)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << options;

//    ui->groupBox_DeviceBrightness->setVisible(options & DeviceTab::Brightness);
//    ui->groupBox_DeviceSmoothSlowdown->setVisible(options & DeviceTab::SmoothSlowdown);
//    ui->groupBox_DeviceRefreshDelay->setVisible((options & DeviceTab::RefreshDelay) && Settings::isExpertModeEnabled());

    int majorVersion = getLigtpackFirmwareVersionMajor();
    if (majorVersion == 4 || majorVersion == 5)
    {
        // Show color depth only if lightpack hw4.x or hw5.x
        ui->label_7->setVisible((options & DeviceTab::ColorDepth) && Settings::isExpertModeEnabled());
        ui->horizontalSlider_DeviceColorDepth->setVisible((options & DeviceTab::ColorDepth) && Settings::isExpertModeEnabled());
        ui->spinBox_DeviceColorDepth->setVisible((options & DeviceTab::ColorDepth) && Settings::isExpertModeEnabled());
    } else {
        ui->label_7->setVisible(false);
        ui->horizontalSlider_DeviceColorDepth->setVisible(false);
        ui->spinBox_DeviceColorDepth->setVisible(false);
    }

    // NumberOfLeds
//    ui->label_NumberOfLeds->setVisible(options & DeviceTab::NumberOfLeds);
//    ui->spinBox_NumberOfLeds->setVisible(options & DeviceTab::NumberOfLeds);

    // SerialPort
//    ui->label_SerialPort->setVisible(options & DeviceTab::SerialPort);
//    ui->lineEdit_SerialPort->setVisible(options & DeviceTab::SerialPort);

    // SerialPortBaudRate
//    ui->label_SerialPortBaudRate->setVisible(options & DeviceTab::SerialPort);
//    ui->comboBox_SerialPortBaudRate->setVisible(options & DeviceTab::SerialPort);

    // Gamma
//    ui->label_GammaCorrection->setVisible((options & DeviceTab::Gamma) && Settings::isExpertModeEnabled());
//    ui->doubleSpinBox_DeviceGamma->setVisible((options & DeviceTab::Gamma) && Settings::isExpertModeEnabled());

    // Virtual leds
//    ui->frame_VirtualLeds->setVisible(options & DeviceTab::VirtualLeds);

     // ColorSequence
//    ui->label_ColorSequence->setVisible(options & DeviceTab::ColorSequence);
//    ui->comboBox_ColorSequence->setVisible(options & DeviceTab::ColorSequence);
}

void SettingsWindow::syncLedDeviceWithSettingsWindow()
{
    emit updateBrightness(Settings::getDeviceBrightness());
    emit updateSmoothSlowdown(Settings::getDeviceSmooth());
    emit updateGamma(Settings::getDeviceGamma());
}

void SettingsWindow::setMaximumNumberOfLeds(MaximumNumberOfLeds::Devices maximumNumberOfLeds)
{
//    DEBUG_LOW_LEVEL << Q_FUNC_INFO << maximumNumberOfLeds;

//    ui->spinBox_NumberOfLeds->setMaximum(maximumNumberOfLeds);
}

int SettingsWindow::getLigtpackFirmwareVersionMajor()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if (Settings::getConnectedDevice() != SupportedDevices::DeviceTypeLightpack)
        return -1;

    if (m_deviceFirmwareVersion == DeviceFirmvareVersionUndef)
        return -1;

    bool ok = false;
    double version = m_deviceFirmwareVersion.toDouble(&ok);

    if (!ok)
    {
        DEBUG_MID_LEVEL << Q_FUNC_INFO << "Convert to double fail. Device firmware version =" << m_deviceFirmwareVersion;
        return -1;
    }

    int majorVersion = (int)version;

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Lightpack major version:" << majorVersion;

    return majorVersion;
}

void SettingsWindow::onPostInit() {
    updateUiFromSettings();
}

void SettingsWindow::onEnableApi_Toggled(bool isEnabled)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isEnabled;

    Settings::setIsApiEnabled(isEnabled);

}

void SettingsWindow::onApiKey_EditingFinished()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString apikey = ui->lineEdit_ApiKey->text();

    Settings::setApiKey(apikey);
}

void SettingsWindow::onSetApiPort_Clicked()
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

void SettingsWindow::onGenerateNewApiKey_Clicked()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString generatedApiKey = QUuid::createUuid().toString();

    ui->lineEdit_ApiKey->setText(generatedApiKey);

    Settings::setApiKey(generatedApiKey);
}

void SettingsWindow::onLoggingLevel_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    // WARNING: Multithreading bug here with g_debugLevel
    g_debugLevel = value;

    Settings::setDebugLevel(value);
}

void SettingsWindow::setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus status,  QList<QString> modules)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << status;
    m_deviceLockStatus = status;
    m_deviceLockKey = modules;


    if (m_deviceLockStatus == DeviceLocked::Unlocked)
    {
        syncLedDeviceWithSettingsWindow();

        if (m_lightpackMode == Lightpack::MoodLampMode && ui->radioButton_LiquidColorMoodLampMode->isChecked())
        {
            // Switch off smooth if moodlamp liquid mode
            emit updateSmoothSlowdown(0);
        }
    } else {
        if (m_lightpackMode == Lightpack::MoodLampMode && ui->radioButton_LiquidColorMoodLampMode->isChecked())
        {
            // Restore smooth slowdown value before change control to API
            emit updateSmoothSlowdown(Settings::getDeviceSmooth());
        }
    }

    startBacklight();
}

void SettingsWindow::onDx1011CaptureEnabledChanged(bool isEnabled) {
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isEnabled;

    #ifdef D3D10_GRAB_SUPPORT
    Settings::setDx1011GrabberEnabled(isEnabled);
    #endif
}
void SettingsWindow::setModeChanged(Lightpack::Mode mode)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << mode;
    updateUiFromSettings();
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

    if(ui->list_Plugins->count()>0)
    {
            int count = ui->list_Plugins->count();
            for(int index = 0; index < count; index++)
            {
                DEBUG_LOW_LEVEL << Q_FUNC_INFO << "check session key";
                QListWidgetItem * item = ui->list_Plugins->item(index);
                int indexPlugin =item->data(Qt::UserRole).toUInt();
                QString key = _plugins[indexPlugin]->getSessionKey();
                if (m_deviceLockKey.contains(key))
                {
                    if (m_deviceLockStatus != DeviceLocked::Api  && m_deviceLockKey.indexOf(key)==0)
                         m_deviceLockModule = _plugins[indexPlugin]->getName();
                     if (Settings::isExpertModeEnabled())
                         item->setText(_plugins[indexPlugin]->getName()+" (Lock)");
                }
                else
                    item->setText(_plugins[indexPlugin]->getName());
            }
    }

    if (m_deviceLockKey.count()==0)
        m_deviceLockModule = "";

    updateTrayAndActionStates();
}

void SettingsWindow::updateTrayAndActionStates()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if (m_trayIcon== NULL) return;

    switch (m_backlightStatus)
    {
    case Backlight::StatusOn:
        ui->pushButton_EnableDisableDevice->setIcon(QIcon(":/icons/off.png"));
        ui->pushButton_EnableDisableDevice->setText("  " + tr("Turn lights OFF"));
        m_switchOnBacklightAction->setEnabled(false);
        m_switchOffBacklightAction->setEnabled(true);

        if (m_deviceLockStatus == DeviceLocked::Api)
        {
            m_trayIcon->setIcon(QIcon(":/icons/lock.png"));
            m_trayIcon->setToolTip(tr("Device locked via API"));
        } else
        if (m_deviceLockStatus == DeviceLocked::Plugin)
        {
            m_trayIcon->setIcon(QIcon(":/icons/lock.png"));
            m_trayIcon->setToolTip(tr("Device locked via Plugin")+" ("+m_deviceLockModule+")");
        } else {
            m_trayIcon->setIcon(QIcon(":/icons/on.png"));
            m_trayIcon->setToolTip(tr("Enabled profile: %1").arg(ui->comboBox_Profiles->lineEdit()->text()));
        }
        break;

    case Backlight::StatusOff:
        ui->pushButton_EnableDisableDevice->setIcon(QIcon(":/icons/on.png"));
        ui->pushButton_EnableDisableDevice->setText("  " + tr("Turn lights ON"));
        m_switchOnBacklightAction->setEnabled(true);
        m_switchOffBacklightAction->setEnabled(false);
        m_trayIcon->setIcon(QIcon(":/icons/off.png"));
        m_trayIcon->setToolTip(tr("Disabled"));
        break;

    case Backlight::StatusDeviceError:
        ui->pushButton_EnableDisableDevice->setIcon(QIcon(":/icons/off.png"));
        ui->pushButton_EnableDisableDevice->setText("  " + tr("Turn lights OFF"));
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

void SettingsWindow::initGrabbersRadioButtonsVisibility()
{
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
}

// ----------------------------------------------------------------------------
// Show grabbed colors in another GUI
// ----------------------------------------------------------------------------

void SettingsWindow::initVirtualLeds(int virtualLedsCount)
{
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

        if (m_backlightStatus == Backlight::StatusOff)
        {
            // If status off fill labels black:
            QPalette pal = label->palette();
            pal.setBrush(QPalette::Window, QBrush(Qt::black));
            label->setPalette(pal);
        }

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

    if (Settings::getConnectedDevice() != SupportedDevices::DeviceTypeVirtual)
        return;

    if (colors.count() != m_labelsGrabbedColors.count())
    {
        qCritical() << Q_FUNC_INFO << "Fail: colors.count()" << colors.count() << "!=" << "m_labelsGrabbedColors.count()" << m_labelsGrabbedColors.count() << "."
                    << "Cancel updating virtual colors.";
        return;
    }

    for (int i = 0; i < colors.count(); i++)
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

void SettingsWindow::onPingDeviceEverySecond_Toggled(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    Settings::setPingDeviceEverySecond(state);

    // Force update colors on device for start ping device
//    m_grabManager->reset();
//    m_moodlampManager->reset();
}

void SettingsWindow::processMessage(const QString &message)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << message;

    if (m_trayIcon==NULL) return;
    m_trayMessage = Tray_AnotherInstanceMessage;
    m_trayIcon->showMessage(tr("Lightpack"), tr("Application already running"));
}

// ----------------------------------------------------------------------------
// Show / Hide settings and about windows
// ----------------------------------------------------------------------------

void SettingsWindow::showAbout()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QRect screen = QApplication::desktop()->screenGeometry(this);

    emit requestFirmwareVersion();

    ui->tabWidget->setCurrentWidget(ui->tabAbout);
    this->show();

}

void SettingsWindow::showSettings()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    Lightpack::Mode mode = Settings::getLightpackMode();
    ui->comboBox_LightpackModes->setCurrentIndex((mode == Lightpack::AmbilightMode) ? 0 : 1); // we assume that Lightpack::Mode in same order as comboBox_Modes
    emit showLedWidgets(ui->groupBox_GrabShowGrabWidgets->isChecked() && ui->comboBox_LightpackModes->currentIndex()==0);
    this->show();
}

void SettingsWindow::hideSettings()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    emit showLedWidgets(false);

    this->hide();
}

// ----------------------------------------------------------------------------
// Public slots
// ----------------------------------------------------------------------------

void SettingsWindow::ledDeviceOpenSuccess(bool isSuccess)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isSuccess;

    if (isSuccess)
    {
        // Device just connected and for updating colors
        // we should reset previous saved states
        //m_grabManager->reset();
       // m_moodlampManager->reset();
    }

    ledDeviceCallSuccess(isSuccess);
}

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

void SettingsWindow::ledDeviceFirmwareVersionResult(const QString & fwVersion)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << fwVersion;

    m_deviceFirmwareVersion = fwVersion;

    QString aboutDialogFirmwareString = m_deviceFirmwareVersion;

    if (Settings::getConnectedDevice() == SupportedDevices::DeviceTypeLightpack)
    {
        if (m_deviceFirmwareVersion == "5.0" || m_deviceFirmwareVersion == "4.3")
        {
            aboutDialogFirmwareString += QString(" ") +
                    "(<a href=\"" + LightpackDownloadsPageUrl + "\">" +
                    tr("update firmware") +
                    "</a>)";

            if (Settings::isUpdateFirmwareMessageShown() == false)
            {
                m_trayMessage = Tray_UpdateFirmwareMessage;
                if (m_trayIcon!=NULL)
                    m_trayIcon->showMessage(tr("Lightpack firmware update"), tr("Click on this message to open lightpack downloads page"));
                Settings::setUpdateFirmwareMessageShown(true);
            }
        }
    }

   this->setFirmwareVersion(aboutDialogFirmwareString);

    updateDeviceTabWidgetsVisibility();
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

    this->labelFPS->setText(tr("FPS: ")+QString::number(hz,'f', 2) );
}

void SettingsWindow::onGrabberChanged()
{
    Grab::GrabberType grabberType = getSelectedGrabberType();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "GrabberType: " << grabberType << ", isDx1011CaptureEnabled: " << isDx1011CaptureEnabled();

    Settings::setGrabberType(grabberType);
}

void SettingsWindow::onGrabSlowdown_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setGrabSlowdown(value);
}

void SettingsWindow::onGrabMinLevelOfSensivity_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setThresholdOfBlack(value);
}

void SettingsWindow::onGrabIsAvgColors_toggled(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    Settings::setGrabAvgColorsEnabled(state);
}

void SettingsWindow::onDeviceRefreshDelay_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setDeviceRefreshDelay(value);
}

void SettingsWindow::onDeviceSmooth_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setDeviceSmooth(value);
}

void SettingsWindow::onDeviceBrightness_valueChanged(int percent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << percent;

    Settings::setDeviceBrightness(percent);
}

void SettingsWindow::onDeviceColorDepth_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setDeviceColorDepth(value);
}

void SettingsWindow::onDeviceConnectedDevice_currentIndexChanged(QString value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    int currentIndex = ui->comboBox_ConnectedDevice->currentIndex();
    int virtualDeviceIndex = ui->comboBox_ConnectedDevice->count() - 1;

    if (currentIndex == virtualDeviceIndex)
    {
        value = "Virtual";
    }

    Settings::setConnectedDeviceName(value);

    // Sync connected device and widgets visibility on device tab
    updateDeviceTabWidgetsVisibility();

    // Update number of leds for current selected device
//    ui->spinBox_NumberOfLeds->setValue(Settings::getNumberOfLeds(Settings::getConnectedDevice()));

    int index = ui->comboBox_AdalightColorSequence->findText(Settings::getColorSequence(Settings::getConnectedDevice()));
    if (index < 0)
        index = 0;
    ui->comboBox_AdalightColorSequence->setCurrentIndex(index);

    this->labelDevice->setText(tr("Device:")+value);
    emit recreateLedDevice();
}


void SettingsWindow::onLightpackNumberOfLeds_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setNumberOfLeds(SupportedDevices::DeviceTypeLightpack, value);

}

void SettingsWindow::onAdalightNumberOfLeds_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setNumberOfLeds(SupportedDevices::DeviceTypeAdalight, value);

}

void SettingsWindow::onArdulightNumberOfLeds_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setNumberOfLeds(SupportedDevices::DeviceTypeArdulight, value);

}

void SettingsWindow::onVirtualNumberOfLeds_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setNumberOfLeds(SupportedDevices::DeviceTypeVirtual, value);
    initVirtualLeds(value);
}

void SettingsWindow::onDeviceSerialPort_editingFinished()
{
    QString serialPort = ui->lineEdit_AdalightSerialPort->text();
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << serialPort;

    Settings::setAdalightSerialPortName(serialPort);

    if (Settings::isConnectedDeviceUsesSerialPort())
        emit recreateLedDevice();
}

void SettingsWindow::onDeviceSerialPortBaudRate_valueChanged(QString value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setAdalightSerialPortBaudRate(value);

    if (Settings::isConnectedDeviceUsesSerialPort())
        emit recreateLedDevice();
}

void SettingsWindow::onColorSequence_valueChanged(QString value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setColorSequence(Settings::getConnectedDevice(),value);
}

void SettingsWindow::onDeviceGammaCorrection_valueChanged(double value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    Settings::setDeviceGamma(value);
    ui->horizontalSlider_GammaCorrection->setValue(floor((value * 100 + 0.5)));
    emit updateGamma(Settings::getDeviceGamma());
}

void SettingsWindow::onSliderDeviceGammaCorrection_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;
    Settings::setDeviceGamma(static_cast<double>(value + 0.4) / 100);
    ui->doubleSpinBox_DeviceGamma->setValue(Settings::getDeviceGamma());
    emit updateGamma(Settings::getDeviceGamma());
}


void SettingsWindow::onDeviceSendDataOnlyIfColorsChanged_toggled(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    Settings::setSendDataOnlyIfColorsChanges(state);
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

    QString configName = ui->comboBox_Profiles->currentText().trimmed();
    ui->comboBox_Profiles->lineEdit()->setText(configName);

    // Signal editingFinished() will be emited if focus wasn't lost (for example when return pressed),
    // and profileRename() function will be called again here
    this->setFocus(Qt::OtherFocusReason);

    if (Settings::getCurrentProfileName() == configName)
    {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Nothing has changed";
        return;
    }

    if (configName == "")
    {
        configName = Settings::getCurrentProfileName();
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Profile name is empty, return back to" << configName;
    }
    else
    {
        Settings::renameCurrentProfile(configName);
    }

    ui->comboBox_Profiles->lineEdit()->setText(configName);
    ui->comboBox_Profiles->setItemText(ui->comboBox_Profiles->currentIndex(), configName);
}

void SettingsWindow::profileSwitch(const QString & configName)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << configName;

    profilesLoadAll();

    int index = ui->comboBox_Profiles->findText(configName);

    if (index < 0)
    {
        qCritical() << Q_FUNC_INFO << "Fail find text:" << configName << "in profiles combobox";
        return;
    }

    ui->comboBox_Profiles->setCurrentIndex(index);


    Settings::loadOrCreateProfile(configName);

    this->setFocus(Qt::OtherFocusReason);

    this->labelProfile->setText(tr("Profile: %1").arg(configName));
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

    // By default backlight is enabled, but make it same as current backlight status for usability purposes
    Settings::setIsBacklightEnabled(m_backlightStatus != Backlight::StatusOff);

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

    profileTraySync();

    // Update settings
    updateUiFromSettings();
    emit settingsProfileChanged();
}

void SettingsWindow::settingsProfileChanged_UpdateUI(const QString &profileName)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    setWindowTitle(tr("Lightpack: %1").arg(profileName));

    if (m_backlightStatus == Backlight::StatusOn && m_trayIcon!=NULL)
        m_trayIcon->setToolTip(tr("Enabled profile: %1").arg(profileName));

    if(ui->comboBox_Profiles->count() > 1){
        ui->pushButton_DeleteProfile->setEnabled(true);
    }else{
        ui->pushButton_DeleteProfile->setEnabled(false);
    }

    updateUiFromSettings();
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
        DEBUG_LOW_LEVEL << "System locale" << locale;
        Settings::setLanguage(SettingsScope::Main::LanguageDefault);
    }
    else if (language == "English") locale = "en_EN"; // :/translations/en_EN.qm
    else if (language == "Russian") locale = "ru_RU"; // :/translations/ru_RU.qm
    // append line for new language/locale here
    else {
        qWarning() << "Language" << language << "not found. Set to default" << SettingsScope::Main::LanguageDefault;
        DEBUG_LOW_LEVEL << "System locale" << locale;

        Settings::setLanguage(SettingsScope::Main::LanguageDefault);
    }

    QString pathToLocale = QString(":/translations/") + locale;

    if(m_translator != NULL){
        qApp->removeTranslator(m_translator);
        delete m_translator;
        m_translator = NULL;
    }

    if(locale == "en_EN" /* default no need to translate */){
        DEBUG_LOW_LEVEL << "Translation removed, using default locale" << locale;
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

void SettingsWindow::onTrayIcon_Activated(QSystemTrayIcon::ActivationReason reason)
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

void SettingsWindow::onTrayIcon_MessageClicked()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << m_trayMessage;

    switch(m_trayMessage)
    {
    case Tray_UpdateFirmwareMessage:
        if (Settings::getConnectedDevice() == SupportedDevices::DeviceTypeLightpack)
        {
            // Open lightpack downloads page
            QDesktopServices::openUrl(QUrl(LightpackDownloadsPageUrl, QUrl::TolerantMode));
        }
        break;

    case Tray_AnotherInstanceMessage:
    default:
        break;
    }
}

void SettingsWindow::updateUiFromSettings()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    this->labelProfile->setText(tr("Profile: %1").arg(Settings::getCurrentProfileName()));
    this->labelDevice->setText(tr("Device: %1").arg(Settings::getConnectedDeviceName()));

    Lightpack::Mode mode = Settings::getLightpackMode();
    switch (mode)
    {
    case Lightpack::AmbilightMode:
        ui->comboBox_LightpackModes->setCurrentIndex(AmbilightModeIndex);
        break;
    case Lightpack::MoodLampMode:
        ui->comboBox_LightpackModes->setCurrentIndex(MoodLampModeIndex);
        break;
    default:
        qCritical() << "Invalid value! mode =" << mode;
        break;
    }

    int index = ui->comboBox_AdalightColorSequence->findText(Settings::getColorSequence(Settings::getConnectedDevice()));
    if (index < 0)
        index = 0;
    ui->comboBox_AdalightColorSequence->setCurrentIndex(index);

    ui->checkBox_ExpertModeEnabled->setChecked          (Settings::isExpertModeEnabled());

    ui->checkBox_SendDataOnlyIfColorsChanges->setChecked(Settings::isSendDataOnlyIfColorsChanges());
    ui->checkBox_SwitchOffAtClosing->setChecked         (Settings::isSwitchOffAtClosing());
    ui->checkBox_PingDeviceEverySecond->setChecked      (Settings::isPingDeviceEverySecond());

    ui->checkBox_GrabIsAvgColors->setChecked            (Settings::isGrabAvgColorsEnabled());
    ui->spinBox_GrabSlowdown->setValue                  (Settings::getGrabSlowdown());
    ui->spinBox_GrabMinLevelOfSensitivity->setValue     (Settings::getThresholdOfBlack());

    // Check the selected moodlamp mode (setChecked(false) not working to select another)
    ui->radioButton_ConstantColorMoodLampMode->setChecked(!Settings::isMoodLampLiquidMode());
    ui->radioButton_LiquidColorMoodLampMode->setChecked (Settings::isMoodLampLiquidMode());    
    ui->pushButton_SelectColor->setColor                (Settings::getMoodLampColor());
    ui->horizontalSlider_MoodLampSpeed->setValue        (Settings::getMoodLampSpeed());

    ui->spinBox_LightpackNumberOfLeds->setValue         (Settings::getNumberOfLeds(SupportedDevices::DeviceTypeLightpack));
    ui->spinBox_AdalightNumberOfLeds->setValue          (Settings::getNumberOfLeds(SupportedDevices::DeviceTypeAdalight));
    ui->spinBox_ArdulightNumberOfLeds->setValue         (Settings::getNumberOfLeds(SupportedDevices::DeviceTypeArdulight));
    ui->spinBox_VirtualNumberOfLeds->setValue           (Settings::getNumberOfLeds(SupportedDevices::DeviceTypeVirtual));
    ui->horizontalSlider_DeviceRefreshDelay->setValue   (Settings::getDeviceRefreshDelay());
    ui->horizontalSlider_DeviceBrightness->setValue     (Settings::getDeviceBrightness());
    ui->horizontalSlider_DeviceSmooth->setValue         (Settings::getDeviceSmooth());
    ui->horizontalSlider_DeviceColorDepth->setValue     (Settings::getDeviceColorDepth());
    ui->doubleSpinBox_DeviceGamma->setValue             (Settings::getDeviceGamma());
    ui->lineEdit_AdalightSerialPort->setText            (Settings::getAdalightSerialPortName());

    ui->groupBox_Api->setChecked                        (Settings::isApiEnabled());
    ui->lineEdit_ApiPort->setText                       (QString::number(Settings::getApiPort()));
    ui->lineEdit_ApiKey->setText                        (Settings::getApiAuthKey());
    ui->spinBox_LoggingLevel->setValue                  (g_debugLevel);

    #ifdef D3D10_GRAB_SUPPORT
    ui->checkBox_EnableDx1011Capture->setChecked        (Settings::isDx1011GrabberEnabled());
    #endif

    switch (Settings::getGrabberType())
    {
#ifdef WINAPI_GRAB_SUPPORT
    case Grab::GrabberTypeWinAPI:
        ui->radioButton_GrabWinAPI->setChecked(true);
        break;
    case Grab::GrabberTypeWinAPIEachWidget:
        ui->radioButton_GrabWinAPI_EachWidget->setChecked(true);
        break;
#endif
#ifdef D3D9_GRAB_SUPPORT
    case Grab::GrabberTypeD3D9:
        ui->radioButton_GrabD3D9->setChecked(true);
        break;
#endif
#ifdef X11_GRAB_SUPPORT
    case Grab::GrabberTypeX11:
        ui->radioButton_GrabX11->setChecked(true);
        break;
#endif
#ifdef MAC_OS_CG_GRAB_SUPPORT
    case Grab::MacCoreGraphicsGrabber:
        ui->radioButton_GrabMacCoreGraphics->setChecked(true);
        break;
#endif
    case Grab::GrabberTypeQtEachWidget:
        ui->radioButton_GrabQt_EachWidget->setChecked(true);
        break;

    default:
        ui->radioButton_GrabQt->setChecked(true);
    }

    onLightpackModes_Activated(ui->comboBox_LightpackModes->currentIndex());
    onMoodLampLiquidMode_Toggled(ui->radioButton_LiquidColorMoodLampMode->isChecked());
    updateExpertModeWidgetsVisibility();
    onGrabberChanged();
}

Grab::GrabberType SettingsWindow::getSelectedGrabberType()
{
#ifdef X11_GRAB_SUPPORT
    if (ui->radioButton_GrabX11->isChecked()) {
        return Grab::GrabberTypeX11;
    }
#endif
#ifdef WINAPI_GRAB_SUPPORT
    if (ui->radioButton_GrabWinAPI->isChecked()) {
        return Grab::GrabberTypeWinAPI;
    }
    if (ui->radioButton_GrabWinAPI_EachWidget->isChecked()) {
        return Grab::GrabberTypeWinAPIEachWidget;
    }
#endif
#ifdef D3D9_GRAB_SUPPORT
    if (ui->radioButton_GrabD3D9->isChecked()) {
        return Grab::GrabberTypeD3D9;
    }
#endif
#ifdef MAC_OS_CG_GRAB_SUPPORT
    if (ui->radioButton_GrabMacCoreGraphics->isChecked()) {
        return Grab::MacCoreGraphicsGrabber;
    }
#endif

    if (ui->radioButton_GrabQt_EachWidget->isChecked()) {
        return Grab::GrabberTypeQtEachWidget;
    }

    return Grab::GrabberTypeQt;
}

bool SettingsWindow::isDx1011CaptureEnabled() {
    return ui->checkBox_EnableDx1011Capture->isChecked();
}

// ----------------------------------------------------------------------------
// Hotkeys slots and functions.
// ----------------------------------------------------------------------------

void SettingsWindow::setOnOffHotKey(QKeySequence keySequence)
{
    GlobalShortcutManager::instance()->clear();
    GlobalShortcutManager::instance()->connect(keySequence, this, SLOT(switchBacklightOnOff()));
    Settings::setOnOffDeviceKey(keySequence);
}

void SettingsWindow::clearOnOffHotKey()
{
    GlobalShortcutManager::instance()->clear();
    Settings::setOnOffDeviceKey(QKeySequence());
}

void SettingsWindow::setupHotkeys()
{
    m_keySequenceWidget = new QKeySequenceWidget(tr("Undefined key"), tr("On/Off light:"), this);
    ui->groupBox_HotKeys->layout()->addWidget(m_keySequenceWidget);

    m_keySequenceWidget->setKeySequence(Settings::getOnOffDeviceKey());
    GlobalShortcutManager::instance()->connect(Settings::getOnOffDeviceKey(), this, SLOT(switchBacklightOnOff()));
}

// ----------------------------------------------------------------------------
// Quit application
// ----------------------------------------------------------------------------

void SettingsWindow::quit()
{
    if (ui->checkBox_SwitchOffAtClosing->isChecked())
    {
        // Process all currently pending signals (which may include updating the color signals)
        QApplication::processEvents(QEventLoop::AllEvents, 1000);

        emit switchOffLeds();
        QApplication::processEvents(QEventLoop::AllEvents, 1000);
    }

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "trayIcon->hide();";

    if (m_trayIcon!=NULL)
    m_trayIcon->hide();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "QApplication::quit();";

    QApplication::quit();
}

void SettingsWindow::onLightpackModes_Activated(int index)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << index;

    switch (index)
    {
    case AmbilightModeIndex:
        Settings::setLightpackMode(Lightpack::AmbilightMode);
        ui->stackedWidget_LightpackModes->setCurrentIndex(AmbilightModeIndex);
        emit showLedWidgets(ui->groupBox_GrabShowGrabWidgets->isChecked() && this->isVisible());
        if (ui->radioButton_LiquidColorMoodLampMode->isChecked())
        {
            // Restore smooth slowdown value
            emit updateSmoothSlowdown(Settings::getDeviceSmooth());
        }
        break;

    case MoodLampModeIndex:
        Settings::setLightpackMode(Lightpack::MoodLampMode);
        ui->stackedWidget_LightpackModes->setCurrentIndex(MoodLampModeIndex);
        emit showLedWidgets(false);
        if (ui->radioButton_LiquidColorMoodLampMode->isChecked())
        {
            // Switch off smooth if moodlamp liquid mode
            emit updateSmoothSlowdown(0);
        }
        break;
    }
    backlightStatusChanged(m_backlightStatus);
    m_lightpackMode = Settings::getLightpackMode();
}

void SettingsWindow::onMoodLampColor_changed(QColor color)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << color;
    Settings::setMoodLampColor(color);
}

void SettingsWindow::onMoodLampSpeed_valueChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;
    Settings::setMoodLampSpeed(value);
}

void SettingsWindow::onMoodLampLiquidMode_Toggled(bool checked)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << checked;

    Settings::setMoodLampLiquidMode(checked);    
    if (Settings::isMoodLampLiquidMode())
    {
        // Liquid color mode
        ui->pushButton_SelectColor->setEnabled(false);
        ui->horizontalSlider_MoodLampSpeed->setEnabled(true);
        ui->label_MoodLampSpeed->setEnabled(true);
        // Switch off smooth if liquid mode enabled
        // this helps normal work liquid mode on hw5 and hw4 lightpacks
        emit updateSmoothSlowdown(0);
    } else {
        // Constant color mode
        ui->pushButton_SelectColor->setEnabled(true);
        ui->horizontalSlider_MoodLampSpeed->setEnabled(false);
        ui->label_MoodLampSpeed->setEnabled(false);
        emit updateSmoothSlowdown(Settings::getDeviceSmooth());
    }
}

 void SettingsWindow::onShowLedWidgets_Toggled(bool checked)
 {
     DEBUG_LOW_LEVEL << Q_FUNC_INFO << checked;
     emit showLedWidgets(checked);
 }

 void SettingsWindow::onSetColoredLedWidgets()
 {
     DEBUG_LOW_LEVEL << Q_FUNC_INFO;
     emit setColoredLedWidget(true);
 }

 void SettingsWindow::onSetWhiteLedWidgets()
{
      DEBUG_LOW_LEVEL << Q_FUNC_INFO;
     emit setColoredLedWidget(false);
}

void SettingsWindow::initConnectedDeviceComboBox()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString deviceName = Settings::getConnectedDeviceName();
    QStringList devices = Settings::getSupportedDevices();

    // Remove virtual device because it already in combobox
    devices.removeOne("Virtual");
    devices.append("Virtual (debug)");

    // NOTE: This line emit's signal currentIndex_Changed()
    ui->comboBox_ConnectedDevice->insertItems(0, devices);

    int currentIndex = 0;

    if (deviceName == "Virtual")
    {
        // Virtual device is last in combobox
        currentIndex = ui->comboBox_ConnectedDevice->count() - 1;
    } else {
        currentIndex = ui->comboBox_ConnectedDevice->findText(deviceName);
    }

    if (currentIndex < 0)
    {
        qCritical() << Q_FUNC_INFO << "Just fail. Supported devices"
                    << Settings::getSupportedDevices() << "doesn't contains connected device:" << deviceName;
        currentIndex = 0;
    }
    ui->comboBox_ConnectedDevice->setCurrentIndex(currentIndex);

    int index = ui->comboBox_AdalightColorSequence->findText(Settings::getColorSequence(Settings::getConnectedDevice()));
    if (index < 0)
        index = 0;
    ui->comboBox_AdalightColorSequence->setCurrentIndex(index);

}

void SettingsWindow::initSerialPortBaudRateComboBox()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString baudrate = Settings::getAdalightSerialPortBaudRate();

    ui->comboBox_AdalightSerialPortBaudRate->clear();

    // NOTE: This line emit's signal currentIndex_Changed()
    ui->comboBox_AdalightSerialPortBaudRate->addItems(Settings::getSupportedSerialPortBaudRates());

    int index = ui->comboBox_AdalightSerialPortBaudRate->findText(baudrate);

    if (index < 0)
    {
        qCritical() << Q_FUNC_INFO << "Just another fail. Serial port supported baud rates"
                    << Settings::getSupportedSerialPortBaudRates() << "doesn't contains baud rate:" << baudrate;
        index = 0;
    }
    ui->comboBox_AdalightSerialPortBaudRate->setCurrentIndex(index);
}

void SettingsWindow::adjustSizeAndMoveCenter()
{
    QRect screen = QApplication::desktop()->screenGeometry(this);

    adjustSize();
    move(screen.width()  / 2 - width()  / 2,
         screen.height() / 2 - height() / 2);
    resize(minimumSize());
}

QWidget* SettingsWindow::getSettingBox()
{
    return ui->tabSettingsPlugin;
}

bool SettingsWindow::toPriority(PyPlugin* s1 ,PyPlugin* s2 )
{
    return s1->getPriority() > s2->getPriority();
}

void SettingsWindow::updatePlugin(QList<PyPlugin*> plugins)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;


    _plugins = plugins;
    // sort priority
    qSort(_plugins.begin() , _plugins.end(), SettingsWindow::toPriority );
    ui->list_Plugins->clear();
    foreach(PyPlugin* plugin, _plugins){
        int index = _plugins.indexOf(plugin);
        QListWidgetItem *item = new QListWidgetItem(plugin->getName());
        item->setData(Qt::UserRole, index);
        item->setIcon(plugin->getIcon());
        if (plugin->isEnabled())
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        ui->list_Plugins->addItem(item);
        }
    if (_plugins.count()>0)
    {
        ui->list_Plugins->setCurrentRow(0);
        pluginSwitch(0);
    }
    ui->pushButton->setEnabled(true);


    startBacklight();

}

void SettingsWindow::on_list_Plugins_clicked(QListWidgetItem *current)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

        bool isEnabled = true;

        if (current->checkState() == Qt::Checked)
           isEnabled = true;
        else
           isEnabled = false;

        int index =current->data(Qt::UserRole).toUInt();
        if (_plugins[index]->isEnabled() != isEnabled)
            _plugins[index]->setEnabled(isEnabled);

        pluginSwitch(index);
}

void SettingsWindow::pluginSwitch(int index)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << index;

    qDeleteAll( ui->tabSettingsPlugin->findChildren<QWidget*>() );
    delete  ui->tabSettingsPlugin->layout();
    if (_plugins.count()-1 < index) return;
    if (_plugins[index]->isEnabled())
    {
        if (ui->tabPlugin->indexOf(ui->tabSettingsPlugin) < 0)
            ui->tabPlugin->insertTab(0,ui->tabSettingsPlugin, tr("Plugin settings"));
        //ui->tabPlugin->setCurrentIndex(0);
        _plugins[index]->getSettings();
    }
    else
    {
        ui->tabPlugin->removeTab(ui->tabPlugin->indexOf(ui->tabSettingsPlugin));
    }
    ui->label_PluginName->setText(_plugins[index]->getName());
    ui->label_PluginAuthor->setText(_plugins[index]->getAuthor());
    ui->label_PluginVersion->setText(_plugins[index]->getVersion());
    ui->tb_PluginDescription->setText(_plugins[index]->getDescription());
    ui->label_PluginIcon->setPixmap(_plugins[index]->getIcon().pixmap(50,50));
}

void SettingsWindow::viewPluginConsole()
{
    emit getPluginConsole();
}

void SettingsWindow::on_pushButton_clicked()
{
     foreach(PyPlugin* plugin, _plugins){
         plugin->stop();
     }
    qDeleteAll( ui->tabSettingsPlugin->findChildren<QWidget*>() );
    delete  ui->tabSettingsPlugin->layout();
    ui->list_Plugins->clear();
    _plugins.clear();
    ui->pushButton->setEnabled(false);
    emit reloadPlugins();
}

void SettingsWindow::MoveUpPlugin() {
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
   int k= ui->list_Plugins->currentRow();
   if (k==0) return;
   int n = k-1;
   QListWidgetItem* pItem = ui->list_Plugins->takeItem(k);
   ui->list_Plugins->insertItem(n, pItem);
   ui->list_Plugins->setCurrentRow(n);
   savePriorityPlugin();

}
void SettingsWindow::MoveDownPlugin() {
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    int k= ui->list_Plugins->currentRow();
    if (k==ui->list_Plugins->count()-1) return;
    int n = k+1;
    QListWidgetItem* pItem = ui->list_Plugins->takeItem(k);
    ui->list_Plugins->insertItem(n, pItem);
    ui->list_Plugins->setCurrentRow(n);
    savePriorityPlugin();
}

void SettingsWindow::savePriorityPlugin()
{
    int count = ui->list_Plugins->count();
    for(int index = 0; index < count; index++)
    {
        QListWidgetItem * item = ui->list_Plugins->item(index);
        int indexPlugin =item->data(Qt::UserRole).toUInt();
        _plugins[indexPlugin]->setPriority(count - index);
    }
}
void SettingsWindow::setFirmwareVersion(const QString &firmwareVersion)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    this->fimwareVersion = firmwareVersion;
    versionsUpdate();
}


void SettingsWindow::versionsUpdate()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    ui->retranslateUi(this);

    // Save templete for construct version string
    QString versionsTemplate = ui->labelVersions->text();

#ifdef GIT_REVISION
    versionsTemplate = versionsTemplate.arg(
            QApplication::applicationVersion(),
            GIT_REVISION,
            fimwareVersion );
#else
    versionsTemplate = versionsTemplate.arg(
            QApplication::applicationVersion(),
            "unknown",
            fimwareVersion );
    versionsTemplate.remove(QRegExp(" \\([^()]+unknown[^()]+\\)"));
#endif

    ui->labelVersions->setText( versionsTemplate );

    adjustSize();

    setFixedSize( sizeHint() );
}

void SettingsWindow::on_pushButton_3_clicked()
{
    QCoreApplication::postEvent(ui->horizontalSlider_DeviceSmooth, new QHelpEvent(QEvent::WhatsThis, QPoint(0,0), QCursor::pos()));
}
