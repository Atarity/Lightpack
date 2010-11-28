/*
 * mainwindow.cpp
 *
 *  Created on: 26.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: AmbilightUSB
 *
 *  AmbilightUSB is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  AmbilightUSB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  AmbilightUSB is distributed in the hope that it will be useful,
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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createTrayIcon();

    qDebug() << "MainWindow(): new AmbilightUsb(this)";
    ambilightUsb = new AmbilightUsb(this);

    qDebug() << "MainWindow(): new GrabDesktopWindowLeds(this)";
    grabDesktopWindowLeds = new GrabDesktopWindowLeds(this);

    qDebug() << "MainWindow(): loadSettingsToMainWindow()";
    loadSettingsToMainWindow();

    qDebug() << "MainWindow(): connectSignalsSlots()";
    connectSignalsSlots();


    // Initialize limits of height and width
    ui->horizontalSliderWidth->setMaximum( QApplication::desktop()->width() / 2 );
    ui->spinBox_WidthAmbilight->setMaximum( QApplication::desktop()->width() / 2 );

    // 25px - default height of panels in Ubuntu 10.04
    ui->horizontalSliderHeight->setMaximum( QApplication::desktop()->height() / 2  - 25);
    ui->spinBox_HeightAmbilight->setMaximum( QApplication::desktop()->height() / 2 - 25);

        
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


    this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Window);    

    qDebug() << "MainWindow(): initialized";
}

void MainWindow::connectSignalsSlots()
{
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SLOT(close()));

    // TODO: ambilightUsb slot setUsbReconnectDelay
    //connect(ui->spinBox_ReconnectDelay, SIGNAL(valueChanged(int)), ambilightUsb, SLOT(setUsbReconnectDelay(int)));    
    connect(ui->doubleSpinBoxUsbSendDataTimeout, SIGNAL(valueChanged(double)), ambilightUsb, SLOT(setUsbSendDataTimeoutMs(double)));

    // Connect to grabDesktopWindowLeds
    connect(ui->spinBox_UpdateDelay, SIGNAL(valueChanged(int)), grabDesktopWindowLeds, SLOT(setAmbilightRefreshDelayMs(int)));
    connect(ui->checkBox_ShowPixelsAmbilight, SIGNAL(toggled(bool)), grabDesktopWindowLeds, SLOT(setVisibleGrabPixelsRects(bool)));
    connect(ui->spinBox_WidthAmbilight, SIGNAL(valueChanged(int)), grabDesktopWindowLeds, SLOT(setAmbilightWidth(int)));
    connect(ui->spinBox_HeightAmbilight, SIGNAL(valueChanged(int)), grabDesktopWindowLeds, SLOT(setAmbilightHeight(int)));
    connect(ui->doubleSpinBox_WB_Red, SIGNAL(valueChanged(double)), grabDesktopWindowLeds, SLOT(setAmbilightWhiteBalanceR(double)));
    connect(ui->doubleSpinBox_WB_Green, SIGNAL(valueChanged(double)), grabDesktopWindowLeds, SLOT(setAmbilightWhiteBalanceG(double)));
    connect(ui->doubleSpinBox_WB_Blue, SIGNAL(valueChanged(double)), grabDesktopWindowLeds, SLOT(setAmbilightWhiteBalanceB(double)));
    connect(ui->horizontalSlider_HW_ColorDepth, SIGNAL(valueChanged(int)), grabDesktopWindowLeds, SLOT(setAmbilightColorDepth(int)));    
    connect(ui->radioButton_Colored, SIGNAL(toggled(bool)), grabDesktopWindowLeds, SLOT(setColoredGrabPixelsRects(bool)));
    connect(ui->radioButton_White, SIGNAL(toggled(bool)), grabDesktopWindowLeds, SLOT(setWhiteGrabPixelsRects(bool)));

    // Connect grabDesktopWindowLeds with ambilightUsb
    connect(grabDesktopWindowLeds, SIGNAL(updateLedsColors(LedColors)), ambilightUsb, SLOT(updateColors(LedColors)));

    // Software options
    connect(ui->spinBox_UpdateDelay, SIGNAL(valueChanged(int)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->checkBox_ShowPixelsAmbilight, SIGNAL(toggled(bool)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->spinBox_WidthAmbilight, SIGNAL(valueChanged(int)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->spinBox_HeightAmbilight, SIGNAL(valueChanged(int)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->doubleSpinBox_WB_Red, SIGNAL(valueChanged(double)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->doubleSpinBox_WB_Green, SIGNAL(valueChanged(double)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->doubleSpinBox_WB_Blue, SIGNAL(valueChanged(double)), this, SLOT(settingsSoftwareOptionsChange()));
    // Hardware options
    connect(ui->horizontalSlider_HW_ColorDepth, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareColorDepthOptionChange()));
    connect(ui->horizontalSlider_HW_Prescaller, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareTimerOptionsChange()));
    connect(ui->horizontalSlider_HW_OCR, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareTimerOptionsChange()));
    // Software/Hardware options
    connect(ui->spinBox_ReconnectDelay, SIGNAL(valueChanged(int)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->doubleSpinBoxUsbSendDataTimeout, SIGNAL(valueChanged(double)), this, SLOT(settingsSoftwareOptionsChange()));

    // ambilightUsb to this
    connect(ambilightUsb, SIGNAL(openDeviceSuccess(bool)), this, SLOT(ambilightUsbSuccess(bool)));
    connect(ambilightUsb, SIGNAL(readBufferFromDeviceSuccess(bool)), this, SLOT(ambilightUsbSuccess(bool)));
    connect(ambilightUsb, SIGNAL(writeBufferToDeviceSuccess(bool)), this, SLOT(ambilightUsbSuccess(bool)));

    // grabDesktopWindowLeds to this
    connect(grabDesktopWindowLeds, SIGNAL(ambilightTimeOfUpdatingColors(double)), this, SLOT(refreshAmbilightEvaluated(double)));    
}


MainWindow::~MainWindow()
{
    delete ui;
    delete onAmbilightAction;
    delete offAmbilightAction;
    delete settingsAction;
    delete aboutAction;
    delete quitAction;

    delete trayIcon;
    delete trayIconMenu;

    delete ambilightUsb;
    delete grabDesktopWindowLeds;
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
        grabDesktopWindowLeds->setVisibleGrabPixelsRects(false);
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
    settings->setValue("IsAmbilightOn", isAmbilightOn);
    grabDesktopWindowLeds->setAmbilightOn(isAmbilightOn);
}

void MainWindow::trayAmbilightOn()
{
    trayIcon->setIcon(QIcon(":/icons/on.png"));
    trayIcon->setToolTip(tr("Ambilight USB. On state."));
}

void MainWindow::trayAmbilightOff()
{
    trayIcon->setIcon(QIcon(":/icons/off.png"));
    trayIcon->setToolTip(tr("Ambilight USB. Off state."));
}

void MainWindow::trayAmbilightError()
{
    trayIcon->setIcon(QIcon(":/icons/error.png"));
    trayIcon->setToolTip(tr("Ambilight USB. Error state."));
}



void MainWindow::showAbout()
{
    QString hardwareVerison = ambilightUsb->hardwareVersion();
    QString firmwareVerison = ambilightUsb->firmwareVersion();
    aboutDialog *about = new aboutDialog(hardwareVerison, firmwareVerison, this);
    about->show();
}

void MainWindow::showSettings()
{
    this->move(QApplication::desktop()->width() / 2 - this->width() / 2,
            QApplication::desktop()->height() / 2 - this->height() / 2);
    grabDesktopWindowLeds->setVisibleGrabPixelsRects(ui->checkBox_ShowPixelsAmbilight->isChecked());
    this->show();
}


// SLOT
void MainWindow::ambilightUsbSuccess(bool isSuccess)
{    
    if(isErrorState && isSuccess){
        isErrorState = false;
        trayAmbilightOn();

        qWarning() << "Ambilight USB. On state.";
        grabDesktopWindowLeds->clearColors();
    }else if(!isErrorState && !isSuccess){
        isErrorState = true;
        trayAmbilightError();

        qWarning() << "Ambilight USB. Error state.";
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

    ui->lineEdit_RefreshAmbilihtEvaluated->setText( QString::number(hz,'f', 4) /* ms to hz */ );
}

