/*
 * mainwindow.cpp
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


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "LedDeviceFactory.hpp"
#include <QDesktopWidget>
#include <QPlainTextEdit>
#include "WinAPIGrabber.hpp"
#include "QtGrabber.hpp"
#include "X11Grabber.hpp"
#include "MacOSGrabber.hpp"
#include "debug.h"
#include "../src/apiserver.h"

// ----------------------------------------------------------------------------
// Lightpack settings window
// ----------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    server = NULL;

    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex( 0 );

    createActions();
    createTrayIcon();

    QRect screen = QApplication::desktop()->screenGeometry(this);

    this->setWindowFlags( Qt::Window
                          | Qt::WindowStaysOnTopHint
                          | Qt::CustomizeWindowHint
                          | Qt::WindowCloseButtonHint );
    this->setFocus(Qt::OtherFocusReason);

    // Check windows reserved simbols in profile input name
    QRegExp rx("[^<>:\"/\\|?*]+");
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    ui->comboBox_Profiles->lineEdit()->setValidator(validator);

    ledDevice = LedDeviceFactory::create(this);

    grabManager = new GrabManager(createGrabber(Settings::getGrabMode()));

    aboutDialog = new AboutDialog(this);

    speedTest = new SpeedTest();

#ifndef WINAPI_GRAB_SUPPORT
    ui->radioButton_GrabWinAPI->setVisible(false);
#endif
#ifndef X11_GRAB_SUPPORT
    ui->radioButton_GrabX11->setVisible(false);
#endif
#ifndef QT_GRAB_SUPPORT
    ui->radioButton_GrabQt->setVisible(false);
#else
    ui->radioButton_GrabQt->setChecked(true);
#endif
    profilesFindAll();

    initLanguages();

    initLabelsForGrabbedColors();

    connectSignalsSlots();

    profileLoadLast();

    loadTranslation( Settings::valueMain( "Language" ).toString() );

    isErrorState = false;
    isAmbilightOn = Settings::value("IsAmbilightOn").toBool();

    onGrabModeChanged();

    this->adjustSize();
    this->move( screen.width() / 2  - this->width() / 2,
                screen.height() / 2 - this->height() / 2);
    this->resize(this->minimumSize());

    updateCbModesPosition();
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "initialized";
}

void MainWindow::startApi()
{
    if (!Settings::isApiEnabled()) return;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Start API server";

    int port = Settings::valueMain("ApiPort").toInt();

    server = new ApiServer(this);
    server-> ApiKey = Settings::valueMain("ApiKey").toString();

    if (!server->listen(QHostAddress::Any, port)) {
        QString errorStr = tr("API server unable to start (port: %1): %2.").arg(port).arg(server->errorString());

        QMessageBox::critical(this, tr("API Server"), errorStr);
        qCritical() << Q_FUNC_INFO << errorStr;
    }
}

void MainWindow::connectSignalsSlots()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SLOT(close()));

    // Connect to GrabManager
    connect(ui->spinBox_SlowdownGrab, SIGNAL(valueChanged(int)), grabManager, SLOT(setAmbilightSlowdownMs(int)));
    connect(ui->groupBox_ShowGrabWidgets, SIGNAL(toggled(bool)), grabManager, SLOT(setVisibleLedWidgets(bool)));
    connect(ui->spinBox_HW_ColorDepth, SIGNAL(valueChanged(int)), grabManager, SLOT(setAmbilightColorDepth(int)));
    connect(ui->radioButton_Colored, SIGNAL(toggled(bool)), grabManager, SLOT(setColoredLedWidgets(bool)));
    connect(ui->radioButton_White, SIGNAL(toggled(bool)), grabManager, SLOT(setWhiteLedWidgets(bool)));
    connect(ui->checkBox_USB_SendDataOnlyIfColorsChanges, SIGNAL(toggled(bool)), grabManager, SLOT(setUpdateColorsOnlyIfChanges(bool)));
    connect(ui->checkBox_AVG_Colors, SIGNAL(toggled(bool)), grabManager, SLOT(setAvgColorsOnAllLeds(bool)));
    connect(ui->spinBox_MinLevelOfSensitivity, SIGNAL(valueChanged(int)), grabManager, SLOT(setMinLevelOfSensivity(int)));
    connect(ui->doubleSpinBox_HW_GammaCorrection, SIGNAL(valueChanged(double)), grabManager, SLOT(setGrabGammaCorrection(double)));
    connect(ui->radioButton_LiquidColorMoodLampMode, SIGNAL(toggled(bool)), this, SLOT(onMoodLampModeChanged(bool)));
    connect(this, SIGNAL(settingsProfileChanged()), grabManager, SLOT(settingsProfileChanged()));

    // Main options
    connect(ui->cb_Modes,SIGNAL(activated(int)), this, SLOT(onCbModesChanged(int)));
    connect(ui->comboBox_Language, SIGNAL(activated(QString)), this, SLOT(loadTranslation(QString)));
    connect(ui->pushButton_EnableDisableDevice, SIGNAL(clicked()), this, SLOT(grabAmbilightOnOff()));

    // Hardware options
    connect(ui->spinBox_HW_ColorDepth, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareSetColorDepth(int)));
    connect(ui->spinBox_HW_OCR, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareTimerOptionsChange()));
    // TODO: remove checkBox_SmoothChangeColors
//    connect(ui->checkBox_SmoothChangeColors, SIGNAL(toggled(bool)), this, SLOT(settingsHardwareChangeColorsIsSmooth(bool)));

    // LedDevice connections
    connectLedDeviceSignalsSlots();

    // GrabManager to this
    connect(grabManager, SIGNAL(ambilightTimeOfUpdatingColors(double)), this, SLOT(refreshAmbilightEvaluated(double)));

    // Open Settings file
    connect(ui->commandLinkButton_OpenSettings, SIGNAL(clicked()), this, SLOT(openCurrentProfile()));

    // Connect profile signals to this slots
    connect(ui->comboBox_Profiles->lineEdit(), SIGNAL(returnPressed()), this, SLOT(profileRename()));
    connect(ui->comboBox_Profiles, SIGNAL(currentIndexChanged(QString)), this, SLOT(profileSwitch(QString)));
    connect(ui->pushButton_ProfileNew, SIGNAL(clicked()), this, SLOT(profileNew()));
    connect(ui->pushButton_ProfileResetToDefault, SIGNAL(clicked()), this, SLOT(profileResetToDefaultCurrent()));
    connect(ui->pushButton_DeleteProfile, SIGNAL(clicked()), this, SLOT(profileDeleteCurrent()));

    connect(this, SIGNAL(settingsProfileChanged()), this, SLOT(settingsProfileChanged_UpdateUI()));
    connect(ui->pushButton_SelectColor, SIGNAL(colorChanged(QColor)), this, SLOT(onMoodLampColorChanged(QColor)));
    connect(ui->checkBox_ExpertModeEnabled, SIGNAL(toggled(bool)), this, SLOT(onExpertModeEnabledChanged(bool)));



    // Dev tab
    connect(ui->pushButton_StartTests, SIGNAL(clicked()), this, SLOT(startTestsClick()));

    connect(ui->radioButton_GrabQt, SIGNAL(toggled(bool)), this, SLOT(onGrabModeChanged()));
#ifdef WINAPI_GRAB_SUPPORT
    connect(ui->radioButton_GrabWinAPI, SIGNAL(toggled(bool)), this, SLOT(onGrabModeChanged()));
#endif
#ifdef X11_GRAB_SUPPORT
    connect(ui->radioButton_GrabX11, SIGNAL(toggled(bool)), this, SLOT(onGrabModeChanged()));
#endif

    connect(grabManager, SIGNAL(updateLedsColors(QList<StructRGB>)), this, SLOT(updateGrabbedColors(QList<StructRGB>)));
    connect(ui->spinBox_HW_SmoothSlowdown, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareSetSmoothSlowdown(int)));
    connect(ui->spinBox_HW_Brightness, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareSetBrightness(int)));
    connect(ui->spinBox_HW_SetAvgColor, SIGNAL(valueChanged(int)), this, SLOT(setAvgColorOnAllLEDs(int)));
    connect(ui->checkBox_ConnectVirtualDevice, SIGNAL(toggled(bool)), this, SLOT(onCheckBox_ConnectVirtualDeviceToggled(bool)));

    connect(ui->groupBox_Api,SIGNAL(toggled(bool)),this,SLOT(onGroupBox_EnableApiToggled(bool)));
    connect(ui->lineEdit_ApiPort,SIGNAL(textChanged(QString)),this,SLOT(onApiPort_Changed(QString)));
    connect(ui->pushButton_NewKey,SIGNAL(clicked()),this,SLOT(genNewKey()));
    connect(ui->lineEdit_ApiKey,SIGNAL(textChanged(QString)),this,SLOT(onApiKey_Changed(QString)));
    connect(ui->checkBox_ApiAuth,SIGNAL(toggled(bool)),this,SLOT(onApiAuth_Toggled(bool)));
}

void MainWindow::connectLedDeviceSignalsSlots()
{
    // Connect GrabManager with ledDevice
    connect(grabManager, SIGNAL(updateLedsColors(const QList<StructRGB> &)), ledDevice, SLOT(updateColors(const QList<StructRGB> &)));
    connect(server, SIGNAL(updateLedsColors(const QList<StructRGB> &)), ledDevice, SLOT(updateColors(const QList<StructRGB> &)));

    // ledDevice to this
    connect(ledDevice, SIGNAL(openDeviceSuccess(bool)), this, SLOT(ledDeviceCallSuccess(bool)));
    connect(ledDevice, SIGNAL(ioDeviceSuccess(bool)), this, SLOT(ledDeviceCallSuccess(bool)));
}

void MainWindow::disconnectLedDeviceSignalsSlots()
{
    // Connect GrabManager with ledDevice
    disconnect(grabManager, SIGNAL(updateLedsColors(const QList<StructRGB> &)), ledDevice, SLOT(updateColors(const QList<StructRGB> &)));
    disconnect(server, SIGNAL(updateLedsColors(const QList<StructRGB> &)), ledDevice, SLOT(updateColors(const QList<StructRGB> &)));

    // ledDevice to this
    disconnect(ledDevice, SIGNAL(openDeviceSuccess(bool)), this, SLOT(ledDeviceCallSuccess(bool)));
    disconnect(ledDevice, SIGNAL(ioDeviceSuccess(bool)), this, SLOT(ledDeviceCallSuccess(bool)));
}

MainWindow::~MainWindow()
{    
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    delete onAmbilightAction;
    delete offAmbilightAction;
    delete settingsAction;
    delete aboutAction;
    delete quitAction;

    delete trayIcon;
    delete trayIconMenu;

    delete ledDevice;
    delete grabManager;

    delete ui;
}

// ----------------------------------------------------------------------------
// Events
// ----------------------------------------------------------------------------

void MainWindow::changeEvent(QEvent *e)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << e->type();

    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:

        ui->retranslateUi(this);
        onAmbilightAction->setText( tr("&Turn on") );
        offAmbilightAction->setText( tr("&Turn off") );
        settingsAction->setText( tr("&Settings") );
        aboutAction->setText( tr("&About") );
        quitAction->setText( tr("&Quit") );

        profilesMenu->setTitle( tr("&Profiles") );

        if( isAmbilightOn ){
            trayIcon->setToolTip( tr("Enabled profile: %1").arg( ui->comboBox_Profiles->lineEdit()->text() ) );
        }else{
            trayIcon->setToolTip( tr("Disabled") );
        }

        if(isErrorState) trayIcon->setToolTip(tr("Error with connection device, verbose in logs"));

        setWindowTitle( tr("Lightpack: %1").arg( ui->comboBox_Profiles->lineEdit()->text() ) );

        ui->comboBox_Language->setItemText( 0, tr("System default") );

        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if( trayIcon->isVisible() ){
        // Just hide settings
        hideSettings();
        event->ignore();
    }
}
void MainWindow::onExpertModeEnabledChanged(bool isEnabled)
{
    Settings::setExpertModeEnabled(isEnabled);
    updateExpertModeWidgetsVisibility();
}

void MainWindow::updateExpertModeWidgetsVisibility()
{
    ui->groupBox_pwmColorDepth->setVisible(Settings::isExpertModeEnabled());
    ui->groupBox_pwmTimerDelay->setVisible(Settings::isExpertModeEnabled());
    ui->label_GammaCorrection->setVisible(Settings::isExpertModeEnabled());
    ui->doubleSpinBox_HW_GammaCorrection->setVisible(Settings::isExpertModeEnabled());
    ui->checkBox_USB_SendDataOnlyIfColorsChanges->setVisible(Settings::isExpertModeEnabled());
    if(Settings::isExpertModeEnabled()) {
        if (ui->tabWidget->indexOf(ui->tabAnotherGUI) < 0)
            ui->tabWidget->addTab(ui->tabAnotherGUI, tr("Dev tab"));
    } else {
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabAnotherGUI));
    }
}

void MainWindow::onCheckBox_ConnectVirtualDeviceToggled(bool isEnabled)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isEnabled;

    if (isEnabled){
        Settings::setConnectedDevice(SupportedDevice_Virtual);
    } else {
        // TODO: think about saving last connected device to main config
        Settings::setConnectedDevice(SupportedDevice_Default);
    }

    disconnectLedDeviceSignalsSlots();

    delete ledDevice;
    ledDevice = LedDeviceFactory::create(this);

    connectLedDeviceSignalsSlots();
}

void MainWindow::onGroupBox_EnableApiToggled(bool isEnabled)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isEnabled;

    Settings::setApiEnabled(isEnabled);
    if (isEnabled){
        startApi();
    } else {
        if (server!=NULL)
        {
            server->close();
            delete server;
        }
    }
}

void MainWindow::onApiPort_Changed(QString apiport)
{
     DEBUG_LOW_LEVEL << Q_FUNC_INFO << apiport;
     Settings::setValueMain("ApiPort",apiport);
     if (server!=NULL)
     {
         server->close();
         delete server;
     }
     startApi();
}

void MainWindow::onApiKey_Changed(QString apikey)
{
     DEBUG_LOW_LEVEL << Q_FUNC_INFO << apikey;
     Settings::setValueMain("ApiKey",apikey);
     if (server!=NULL)
     {
         server->ApiKey = apikey;
     }
}

void MainWindow::genNewKey()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    ui->lineEdit_ApiKey->setText(QUuid::createUuid().toString());
}

void MainWindow::onApiAuth_Toggled(bool isEnabled)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isEnabled;
    Settings::setValueMain("ApiAuth",isEnabled);
    if (server!=NULL)
    {
        server->enableAuth = isEnabled;
    }
}

// ----------------------------------------------------------------------------
// Ambilight On / Off
// ----------------------------------------------------------------------------

void MainWindow::ambilightOn()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    isAmbilightOn = true;
    startAmbilight();
}

void MainWindow::ambilightOff()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    isAmbilightOn = false;
    startAmbilight();
}

void MainWindow::grabAmbilightOnOff()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    isAmbilightOn = !isAmbilightOn;
    startAmbilight();
}

void MainWindow::startAmbilight()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isAmbilightOn;

    Settings::setValue("IsAmbilightOn", isAmbilightOn);
    grabManager->setAmbilightOn( isAmbilightOn, isErrorState );

    if(isAmbilightOn == false){
        isErrorState = false;
    }

    updateTrayAndActionStates();
}

void MainWindow::updateTrayAndActionStates()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if( isAmbilightOn ){
        ui->pushButton_EnableDisableDevice->setIcon(QIcon(":/icons/off.png"));
        ui->label_EnableDisableDevice->setText(tr("Switch off Lightpack"));
    }else{
        ui->pushButton_EnableDisableDevice->setIcon(QIcon(":/icons/on.png"));
        ui->label_EnableDisableDevice->setText(tr("Switch on Lightpack"));
    }

    if( isErrorState ){
        trayIcon->setIcon(QIcon(":/icons/error.png"));
        trayIcon->setToolTip(tr("Error with connection device, verbose in logs"));
    }else{
        if( isAmbilightOn ){
            trayIcon->setIcon(QIcon(":/icons/on.png"));
            trayIcon->setToolTip( tr("Enabled profile: %1").arg( ui->comboBox_Profiles->lineEdit()->text() ) );

            onAmbilightAction->setEnabled(false);
            offAmbilightAction->setEnabled(true);
        }else{
            trayIcon->setIcon(QIcon(":/icons/off.png"));
            trayIcon->setToolTip(tr("Disabled"));

            onAmbilightAction->setEnabled(true);
            offAmbilightAction->setEnabled(false);
        }
    }    
}

// ----------------------------------------------------------------------------
// Show grabbed colors in another GUI
// ----------------------------------------------------------------------------

void MainWindow::initLabelsForGrabbedColors()
{
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        QLabel *label = new QLabel(this);
        label->setText(QString::number( ledIndex+1 ));
        label->setAutoFillBackground(true);

        labelsGrabbedColors.append(label);
        ui->horizontalLayout_GrabbedColors->addWidget(label);
    }
}

void MainWindow::updateGrabbedColors(const QList<StructRGB> & colors)
{
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        QLabel *label = labelsGrabbedColors[ ledIndex ];
        QColor color(colors[ ledIndex ].rgb);

        QPalette pal = label->palette();
        pal.setBrush(QPalette::Window, QBrush(color));
        label->setPalette(pal);
    }
}

void MainWindow::setAvgColorOnAllLEDs(int value)
{
    QList<StructRGB> colors;

    for (int i = 0; i < LEDS_COUNT; i++)
    {
        colors << StructRGB(qRgb(value, value, value), 0);
    }

    ledDevice->updateColors(colors);

    ui->label_HW_SetAvgColor_Value->setText(
            QString("0b%1").arg(QString::number(value, 2), 8, '0'));
}

// ----------------------------------------------------------------------------
// Show / Hide settings and about windows
// ----------------------------------------------------------------------------

void MainWindow::showAbout()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QRect screen = QApplication::desktop()->screenGeometry(this);

    aboutDialog->setFirmwareVersion( ledDevice->firmwareVersion() );

    aboutDialog->move(screen.width() / 2 - aboutDialog->width() / 2,
            screen.height() / 2 - aboutDialog->height() / 2);

    aboutDialog->show();
}

void MainWindow::showSettings()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    LightpackMode mode = Settings::getMode();
    ui->cb_Modes->setCurrentIndex   ( mode == Grab ? 0 : 1 ); // we assume that LightpackMode in same order as cb_Modes
    grabManager->setVisibleLedWidgets( ui->groupBox_ShowGrabWidgets->isChecked() && ui->cb_Modes->currentIndex()==0 );
    this->show();
}

void MainWindow::hideSettings()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    grabManager->setVisibleLedWidgets(false);
    this->hide();
}

// ----------------------------------------------------------------------------
// Public slots
// ----------------------------------------------------------------------------

void MainWindow::ledDeviceCallSuccess(bool isSuccess)
{    
    DEBUG_MID_LEVEL << Q_FUNC_INFO << isSuccess;

    if(isErrorState != ! isSuccess){
        isErrorState = ! isSuccess;
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "isErrorState" << isErrorState;
    }

    updateTrayAndActionStates();
}

void MainWindow::refreshAmbilightEvaluated(double updateResultMs)
{    
    DEBUG_MID_LEVEL << Q_FUNC_INFO << updateResultMs;

    double secs = updateResultMs / 1000;
    double hz = 0;

    if(secs != 0){
        hz = 1 / secs;
    }

    ui->label_GrabFrequency_value->setText( QString::number(hz,'f', 2) /* ms to hz */ );
}

