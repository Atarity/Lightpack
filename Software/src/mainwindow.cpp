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
#include "desktop.h"

#include <QDesktopWidget>
#include <QPlainTextEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createTrayIcon();

    this->adjustSize();
    this->move(Desktop::Width / 2 - this->width() / 2,
            Desktop::Height / 2 - this->height() / 2);
    this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);


    QRegExp rx("[^<>:\"/\\|?*]+");  // Check windows reserved simbols
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    ui->comboBox_Profiles->lineEdit()->setValidator(validator);


    qDebug() << "MainWindow(): new AmbilightUsb(this)";
    ambilightUsb = new AmbilightUsb(this);

    qDebug() << "MainWindow(): new GrabDesktopWindowLeds()";
    grabDesktopWindowLeds = new GrabDesktopWindowLeds();

    qDebug() << "MainWindow(): profilesFindAll();";
    profilesFindAll();

    qDebug() << "MainWindow(): connectSignalsSlots()";
    connectSignalsSlots();

    qDebug() << "MainWindow(): profilesComboBoxSelectLastProfile();";
    profileLoadLast();


    isErrorState = false;
    isAmbilightOn = Settings::value("IsAmbilightOn").toBool();
    if(isAmbilightOn){
        isAmbilightOn = false; // ambilightOn() check this and if this true, return
        qDebug() << "MainWindow(): ambilightOn()";
        ambilightOn();
    }else{
        isAmbilightOn = true; // ambilightOn() check this and if this false, return
        qDebug() << "MainWindow(): ambilightOff()";
        ambilightOff();
    }    

    logsFilePath = "";

    qDebug() << "MainWindow(): initialized";
}

void MainWindow::connectSignalsSlots()
{
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SLOT(close()));

    // Connect to grabDesktopWindowLeds
    connect(ui->spinBox_UpdateDelay, SIGNAL(valueChanged(int)), grabDesktopWindowLeds, SLOT(setAmbilightRefreshDelayMs(int)));
    connect(ui->groupBox_ShowGrabWidgets, SIGNAL(toggled(bool)), grabDesktopWindowLeds, SLOT(setVisibleLedWidgets(bool)));
    connect(ui->horizontalSlider_HW_ColorDepth, SIGNAL(valueChanged(int)), grabDesktopWindowLeds, SLOT(setAmbilightColorDepth(int)));
    connect(ui->radioButton_Colored, SIGNAL(toggled(bool)), grabDesktopWindowLeds, SLOT(setColoredLedWidgets(bool)));
    connect(ui->radioButton_White, SIGNAL(toggled(bool)), grabDesktopWindowLeds, SLOT(setWhiteLedWidgets(bool)));
    connect(ui->checkBox_USB_SendDataOnlyIfColorsChanges, SIGNAL(toggled(bool)), grabDesktopWindowLeds, SLOT(setUpdateColorsOnlyIfChanges(bool)));
    connect(ui->checkBox_AVG_Colors, SIGNAL(toggled(bool)), grabDesktopWindowLeds, SLOT(setAvgColorsOnAllLeds(bool)));
    connect(ui->spinBox_MinLevelOfSensitivity, SIGNAL(valueChanged(int)), grabDesktopWindowLeds, SLOT(setMinLevelOfSensivity(int)));
    connect(this, SIGNAL(settingsProfileChanged()), grabDesktopWindowLeds, SLOT(settingsProfileChanged()));

    // Connect grabDesktopWindowLeds with ambilightUsb
    connect(grabDesktopWindowLeds, SIGNAL(updateLedsColors(LedColors)), ambilightUsb, SLOT(updateColors(LedColors)));

    // Software options
    connect(ui->spinBox_UpdateDelay, SIGNAL(valueChanged(int)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->groupBox_ShowGrabWidgets, SIGNAL(toggled(bool)), this, SLOT(settingsSoftwareOptionsChange()));
    // Hardware options
    connect(ui->horizontalSlider_HW_ColorDepth, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareColorDepthOptionChange()));
    connect(ui->horizontalSlider_HW_OCR, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareTimerOptionsChange()));
    connect(ui->checkBox_SmoothChangeColors, SIGNAL(toggled(bool)), this, SLOT(settingsHardwareChangeColorsIsSmooth(bool)));

    // ambilightUsb to this
    connect(ambilightUsb, SIGNAL(openDeviceSuccess(bool)), this, SLOT(ambilightUsbSuccess(bool)));
    connect(ambilightUsb, SIGNAL(readBufferFromDeviceSuccess(bool)), this, SLOT(ambilightUsbSuccess(bool)));
    connect(ambilightUsb, SIGNAL(writeBufferToDeviceSuccess(bool)), this, SLOT(ambilightUsbSuccess(bool)));

    // grabDesktopWindowLeds to this
    connect(grabDesktopWindowLeds, SIGNAL(ambilightTimeOfUpdatingColors(double)), this, SLOT(refreshAmbilightEvaluated(double)));    

    // links to Logs and Settings files
    connect(ui->commandLinkButton_OpenLogs, SIGNAL(clicked()), this, SLOT(openLogsFile()));
    connect(ui->commandLinkButton_OpenSettings, SIGNAL(clicked()), this, SLOT(openSettingsFile()));

    // Connect profile signals to this slots
    connect(ui->comboBox_Profiles->lineEdit(), SIGNAL(returnPressed()), this, SLOT(profileRename()));
    connect(ui->comboBox_Profiles, SIGNAL(currentIndexChanged(QString)), this, SLOT(profileSwitch(QString)));
    connect(ui->pushButton_ProfileNew, SIGNAL(clicked()), this, SLOT(profileNew()));
    connect(ui->pushButton_ProfileResetToDefault, SIGNAL(clicked()), this, SLOT(profileResetToDefaultCurrent()));
    connect(ui->pushButton_DeleteProfile, SIGNAL(clicked()), this, SLOT(profileDeleteCurrent()));

    connect(this, SIGNAL(settingsProfileChanged()), this, SLOT(settingsProfileChanged_UpdateUI()));
}


