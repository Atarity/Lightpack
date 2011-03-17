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

//
// Lightpack settings window
//

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex( 0 );


    createActions();
    createTrayIcon();

    this->adjustSize();
    this->move( Desktop::Width / 2  - this->width() / 2,
                Desktop::Height / 2 - this->height() / 2 );
    this->setWindowFlags( Qt::Window
                          | Qt::WindowStaysOnTopHint
                          | Qt::CustomizeWindowHint
                          | Qt::WindowCloseButtonHint );
    this->setFocus(Qt::OtherFocusReason);

    // Check windows reserved simbols in profile input name
    QRegExp rx("[^<>:\"/\\|?*]+");
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    ui->comboBox_Profiles->lineEdit()->setValidator(validator);


    qDebug() << "MainWindow(): new AmbilightUsb(this)";
    ambilightUsb = new AmbilightUsb(this);

    qDebug() << "MainWindow(): new GrabManager()";
    grabManager = new GrabManager();

    qDebug() << "MainWindow(): new AboutDialog(this)";
    aboutDialog = new AboutDialog(this);

    qDebug() << "MainWindow(): profilesFindAll();";
    profilesFindAll();

    qDebug() << "MainWindow(): void initLanguages()";
    initLanguages();

    qDebug() << "MainWindow(): connectSignalsSlots()";
    connectSignalsSlots();

    qDebug() << "MainWindow(): profileLoadLast()";
    profileLoadLast();

    qDebug() << "MainWindow(): loadTranslation(..)";
    loadTranslation( Settings::valueMain( "Language" ).toString() );


    isErrorState = false;
    isAmbilightOn = Settings::value("IsAmbilightOn").toBool();

    logsFilePath = "";

    qDebug() << "MainWindow(): initialized";
}

void MainWindow::connectSignalsSlots()
{
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SLOT(close()));

    // Connect to GrabManager
    connect(ui->spinBox_UpdateDelay, SIGNAL(valueChanged(int)), grabManager, SLOT(setAmbilightRefreshDelayMs(int)));
    connect(ui->groupBox_ShowGrabWidgets, SIGNAL(toggled(bool)), grabManager, SLOT(setVisibleLedWidgets(bool)));
    connect(ui->horizontalSlider_HW_ColorDepth, SIGNAL(valueChanged(int)), grabManager, SLOT(setAmbilightColorDepth(int)));
    connect(ui->radioButton_Colored, SIGNAL(toggled(bool)), grabManager, SLOT(setColoredLedWidgets(bool)));
    connect(ui->radioButton_White, SIGNAL(toggled(bool)), grabManager, SLOT(setWhiteLedWidgets(bool)));
    connect(ui->checkBox_USB_SendDataOnlyIfColorsChanges, SIGNAL(toggled(bool)), grabManager, SLOT(setUpdateColorsOnlyIfChanges(bool)));
    connect(ui->checkBox_AVG_Colors, SIGNAL(toggled(bool)), grabManager, SLOT(setAvgColorsOnAllLeds(bool)));
    connect(ui->spinBox_MinLevelOfSensitivity, SIGNAL(valueChanged(int)), grabManager, SLOT(setMinLevelOfSensivity(int)));
    connect(this, SIGNAL(settingsProfileChanged()), grabManager, SLOT(settingsProfileChanged()));

    // Connect GrabManager with ambilightUsb
    connect(grabManager, SIGNAL(updateLedsColors(const QList<StructRGB> &)), ambilightUsb, SLOT(updateColors(const QList<StructRGB> &)));

    // Main options
    connect(ui->comboBox_Language, SIGNAL(activated(QString)), this, SLOT(loadTranslation(QString)));
    connect(ui->pushButton_GrabOnOff, SIGNAL(clicked(bool)), this, SLOT(grabAmbilightOnOff(bool)));

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

    // GrabManager to this
    connect(grabManager, SIGNAL(ambilightTimeOfUpdatingColors(double)), this, SLOT(refreshAmbilightEvaluated(double)));

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
    delete grabManager;

    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        onAmbilightAction->setText( tr("&Turn on") );
        offAmbilightAction->setText( tr("&Turn off") );
        settingsAction->setText( tr("&Settings") );
        aboutAction->setText( tr("&About") );
        quitAction->setText( tr("&Quit") );

        if(isAmbilightOn) trayIcon->setToolTip( tr("Enabled profile: %1").arg( ui->comboBox_Profiles->lineEdit()->text() ) );
        else trayIcon->setToolTip( tr("Disabled") );

        if(isErrorState) trayIcon->setToolTip(tr("Error with connection device, verbose in logs"));

        setWindowTitle( tr("Lightpack: %1").arg( ui->comboBox_Profiles->lineEdit()->text() ) );

        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if( trayIcon->isVisible() ){
        // Just hide settings
        hideSettings();
        event->ignore();
    }
}