// ----------------------------------------------------------------------------
// Send timer options to device
// ----------------------------------------------------------------------------
void MainWindow::settingsHardwareTimerOptionsChange()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    int timerPrescallerIndex = Settings::value("Firmware/TimerPrescallerIndex").toInt();
    int timerOutputCompareRegValue = ui->spinBox_HW_OCR->value();

    Settings::setValue("Firmware/TimerOCR", timerOutputCompareRegValue);

    updatePwmFrequency();

    if(pwmFrequency > 1000){
        qWarning() << "PWM frequency to high! setTimerOptions canceled. pwmFrequency =" << pwmFrequency << "Hz";
    }else if(pwmFrequency < 10){
        qWarning() << "PWM frequency to low! setTimerOptions canceled. pwmFrequency =" << pwmFrequency << "Hz";
    }else{
        // Set timer for PWM generation options. 10Hz <= pwmFrequency <= 1000Hz
        ledDevice->setTimerOptions(timerPrescallerIndex, timerOutputCompareRegValue);
    }
}

// ----------------------------------------------------------------------------
// Send color depth to device
// ----------------------------------------------------------------------------
void MainWindow::settingsHardwareSetColorDepth(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    Settings::setValue("Firmware/ColorDepth", value);

    updatePwmFrequency();

    if(pwmFrequency > 1000){
        qWarning() << "PWM frequency to high! setColorDepth canceled. pwmFrequency =" << pwmFrequency << "Hz";
    }else if(pwmFrequency < 10){
        qWarning() << "PWM frequency to low! setColorDepth canceled. pwmFrequency =" << pwmFrequency << "Hz";
    }else{
        // Set timer for PWM generation options. 10Hz <= pwmFrequency <= 1000Hz
        ledDevice->setColorDepth(value);
    }
}