MainWindow::~MainWindow()
{    
    delete onAmbilightAction;
    delete offAmbilightAction;
    delete settingsAction;
    delete aboutAction;
    delete quitAction;

    delete trayIcon;
    delete trayIconMenu;

    delete ambilightUsb;
    delete grabDesktopWindowLeds;

    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        grabDesktopWindowLeds->setVisibleLedWidgets(false);
        this->hide();
        event->ignore();
    }
}


void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        if(isAmbilightOn){
            ambilightOff();
        }else{
            ambilightOn();
        }
        break;
    case QSystemTrayIcon::MiddleClick:
    case QSystemTrayIcon::Trigger:
    default: break;
    }
}

void MainWindow::ambilightOn()
{
    grabDesktopWindowLeds->clearColors();

    if(isErrorState == false && isAmbilightOn == false){
        trayAmbilightOn();
    }
    isAmbilightOn = true;
    Settings::setValue("IsAmbilightOn", isAmbilightOn);
    grabDesktopWindowLeds->setAmbilightOn(isAmbilightOn);
}

void MainWindow::ambilightOff()
{
    if(isAmbilightOn){
        trayAmbilightOff();

        if(!isErrorState){
            if(ambilightUsb->deviceOpened()){
                ambilightUsb->offLeds();
            }
        }else{
            isErrorState = false;
        }
        isAmbilightOn = false;
    }

    Settings::setValue("IsAmbilightOn", isAmbilightOn);
    grabDesktopWindowLeds->setAmbilightOn(isAmbilightOn);
}

void MainWindow::trayAmbilightOn()
{
    trayIcon->setIcon(QIcon(":/icons/on.png"));
    trayIcon->setToolTip(tr("On state"));

    onAmbilightAction->setEnabled(false);
    offAmbilightAction->setEnabled(true);
}

void MainWindow::trayAmbilightOff()
{
    trayIcon->setIcon(QIcon(":/icons/off.png"));
    trayIcon->setToolTip(tr("Off state"));

    onAmbilightAction->setEnabled(true);
    offAmbilightAction->setEnabled(false);
}

void MainWindow::trayAmbilightError()
{
    trayIcon->setIcon(QIcon(":/icons/error.png"));
    trayIcon->setToolTip(tr("Error state"));
}



void MainWindow::showAbout()
{
    QString firmwareVerison = ambilightUsb->firmwareVersion();
    aboutDialog *about = new aboutDialog(firmwareVerison, this);
    about->move(Desktop::Width / 2 - about->width() / 2,
            Desktop::Height / 2 - about->height() / 2);
    about->show();
}

void MainWindow::showSettings()
{
    grabDesktopWindowLeds->setVisibleLedWidgets(ui->groupBox_ShowGrabWidgets->isChecked());    
    this->show();
}


