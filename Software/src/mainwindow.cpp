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

    qDebug() << "MainWindow(): new AmbilightUsb(this)";
    ambilightUsb = new AmbilightUsb(this);

    qDebug() << "MainWindow(): new GrabDesktopWindowLeds()";
    grabDesktopWindowLeds = new GrabDesktopWindowLeds();

    qDebug() << "MainWindow(): loadSettingsToMainWindow()";
    loadSettingsToMainWindow();

    qDebug() << "MainWindow(): connectSignalsSlots()";
    connectSignalsSlots();
        
    isErrorState = false;
    isAmbilightOn = settings->value("IsAmbilightOn").toBool();
    if(isAmbilightOn){
        isAmbilightOn = false; // ambilightOn() check this and if this true, return
        qDebug() << "MainWindow(): ambilightOn()";
        ambilightOn();
    }else{
        isAmbilightOn = true; // ambilightOn() check this and if this false, return
        qDebug() << "MainWindow(): ambilightOff()";
        ambilightOff();
    }

    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

    logsFilePath = "";

    qDebug() << "Check screen geometry...";

    int screen = QApplication::desktop()->primaryScreen();
    qDebug() << "  primaryScreen =" << screen;
    qDebug() << "  isVirtualDesktop =" << QApplication::desktop()->isVirtualDesktop();
    qDebug() << "  numScreens = " << QApplication::desktop()->numScreens();
    qDebug() << "  primaryScreen Width x Height = " << Desktop::WidthAvailable << "x" << Desktop::HeightAvailable;
    qDebug() << "  desktop       Width x Height = " << Desktop::WidthFull << "x" << Desktop::HeightFull;
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
    settings->setValue("IsAmbilightOn", isAmbilightOn);
    grabDesktopWindowLeds->setAmbilightOn(isAmbilightOn);
}

void MainWindow::ambilightOff()
{
    if(isAmbilightOn){
        qDebug("trayAmbilightOff...");
        trayAmbilightOff();

        if(!isErrorState){
            qDebug("isErrorState == false");
            if(ambilightUsb->deviceOpened()){
                ambilightUsb->offLeds();
            }
        }else{
            isErrorState = false;
        }
        isAmbilightOn = false;
    }
    qDebug("settings...");
    settings->setValue("IsAmbilightOn", isAmbilightOn);
    qDebug("grabDesktopWindowLeds...");
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
    about->move(Desktop::WidthAvailable / 2 - about->width() / 2,
            Desktop::HeightFull / 2 - about->height() / 2);
    about->show();
}

void MainWindow::showSettings()
{
    this->adjustSize();
    this->move(Desktop::WidthAvailable / 2 - this->width() / 2,
            Desktop::HeightFull / 2 - this->height() / 2);
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

    ui->label_UpdateFrequencyEval->setText( QString::number(hz,'f', 4) /* ms to hz */ );
}

void MainWindow::settingsSoftwareOptionsChange()
{
    settings->setValue("RefreshAmbilightDelayMs", ui->spinBox_UpdateDelay->value());
}

// Send timer options to device
void MainWindow::settingsHardwareTimerOptionsChange()
{
    int timerPrescallerIndex = settings->value("Firmware/TimerPrescallerIndex").toInt();
    int timerOutputCompareRegValue = ui->spinBox_HW_OCR->value();

    settings->setValue("Firmware/TimerOCR", timerOutputCompareRegValue);

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

    settings->setValue("Firmware/ColorDepth", colorDepth);

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
    settings->setValue("Firmware/IsSmoothChangeColors", isSmooth);
    ambilightUsb->smoothChangeColors(isSmooth);
}


void MainWindow::openLogsFile()
{
    QDesktopServices::openUrl( QUrl(this->logsFilePath) );
}

void MainWindow::openSettingsFile()
{
    QDesktopServices::openUrl( QUrl(settings->fileName()) );
}


void MainWindow::updatePwmFrequency()
{
    int timerPrescallerIndex = settings->value("Firmware/TimerPrescallerIndex").toInt();
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

    ui->label_PWM_Freq->setText(QString::number(pwmFrequency,'g',3));
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
    ui->spinBox_UpdateDelay->setValue( settings->value("RefreshAmbilightDelayMs").toInt() );
    ui->spinBox_MinLevelOfSensitivity->setValue( settings->value("MinimumLevelOfSensitivity").toInt() );
    ui->checkBox_AVG_Colors->setChecked( settings->value("IsAvgColorsOn").toBool() );

    ui->horizontalSlider_HW_OCR->setValue( settings->value("Firmware/TimerOCR").toInt() );
    ui->horizontalSlider_HW_ColorDepth->setValue( settings->value("Firmware/ColorDepth").toInt() );
    ui->checkBox_SmoothChangeColors->setChecked( settings->value("Firmware/IsSmoothChangeColors").toBool() );


    updatePwmFrequency(); // eval PWM generation frequency and show it in settings
    settingsHardwareColorDepthOptionChange(); // synchonize color depth value with device
    settingsHardwareTimerOptionsChange(); // synchonize timer options with device    
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