void MainWindow::settingsSoftwareOptionsChange()
{
    settings->setValue("RefreshAmbilightDelayMs", ui->spinBox_UpdateDelay->value());
    settings->setValue("ReconnectAmbilightUSBDelayMs", ui->spinBox_ReconnectDelay->value() * 1000 /* sec to ms */);
    settings->setValue("WidthAmbilight",  ui->spinBox_WidthAmbilight->value());
    settings->setValue("HeightAmbilight", ui->spinBox_HeightAmbilight->value());
    settings->setValue("UsbSendDataTimeout", (int)(ui->doubleSpinBoxUsbSendDataTimeout->value() * 1000));
    settings->setValue("WhiteBalanceCoefRed", ui->doubleSpinBox_WB_Red->value());
    settings->setValue("WhiteBalanceCoefGreen", ui->doubleSpinBox_WB_Green->value());
    settings->setValue("WhiteBalanceCoefBlue", ui->doubleSpinBox_WB_Blue->value());
}

// Send timer options to device
void MainWindow::settingsHardwareTimerOptionsChange()
{
    int timerPrescallerIndex = ui->comboBox_HW_Prescaller->currentIndex();
    int timerOutputCompareRegValue = ui->spinBox_HW_OCR->value();

    settings->setValue("HwTimerPrescallerIndex", timerPrescallerIndex);
    settings->setValue("HwTimerOCR", timerOutputCompareRegValue);

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

    settings->setValue("HwColorDepth", colorDepth);

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

void MainWindow::updatePwmFrequency()
{
    int timerPrescallerIndex = ui->comboBox_HW_Prescaller->currentIndex();
    int timerOutputCompareRegValue = ui->spinBox_HW_OCR->value();
    int colorDepth = ui->horizontalSlider_HW_ColorDepth->value();

    if(colorDepth == 0) {
        qWarning() << "void MainWindow::settingsHardwareOptionsChange() Magic! colorDepth == 0";
        return;
    }
    pwmFrequency = 12000000 / colorDepth; // colorDepth - PWM level max value;

    switch(timerPrescallerIndex){
    case CMD_SET_PRESCALLER_1:      break;
    case CMD_SET_PRESCALLER_8:      pwmFrequency /= 8; break;
    case CMD_SET_PRESCALLER_64:     pwmFrequency /= 64;break;
    case CMD_SET_PRESCALLER_256:    pwmFrequency /= 256;break;
    case CMD_SET_PRESCALLER_1024:   pwmFrequency /= 1024;break;
    default: qWarning() << "bad value of 'timerPrescallerIndex' =" << timerPrescallerIndex; break;
    }
    pwmFrequency /= timerOutputCompareRegValue;

    ui->lineEdit_PWM_Frequency->setText(QString::number(pwmFrequency,'g',3));
}



void MainWindow::createActions()
{
    onAmbilightAction = new QAction(tr("&On ambilight"), this);
    connect(onAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOn()));

    offAmbilightAction = new QAction(tr("O&ff ambilight"), this);
    connect(offAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOff()));

    settingsAction = new QAction(tr("&Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
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
    ui->spinBox_ReconnectDelay->setValue( settings->value("ReconnectAmbilightUSBDelayMs").toInt() / 1000 /* ms to sec */ );

    ui->spinBox_WidthAmbilight->setValue( settings->value("WidthAmbilight").toInt() );
    ui->spinBox_HeightAmbilight->setValue( settings->value("HeightAmbilight").toInt() );

    ui->doubleSpinBoxUsbSendDataTimeout->setValue( settings->value("UsbSendDataTimeout").toInt() / 1000.0 /* ms to sec */ );

    ui->horizontalSlider_HW_OCR->setValue( settings->value("HwTimerOCR").toInt());
    ui->horizontalSlider_HW_Prescaller->setValue( settings->value("HwTimerPrescallerIndex").toInt());
    ui->horizontalSlider_HW_ColorDepth->setValue( settings->value("HwColorDepth").toInt());

    ui->doubleSpinBox_WB_Red->setValue(settings->value("WhiteBalanceCoefRed").toDouble());
    ui->doubleSpinBox_WB_Green->setValue(settings->value("WhiteBalanceCoefGreen").toDouble());
    ui->doubleSpinBox_WB_Blue->setValue(settings->value("WhiteBalanceCoefBlue").toDouble());

    updatePwmFrequency(); // eval PWM generation frequency and show it in settings
    settingsHardwareColorDepthOptionChange(); // synchonize color depth value with device
    settingsHardwareTimerOptionsChange(); // synchonize timer options with device
}