void MainWindow::settingsHardwareSetSmoothSlowdown(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    Settings::setValue("Firmware/SmoothSlowdown", value);
    ledDevice->setSmoothSlowdown(value);
}

void MainWindow::settingsHardwareSetBrightness(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    // TODO: settings
    Settings::setValue("Firmware/Brightness", value);
    ledDevice->setBrightness(value);
}

// ----------------------------------------------------------------------------


void MainWindow::openFile(const QString &filePath)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString filePrefix = "file://";

#ifdef Q_WS_WIN
    filePrefix = "file:///";
#endif

    QDesktopServices::openUrl( QUrl( filePrefix + filePath, QUrl::TolerantMode) );
}

// ----------------------------------------------------------------------------
// Profiles
// ----------------------------------------------------------------------------

void MainWindow::openCurrentProfile()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    openFile( Settings::fileName() );
}

void MainWindow::profileRename()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    //
    // Press <Enter> in profile edit line will perform renaming it
    // also update settings for usable configuration LED coefs
    //
    QString configName = ui->comboBox_Profiles->currentText().trimmed();
    ui->comboBox_Profiles->setItemText( ui->comboBox_Profiles->currentIndex(), configName );

    if(configName == ""){
        return;
    }

    Settings::renameCurrentConfig(configName);

    this->setFocus(Qt::OtherFocusReason);

    loadSettingsToMainWindow();
    emit settingsProfileChanged();
}