// SLOT
void MainWindow::ambilightUsbSuccess(bool isSuccess)
{    
    if(isErrorState && isSuccess){
        isErrorState = false;
        trayAmbilightOn();

        qWarning() << "on state.";
        grabDesktopWindowLeds->clearColors();
    }else if(!isErrorState && !isSuccess){
        isErrorState = true;
        trayAmbilightError();

        qWarning() << "error state.";
    }    
}

// SLOT
void MainWindow::refreshAmbilightEvaluated(double updateResultMs)
{    
    int usbTimerDelayMs = ui->spinBox_UpdateDelay->value();
    double secs = (updateResultMs + usbTimerDelayMs) / 1000;
    double hz = 0;

    if(secs != 0){
        hz = 1 / secs;
    }

    ui->label_UpdateFrequencyEval->setText( QString::number(hz,'f', 2) /* ms to hz */ );
}

void MainWindow::settingsSoftwareOptionsChange()
{
    Settings::setValue("RefreshAmbilightDelayMs", ui->spinBox_UpdateDelay->value());
}

// Send timer options to device
void MainWindow::settingsHardwareTimerOptionsChange()
{
    int timerPrescallerIndex = Settings::value("Firmware/TimerPrescallerIndex").toInt();
    int timerOutputCompareRegValue = ui->spinBox_HW_OCR->value();

    Settings::setValue("Firmware/TimerOCR", timerOutputCompareRegValue);

    updatePwmFrequency();

    if(pwmFrequency > 10000){
        qWarning() << "PWM frequency to high! setTimerOptions canceled. pwmFrequency =" << pwmFrequency << "Hz";
    }else if(pwmFrequency < 10){
        qWarning() << "PWM frequency to low! setTimerOptions canceled. pwmFrequency =" << pwmFrequency << "Hz";
    }else{
        // Set timer for PWM generation options. 10Hz <= pwmFrequency <= 1000Hz
        ambilightUsb->setTimerOptions(timerPrescallerIndex, timerOutputCompareRegValue);
    }
}

// Send color depth to device
void MainWindow::settingsHardwareColorDepthOptionChange()
{
    int colorDepth = ui->horizontalSlider_HW_ColorDepth->value();

    Settings::setValue("Firmware/ColorDepth", colorDepth);

    updatePwmFrequency();

    if(pwmFrequency > 1000){
        qWarning() << "PWM frequency to high! setColorDepth canceled. pwmFrequency =" << pwmFrequency << "Hz";
    }else if(pwmFrequency < 10){
        qWarning() << "PWM frequency to low! setColorDepth canceled. pwmFrequency =" << pwmFrequency << "Hz";
    }else{
        // Set timer for PWM generation options. 10Hz <= pwmFrequency <= 1000Hz
        ambilightUsb->setColorDepth(colorDepth);
    }
}

void MainWindow::settingsHardwareChangeColorsIsSmooth(bool isSmooth)
{
    Settings::setValue("Firmware/IsSmoothChangeColors", isSmooth);
    ambilightUsb->smoothChangeColors(isSmooth);
}

// Logs tab

void MainWindow::openLogsFile()
{
    // Logs file name does'n contains russian symbols, just simple open it as is; see openSettingsFile() for details
    QDesktopServices::openUrl( QUrl(this->logsFilePath) );
}


// Profile tab

void MainWindow::openSettingsFile()
{
    QString filePrefix = "file://";

#if defined(__WIN32__) || defined(__WIN64__)
    filePrefix = "file:///";
#endif

    QDesktopServices::openUrl( QUrl( filePrefix + Settings::fileName(), QUrl::TolerantMode) );
}

void MainWindow::profileRename()
{
    QString configName = ui->comboBox_Profiles->currentText().trimmed();
    ui->comboBox_Profiles->setItemText( ui->comboBox_Profiles->currentIndex(), configName );

    if(configName == ""){
        return;
    }

    Settings::renameCurrentConfig(configName);

    this->setFocus(Qt::OtherFocusReason);
}

void MainWindow::profileSwitch(const QString & configName)
{
    qDebug() << "Switch profile on:" << configName;

    Settings::loadOrCreateConfig(configName);

    this->setFocus(Qt::OtherFocusReason);

    // Update settings
    loadSettingsToMainWindow();
    emit settingsProfileChanged();
}

void MainWindow::profileNew()
{
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
    Settings::resetToDefaults();
    // Update settings
    loadSettingsToMainWindow();
    emit settingsProfileChanged();
}