void MainWindow::ambilightOn()
{
    isAmbilightOn = true;
    startAmbilight();
}

void MainWindow::ambilightOff()
{    
    isAmbilightOn = false;
    startAmbilight();
}

void MainWindow::grabAmbilightOnOff(bool state)
{
    isAmbilightOn = state;
    startAmbilight();
}

void MainWindow::updateTrayAndActionStates()
{
    if( isAmbilightOn ){
        ui->pushButton_GrabOnOff->setIcon(QIcon(":/icons/on.png"));
        ui->pushButton_GrabOnOff->setChecked(true);
    }else{
        ui->pushButton_GrabOnOff->setIcon(QIcon(":/icons/off.png"));
        ui->pushButton_GrabOnOff->setChecked(false);
    }

    if( isErrorState ){
        trayIcon->setIcon(QIcon(":/icons/error.png"));
        trayIcon->setToolTip(tr("Error with connection device, verbose in logs"));

        ui->pushButton_GrabOnOff->setIcon(QIcon(":/icons/error.png"));
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



void MainWindow::showAbout()
{
    aboutDialog->setFirmwareVersion( ambilightUsb->firmwareVersion() );

    aboutDialog->move(Desktop::Width / 2 - aboutDialog->width() / 2,
            Desktop::Height / 2 - aboutDialog->height() / 2);

    aboutDialog->show();
}

void MainWindow::showSettings()
{
    grabManager->setVisibleLedWidgets( ui->groupBox_ShowGrabWidgets->isChecked() );
    this->show();
}

void MainWindow::hideSettings()
{
    grabManager->setVisibleLedWidgets(false);
    this->hide();
}

// public slot
void MainWindow::ambilightUsbSuccess(bool isSuccess)
{    
    if(isErrorState != ! isSuccess){
        isErrorState = ! isSuccess;
        qWarning() << "isErrorState state" << isErrorState;
    }

    updateTrayAndActionStates();
}

// public slot
void MainWindow::refreshAmbilightEvaluated(double updateResultMs)
{    
    double secs = updateResultMs / 1000;
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

    if(pwmFrequency > 1000){
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

// private
void MainWindow::openFile(const QString &filePath)
{
    QString filePrefix = "file://";

#ifdef Q_WS_WIN
    filePrefix = "file:///";
#endif

    QDesktopServices::openUrl( QUrl( filePrefix + filePath, QUrl::TolerantMode) );
}


// Logs tab

void MainWindow::openLogsFile()
{
    openFile( this->logsFilePath );
}


// Main tab

void MainWindow::openSettingsFile()
{
    openFile( Settings::fileName() );
}

void MainWindow::profileRename()
{
    // Press <Enter> in profile edit line will perform renaming it
    // also update settings for usable configuration LED coefs

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
        settingsFiles.append( compBaseName );
    }

    qDebug() << "Found profiles:" << settingsFiles;
    for(int i=0; i<settingsFiles.count(); i++){
        if(ui->comboBox_Profiles->findText(settingsFiles.at(i)) == -1){
            ui->comboBox_Profiles->addItem(settingsFiles.at(i));
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
    setWindowTitle( tr("Lightpack: %1").arg( ui->comboBox_Profiles->lineEdit()->text() ) );
    if(isAmbilightOn) trayIcon->setToolTip( tr("Enabled profile: %1").arg( ui->comboBox_Profiles->lineEdit()->text() ) );

    if(ui->comboBox_Profiles->count() > 1){
        ui->pushButton_DeleteProfile->setEnabled(true);
    }else{
        ui->pushButton_DeleteProfile->setEnabled(false);
    }
}

void MainWindow::initLanguages()
{
    ui->comboBox_Language->addItem( "English" );
    ui->comboBox_Language->addItem( "Russian" );

    int langIndex = ui->comboBox_Language->findText( Settings::valueMain("Language").toString() );
    ui->comboBox_Language->setCurrentIndex( langIndex );

    translator = NULL;
}

void MainWindow::loadTranslation(const QString & language)
{
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
    if( language == "<System>" ){
        qDebug() << "System locale" << locale;
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
        qDebug() << "Load translation for locale" << locale;
        qApp->installTranslator(translator);
    }else{
        qWarning() << "Fail load translation for locale" << locale << "pathToLocale" << pathToLocale;
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

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
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

    case QSystemTrayIcon::Trigger:
    default: break;
    }
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

void MainWindow::startAmbilight()
{
    qDebug() << Q_FUNC_INFO << "isAmbilightOn" << isAmbilightOn;

    Settings::setValue("IsAmbilightOn", isAmbilightOn);
    grabManager->setAmbilightOn( isAmbilightOn, isErrorState );

    if(isAmbilightOn == false){
        isErrorState = false;
    }

    updateTrayAndActionStates();
}

void MainWindow::quit()
{
    trayIcon->hide();
    QApplication::quit();
}