void MainWindow::profileSwitch(const QString & configName)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << configName;

    Settings::loadOrCreateConfig(configName);

    this->setFocus(Qt::OtherFocusReason);

    // Update settings
    loadSettingsToMainWindow();
    emit settingsProfileChanged();
}

// Slot for switch profiles by tray menu
void MainWindow::profileTraySwitch()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    for(int i=0; i < profilesMenu->actions().count(); i++){
        QAction * action = profilesMenu->actions().at(i);
        if( action->isChecked() ){
            if( action->text() != ui->comboBox_Profiles->currentText() ){
                DEBUG_LOW_LEVEL << Q_FUNC_INFO << "switch to" << action->text();
                profileSwitchCombobox( action->text() );
                return;
            }
        }else{
            if( action->text() == ui->comboBox_Profiles->currentText() ){
                DEBUG_LOW_LEVEL << Q_FUNC_INFO << "set checked" << action->text();
                action->setChecked(true);
            }
        }
    }
}

void MainWindow::profileSwitchCombobox(QString profile)
{
    int index = ui->comboBox_Profiles->findText( profile );
    ui->comboBox_Profiles->setCurrentIndex( index );
}

void MainWindow::profileNew()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString profileName = tr("New profile");

    if(ui->comboBox_Profiles->findText(profileName) != -1){
        int i = 1;
        while( ui->comboBox_Profiles->findText(profileName +" "+ QString::number(i)) != -1 ){
            i++;
        }
        profileName += + " " + QString::number(i);
    }

    ui->comboBox_Profiles->insertItem( 0, profileName );
    ui->comboBox_Profiles->setCurrentIndex( 0 );

    ui->comboBox_Profiles->lineEdit()->selectAll();
    ui->comboBox_Profiles->lineEdit()->setFocus(Qt::MouseFocusReason);
}