void MainWindow::profileDeleteCurrent()
{
    if(ui->comboBox_Profiles->count() <= 1){
        qWarning() << "void MainWindow::profileDeleteCurrent(): profiles count ==" << ui->comboBox_Profiles->count();
        return;
    }

    // Delete settings file
    Settings::removeCurrentConfig();
    // Remove from combobox
    ui->comboBox_Profiles->removeItem( ui->comboBox_Profiles->currentIndex() );
}


void MainWindow::profilesFindAll()
{
    QFileInfo setsFile( Settings::fileName() );
    QFileInfoList iniFiles = setsFile.absoluteDir().entryInfoList(QStringList("*.ini"));

    QStringList settingsFiles;
    for(int i=0; i<iniFiles.count(); i++){
        QString compBaseName = iniFiles.at(i).completeBaseName();
        if(compBaseName != "Main"){
            settingsFiles.append( compBaseName );
        }
    }

    qDebug() << "Find settings files:" << settingsFiles;

    qDebug() << "find Settings::lastProfileName() =" <<  Settings::lastProfileName();

    for(int i=0; i<settingsFiles.count(); i++){
        if(ui->comboBox_Profiles->findText(settingsFiles.at(i)) == -1){
            qDebug() << "i Settings::lastProfileName() =" <<  Settings::lastProfileName();
            ui->comboBox_Profiles->addItem(settingsFiles.at(i));
            qDebug() << "o Settings::lastProfileName() =" <<  Settings::lastProfileName();
        }
    }
}

void MainWindow::profileLoadLast()
{
    ui->comboBox_Profiles->setCurrentIndex( ui->comboBox_Profiles->findText( Settings::lastProfileName() ) );

    // Update settings
    loadSettingsToMainWindow();
    emit settingsProfileChanged();
}

void MainWindow::settingsProfileChanged_UpdateUI()
{
    if(ui->comboBox_Profiles->count() > 1){
        ui->pushButton_DeleteProfile->setEnabled(true);
    }else{
        ui->pushButton_DeleteProfile->setEnabled(false);
    }
}


void MainWindow::updatePwmFrequency()
{
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



void MainWindow::createActions()
{
    onAmbilightAction = new QAction(QIcon(":/icons/on.png"), tr("&Turn on"), this);
    onAmbilightAction->setIconVisibleInMenu(true);
    connect(onAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOn()));

    offAmbilightAction = new QAction(QIcon(":/icons/off.png"), tr("&Turn off"), this);
    offAmbilightAction->setIconVisibleInMenu(true);
    connect(offAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOff()));

    settingsAction = new QAction(QIcon(":/icons/settings.png"), tr("&Settings"), this);
    settingsAction->setIconVisibleInMenu(true);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

    aboutAction = new QAction(QIcon(":/icons/Lightpack.png"), tr("&About"), this);
    aboutAction->setIconVisibleInMenu(true);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(onAmbilightAction);
    trayIconMenu->addAction(offAmbilightAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(settingsAction);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icons/off.png"));
    trayIcon->show();
}

void MainWindow::loadSettingsToMainWindow()
{
    qDebug() << "MainWindow::loadSettingsToMainWindow()";

    ui->spinBox_UpdateDelay->setValue( Settings::value("RefreshAmbilightDelayMs").toInt() );
    ui->spinBox_MinLevelOfSensitivity->setValue( Settings::value("MinimumLevelOfSensitivity").toInt() );
    ui->checkBox_AVG_Colors->setChecked( Settings::value("IsAvgColorsOn").toBool() );

    ui->horizontalSlider_HW_OCR->setValue( Settings::value("Firmware/TimerOCR").toInt() );
    ui->horizontalSlider_HW_ColorDepth->setValue( Settings::value("Firmware/ColorDepth").toInt() );
    ui->checkBox_SmoothChangeColors->setChecked( Settings::value("Firmware/IsSmoothChangeColors").toBool() );


    updatePwmFrequency(); // eval PWM generation frequency and show it in settings
    settingsHardwareChangeColorsIsSmooth( ui->checkBox_SmoothChangeColors->isChecked() );
}

void MainWindow::appendLogsLine(const QString & line)
{
    ui->plainTextLogs->appendPlainText(line);
}

void MainWindow::setLogsFilePath(const QString & filePath)
{
    this->logsFilePath = filePath;
}

void MainWindow::quit()
{
    trayIcon->hide();
    QApplication::quit();
}