void MainWindow::profileResetToDefaultCurrent()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    Settings::resetDefaults();
    // Update settings
    loadSettingsToMainWindow();
    emit settingsProfileChanged();
}

void MainWindow::profileDeleteCurrent()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if(ui->comboBox_Profiles->count() <= 1){
        qWarning() << "void MainWindow::profileDeleteCurrent(): profiles count ==" << ui->comboBox_Profiles->count();
        return;
    }

    // Delete settings file
    Settings::removeCurrentConfig();
    // Remove from combobox
    ui->comboBox_Profiles->removeItem( ui->comboBox_Profiles->currentIndex() );
}

QStringList MainWindow::profilesFindAll()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QFileInfo setsFile( Settings::fileName() );
    QFileInfoList iniFiles = setsFile.absoluteDir().entryInfoList(QStringList("*.ini"));

    QStringList settingsFiles;
    for(int i=0; i<iniFiles.count(); i++){
        QString compBaseName = iniFiles.at(i).completeBaseName();
        settingsFiles.append( compBaseName );
    }

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "found profiles:" << settingsFiles;
    for(int i=0; i<settingsFiles.count(); i++){
        if(ui->comboBox_Profiles->findText(settingsFiles.at(i)) == -1){
            ui->comboBox_Profiles->addItem(settingsFiles.at(i));
        }
    }
    return settingsFiles;
}

void MainWindow::profileLoadLast()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    ui->comboBox_Profiles->setCurrentIndex( ui->comboBox_Profiles->findText( Settings::lastProfileName() ) );

    // Update settings
    loadSettingsToMainWindow();
    emit settingsProfileChanged();
}

void MainWindow::settingsProfileChanged_UpdateUI()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    setWindowTitle( tr("Lightpack: %1").arg( ui->comboBox_Profiles->lineEdit()->text() ) );
    if(isAmbilightOn) trayIcon->setToolTip( tr("Enabled profile: %1").arg( ui->comboBox_Profiles->lineEdit()->text() ) );

    if(ui->comboBox_Profiles->count() > 1){
        ui->pushButton_DeleteProfile->setEnabled(true);
    }else{
        ui->pushButton_DeleteProfile->setEnabled(false);
    }

    profileTraySync();
}

// Syncronize profiles from combobox with tray menu
void MainWindow::profileTraySync()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QAction *profileAction;

    profilesMenu->clear();

    for(int i=0; i < ui->comboBox_Profiles->count(); i++){
        profileAction = new QAction( ui->comboBox_Profiles->itemText( i ), this );
        profileAction->setCheckable( true );
        if( i == ui->comboBox_Profiles->currentIndex() ){
            profileAction->setChecked( true );
        }
        profilesMenu->addAction( profileAction );
        connect(profileAction, SIGNAL(triggered()), this, SLOT(profileTraySwitch()));
    }
}


// ----------------------------------------------------------------------------
// Translate GUI
// ----------------------------------------------------------------------------

void MainWindow::initLanguages()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    ui->comboBox_Language->clear();
    ui->comboBox_Language->addItem( tr("System default") );
    ui->comboBox_Language->addItem( "English" );
    ui->comboBox_Language->addItem( "Russian" );

    int langIndex = 0; // "System default"
    QString langSaved = Settings::valueMain("Language").toString();
    if( langSaved != "<System>" ){
        langIndex = ui->comboBox_Language->findText( langSaved );
    }
    ui->comboBox_Language->setCurrentIndex( langIndex );

    translator = NULL;
}

void MainWindow::loadTranslation(const QString & language)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << language;

    Settings::setValueMain( "Language", language );

    QString locale = QLocale::system().name();

    // add translation to Lightpack.pro TRANSLATIONS
    // lupdate Lightpack.pro
    // open linguist and translate application
    // lrelease Lightpack.pro
    // add new language to LightpackResources.qrc :/translations/
    // add new language to MainWindow::initLanguages() function
    // and only when all this done - append new line
    // locale - name of translation binary file form resources: %locale%.qm
    if( ui->comboBox_Language->currentIndex() == 0 /* System */ ){
        qDebug() << "System locale" << locale;
        Settings::setValueMain( "Language", LANGUAGE_DEFAULT_NAME );
    }
    else if ( language == "English" ) locale = "en_EN"; // :/translations/en_EN.qm
    else if ( language == "Russian" ) locale = "ru_RU"; // :/translations/ru_RU.qm
    // append line for new language/locale here
    else {
        qWarning() << "Language" << language << "not found. Set to default" << LANGUAGE_DEFAULT_NAME;
        qDebug() << "System locale" << locale;

        Settings::setValueMain( "Language", LANGUAGE_DEFAULT_NAME );
    }

    QString pathToLocale = QString(":/translations/") + locale;

    if( translator != NULL ){
        qApp->removeTranslator(translator);
        delete translator;
        translator = NULL;
    }

    if( locale == "en_EN" /* default no need to translate */ ){
        qDebug() << "Translation removed, using default locale" << locale;
        return;
    }

    translator = new QTranslator();
    if(translator->load( pathToLocale )){
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Load translation for locale" << locale;
        qApp->installTranslator(translator);
    }else{
        qWarning() << "Fail load translation for locale" << locale << "pathToLocale" << pathToLocale;
    }
}

// ----------------------------------------------------------------------------


void MainWindow::updatePwmFrequency()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    int timerPrescallerIndex = Settings::value("Firmware/TimerPrescallerIndex").toInt();
    int timerOutputCompareRegValue = ui->spinBox_HW_OCR->value();
    int colorDepth = ui->horizontalSlider_HW_ColorDepth->value();

    if(colorDepth == 0) {
        qWarning() << "void MainWindow::settingsHardwareOptionsChange() Magic! colorDepth == 0";
        return;
    }

    // Eval with oscilloscope
    const double timeSetAllLeds_secs = 30.0 / 1000 / 1000;
    const double timeSmoothlyUpdateLeds_secs = 483.0 / 1000 / 1000;

    double timeBetweenPwm_secs = timerOutputCompareRegValue / 16000000.0;

    switch(timerPrescallerIndex){
    case CMD_SET_PRESCALLER_1:      break;
    case CMD_SET_PRESCALLER_8:      timeBetweenPwm_secs *= 8; break;
    case CMD_SET_PRESCALLER_64:     timeBetweenPwm_secs *= 64; break;
    case CMD_SET_PRESCALLER_256:    timeBetweenPwm_secs *= 256; break;
    case CMD_SET_PRESCALLER_1024:   timeBetweenPwm_secs *= 1024; break;
    default: qWarning() << "bad value of 'timerPrescallerIndex' =" << timerPrescallerIndex; break;
    }

    double timePwm_secs = (timeBetweenPwm_secs + timeSetAllLeds_secs) * colorDepth + timeSmoothlyUpdateLeds_secs;

    pwmFrequency = 1 / timePwm_secs;

    ui->label_PWM_Freq->setText(QString::number(pwmFrequency,'f',2));
}


void MainWindow::onGrabModeChanged()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "GrabMode" << getGrabMode();
    IGrabber * grabber = createGrabber(getGrabMode());
    Settings::setGrabMode(getGrabMode());
    grabManager->setGrabber(grabber);
}

IGrabber * MainWindow::createGrabber(GrabMode grabMode)
{
    switch (grabMode)
    {
#ifdef Q_WS_X11
    case X11GrabMode:
        return new X11Grabber();
#endif
#ifdef Q_WS_WIN
    case WinAPIGrabMode:
        return new WinAPIGrabber();
#endif
    default:
        return new QtGrabber();
    }
}

// ----------------------------------------------------------------------------
// Start grab speed tests
// ----------------------------------------------------------------------------

void MainWindow::startTestsClick()
{
    QString saveText = ui->pushButton_StartTests->text();
    ui->pushButton_StartTests->setText( "Please wait..." );
    ui->pushButton_StartTests->repaint(); // update right now

    // While testing this function freezes GUI
    speedTest->start();

    ui->pushButton_StartTests->setText( saveText );
}

// ----------------------------------------------------------------------------
// Create tray icon and actions
// ----------------------------------------------------------------------------

void MainWindow::createActions()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    onAmbilightAction = new QAction(QIcon(":/icons/on.png"), tr("&Turn on"), this);
    onAmbilightAction->setIconVisibleInMenu(true);
    connect(onAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOn()));

    offAmbilightAction = new QAction(QIcon(":/icons/off.png"), tr("&Turn off"), this);
    offAmbilightAction->setIconVisibleInMenu(true);
    connect(offAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOff()));


    profilesMenu = new QMenu(tr("&Profiles"), this);
    profilesMenu->setIcon( QIcon(":/icons/profiles.png"));
    profilesMenu->clear();

    settingsAction = new QAction(QIcon(":/icons/settings.png"), tr("&Settings"), this);
    settingsAction->setIconVisibleInMenu(true);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

    aboutAction = new QAction(QIcon(":/icons/about.png"), tr("&About"), this);
    aboutAction->setIconVisibleInMenu(true);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(onAmbilightAction);
    trayIconMenu->addAction(offAmbilightAction);
    trayIconMenu->addSeparator();    
    trayIconMenu->addMenu(profilesMenu);
    trayIconMenu->addAction(settingsAction);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icons/off.png"));
    trayIcon->show();
}

// ----------------------------------------------------------------------------
// Process icon click event
// ----------------------------------------------------------------------------

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        if(isErrorState){
            ambilightOff();
        }else{
            if(isAmbilightOn){
                ambilightOff();
            }else{
                ambilightOn();
            }
        }
        break;

    case QSystemTrayIcon::MiddleClick:
        if( this->isVisible() ){
            hideSettings();
        }else{
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
        trayIconMenu->activateWindow();
        break;
#   endif


    default:
        ;
    }
}

// ----------------------------------------------------------------------------
// Read current profile to main settings window
// ----------------------------------------------------------------------------

void MainWindow::loadSettingsToMainWindow()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    ui->spinBox_SlowdownGrab->setValue                  ( Settings::getGrabSlowdownMs());
    ui->spinBox_MinLevelOfSensitivity->setValue         ( Settings::value("MinimumLevelOfSensitivity").toInt() );
    ui->doubleSpinBox_HW_GammaCorrection->setValue      ( Settings::value("GammaCorrection").toDouble() );
    ui->checkBox_AVG_Colors->setChecked                 ( Settings::value("IsAvgColorsOn").toBool() );
    LightpackMode mode = Settings::getMode();
    ui->cb_Modes->setCurrentIndex                       ( mode == Grab ? 0 : 1 ); // we assume that LightpackMode in same order as cb_Modes
    ui->horizontalSlider_Speed->setValue                ( Settings::getMoodLampSpeed());
    ui->horizontalSlider_Brightness->setValue           ( Settings::value("Brightness").toInt());
    ui->pushButton_SelectColor->setColor                ( Settings::getMoodLampColor());
    ui->radioButton_LiquidColorMoodLampMode->setChecked ( Settings::isMoodLampLiquidMode());
    ui->radioButton_ConstantColorMoodLampMode->setChecked(!Settings::isMoodLampLiquidMode());

    ui->horizontalSlider_HW_OCR->setValue           ( Settings::value("Firmware/TimerOCR").toInt() );
    ui->horizontalSlider_HW_OCR->setValue           ( Settings::value("Firmware/TimerOCR").toInt() );
    ui->horizontalSlider_HW_ColorDepth->setValue    ( Settings::value("Firmware/ColorDepth").toInt() );
    ui->spinBox_HW_SmoothSlowdown->setValue         ( Settings::value("Firmware/SmoothSlowdown").toInt());

    ui->checkBox_ExpertModeEnabled->setChecked      ( Settings::isExpertModeEnabled() );
    ui->checkBox_ConnectVirtualDevice->setChecked   ( Settings::getConnectedDevice() == SupportedDevice_Virtual );

    ui->groupBox_Api->setChecked    ( Settings::valueMain("EnableApi").toBool());
    ui->lineEdit_ApiPort->setText   ( Settings::valueMain("ApiPort").toString());
    ui->lineEdit_ApiKey->setText    ( Settings::valueMain("ApiKey").toString());
    ui->checkBox_ApiAuth->setChecked( Settings::valueMain("ApiAuth").toBool());

    switch(Settings::getGrabMode())
    {
#ifdef WINAPI_GRAB_SUPPORT
    case WinAPIGrabMode:
        ui->radioButton_GrabWinAPI->setChecked(true);
        break;
#endif
#ifdef X11_GRAB_SUPPORT
    case X11GrabMode:
        ui->radioButton_GrabX11->setChecked(true);
        break;
#endif
    default:
        ui->radioButton_GrabQt->setChecked(true);
    }

    updatePwmFrequency(); // eval PWM generation frequency and show it in settings
    onCbModesChanged ( ui->cb_Modes->currentIndex() ); //
    onMoodLampModeChanged(ui->radioButton_LiquidColorMoodLampMode->isChecked());
    updateExpertModeWidgetsVisibility();
    onGrabModeChanged();
}

GrabMode MainWindow::getGrabMode()
{
#ifdef X11_GRAB_SUPPORT
    if (ui->radioButton_GrabX11->isChecked()) {
        return X11GrabMode;
    }
#endif
#ifdef WINAPI_GRAB_SUPPORT
    if (ui->radioButton_GrabWinAPI->isChecked()) {
        return WinAPIGrabMode;
    }
#endif

    return QtGrabMode;
}

// ----------------------------------------------------------------------------
// Quit application
// ----------------------------------------------------------------------------

void MainWindow::quit()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "trayIcon->hide();";

    trayIcon->hide();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "QApplication::quit();";

    QApplication::quit();
}

void MainWindow::onCbModesChanged(int index)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << index;

    switch (index)
    {
    case 0:
        ui->frmBacklight->setVisible(false);
        ui->frmAmbilight->setVisible(true);
        grabManager->setVisibleLedWidgets(ui->groupBox_ShowGrabWidgets->isChecked() && this->isVisible());
        break;
    case 1:
        ui->frmBacklight->setVisible(true);
        ui->frmAmbilight->setVisible(false);
        grabManager->setVisibleLedWidgets(false);
        break;
    }
    grabManager->switchMode(index == 0 ? Grab : MoodLamp);
//    Settings::setMode(index == 0 ? Grab : MoodLamp);
}

void MainWindow::on_horizontalSlider_Brightness_valueChanged(int value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << value;
    grabManager->setBrightness(value);
}

void MainWindow::onMoodLampColorChanged(QColor color)
{
    grabManager->setBackLightColor(color);
}


void MainWindow::on_horizontalSlider_Speed_valueChanged(int value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO<< value;
    grabManager->setMoodLampSpeed(value);
}

void MainWindow::paintEvent(QPaintEvent */*event*/)
{
    updateCbModesPosition();
}

void MainWindow::updateCbModesPosition()
{
    int newX = ui->groupBox_7->x() + 10;
    int newY = ui->groupBox_7->y();
    ui->cb_Modes->move(newX, newY);
    ui->cb_Modes->raise();
}

void MainWindow::onMoodLampModeChanged(bool checked)
{
    Settings::setMoodLampLiquidMode(checked);
    if(!checked)
    {
        //constant mode
        ui->pushButton_SelectColor->setEnabled(true);
        ui->label_MoodLampColor->setEnabled(true);
        ui->horizontalSlider_Speed->setEnabled(false);
        ui->label_MoodLampSpeed->setEnabled(false);
        ui->horizontalSlider_Brightness->setEnabled(false);
        ui->label_MoodLampBrightness->setEnabled(false);
        grabManager->setMoodLampSpeed(0);
    } else {
        //liquid mode
        ui->pushButton_SelectColor->setEnabled(false);
        ui->label_MoodLampColor->setEnabled(false);
        ui->horizontalSlider_Speed->setEnabled(true);
        ui->label_MoodLampSpeed->setEnabled(true);
        ui->horizontalSlider_Brightness->setEnabled(true);
        ui->label_MoodLampBrightness->setEnabled(true);
    }
}
