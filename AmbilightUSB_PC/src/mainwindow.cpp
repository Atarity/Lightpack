/*
 * mainwindow.cpp
 *
 *  Created on: 26.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
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

    ambilight_usb = new ambilightUsb();
    rect_get_pixel = new RectGetPixel();    
    timer = new QTimer(this);

    loadSettingsToForm();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(timer, SIGNAL(timeout()), this, SLOT(timerForUsbPoll()));
    connect(ui->spinBox_UpdateDelay, SIGNAL(valueChanged(int)), this, SLOT(usbTimerDelayMsChange()));
    connect(ui->spinBox_ReconnectDelay, SIGNAL(valueChanged(int)), this, SLOT(usbTimerReconnectDelayMsChange()));
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->checkBox_ShowPixelsAmbilight, SIGNAL(toggled(bool)), this, SLOT(settingsShowPixelsForAmbilight(bool)));
    connect(ui->checkBox_ShowPixelsTransparentBackground, SIGNAL(toggled(bool)), this, SLOT(settingsShowPixelsWithTransparentBackground(bool)));
    connect(ui->spinBox_StepX, SIGNAL(valueChanged(int)), this, SLOT(settingsStepXChange()));
    connect(ui->spinBox_StepY, SIGNAL(valueChanged(int)), this, SLOT(settingsStepYChange()));
    connect(ui->spinBox_WidthAmbilight, SIGNAL(valueChanged(int)), this, SLOT(settingsWidthAmbilightChange()));
    connect(ui->spinBox_HeightAmbilight, SIGNAL(valueChanged(int)), this, SLOT(settingsHeightAmbilightChange()));


    usbTimerDelayMs = settings->value("RefreshAmbilightDelayMs").toInt();
    usbTimerReconnectDelayMs = settings->value("ReconnectAmbilightUSBDelayMs").toInt();


    isErrorState = false;
    isAmbilightOn = settings->value("IsAmbilightOn").toBool();
    if(isAmbilightOn){
        isAmbilightOn = false; // ambilightOn() check this and if this true, return
        ambilightOn();
    }else{
        isAmbilightOn = true; // ambilightOn() check this and if this false, return
        ambilightOff();
    }
    trayIcon->show();
}

MainWindow::~MainWindow()
{
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
        settingsShowPixelsForAmbilight(false);
        hide();
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
    ambilight_usb->clearColorSave();

    if(isErrorState){
        timer->start(usbTimerDelayMs);
        isAmbilightOn = true;
    }else if(!isAmbilightOn){
        trayAmbilightOn();

        timer->start(usbTimerDelayMs);
        isAmbilightOn = true;
    }
    settings->setValue("IsAmbilightOn", isAmbilightOn);
}

void MainWindow::ambilightOff()
{
    if(isAmbilightOn){
        trayAmbilightOff();

        timer->stop();
        if(!isErrorState){
            if(ambilight_usb->deviceOpened()){
                ambilight_usb->offLeds();
            }
        }else{
            isErrorState = false;
        }
        isAmbilightOn = false;
    }
    settings->setValue("IsAmbilightOn", isAmbilightOn);
}

void MainWindow::trayAmbilightOn()
{
    trayIcon->setIcon(QIcon(":/res/on.png"));
    trayIcon->setToolTip(tr("Ambilight USB. On state."));
}

void MainWindow::trayAmbilightOff()
{
    trayIcon->setIcon(QIcon(":/res/off.png"));
    trayIcon->setToolTip(tr("Ambilight USB. Off state."));
}

void MainWindow::trayAmbilightError()
{
    trayIcon->setIcon(QIcon(":/res/error.png"));
    trayIcon->setToolTip(tr("Ambilight USB. Error state."));
}



void MainWindow::showSettings()
{
    this->move(QApplication::desktop()->width() / 2 - this->width() / 2,
            QApplication::desktop()->height() / 2 - this->height() / 2);
    settingsShowPixelsForAmbilight(ui->checkBox_ShowPixelsAmbilight->isChecked());
    this->show();
}

void MainWindow::settingsShowPixelsForAmbilight(bool state)
{
    rect_get_pixel->move(0,0);
    if(state){
        rect_get_pixel->setFocusPolicy(Qt::NoFocus);
        rect_get_pixel->setTransparent(ui->checkBox_ShowPixelsTransparentBackground->isChecked());
        rect_get_pixel->showFullScreen();
        this->setFocus();
    }else{
        rect_get_pixel->hide();
    }
}

void MainWindow::settingsShowPixelsWithTransparentBackground(bool state)
{
    rect_get_pixel->setTransparent(state);
}

void MainWindow::timerForUsbPoll()
{
    bool updateResult = ambilight_usb->updateColorsIfChanges();

    if(updateResult == true){
        if(isErrorState){
            isErrorState = false;
            
            trayAmbilightOn();

            qWarning() << "Ambilight USB. On state.";
            ambilight_usb->clearColorSave();
        }
        timer->start( usbTimerDelayMs );
    }else{
        if(!isErrorState){
            isErrorState = true;
            
            trayAmbilightError();

            qWarning() << "Ambilight USB. Error state.";
        }
        timer->start( usbTimerReconnectDelayMs );
    }
}

void MainWindow::usbTimerDelayMsChange()
{
    int ms = 1000 / ui->spinBox_UpdateDelay->value(); /* hz to ms */
    settings->setValue("RefreshAmbilightDelayMs", ms);
    usbTimerDelayMs = ms;
}

void MainWindow::usbTimerReconnectDelayMsChange()
{
    int ms = ui->spinBox_ReconnectDelay->value() * 1000; /* sec to ms */
    settings->setValue("ReconnectAmbilightUSBDelayMs", ms);
    usbTimerReconnectDelayMs = ms;
}

void MainWindow::settingsStepXChange()
{
    settings->setValue("StepX", ui->spinBox_StepX->value());
    rect_get_pixel->settingsChangedUpdateImage();
}

void MainWindow::settingsStepYChange()
{
    settings->setValue("StepY", ui->spinBox_StepY->value());
    rect_get_pixel->settingsChangedUpdateImage();
}

void MainWindow::settingsWidthAmbilightChange()
{
    settings->setValue("WidthAmbilight", ui->spinBox_WidthAmbilight->value() );
    rect_get_pixel->settingsChangedUpdateImage();
}

void MainWindow::settingsHeightAmbilightChange()
{
    settings->setValue("HeightAmbilight", ui->spinBox_HeightAmbilight->value() );
    rect_get_pixel->settingsChangedUpdateImage();
}




void MainWindow::createActions()
{
    onAmbilightAction = new QAction(tr("&On ambilight"), this);
    connect(onAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOn()));

    offAmbilightAction = new QAction(trUtf8("O&ff ambilight"), this);
    connect(offAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOff()));

    settingsAction = new QAction(trUtf8("&Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

    quitAction = new QAction(trUtf8("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(onAmbilightAction);
    trayIconMenu->addAction(offAmbilightAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(settingsAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::loadSettingsToForm()
{
    ui->spinBox_UpdateDelay->setValue( 1000 / settings->value("RefreshAmbilightDelayMs").toInt() /* ms to hz */);
    ui->spinBox_ReconnectDelay->setValue( settings->value("ReconnectAmbilightUSBDelayMs").toInt() / 1000 /* ms to sec */ );

    ui->spinBox_StepX->setValue( settings->value("StepX").toInt() );
    ui->spinBox_StepY->setValue( settings->value("StepY").toInt() );

    ui->spinBox_WidthAmbilight->setValue( settings->value("WidthAmbilight").toInt() );
    ui->spinBox_HeightAmbilight->setValue( settings->value("HeightAmbilight").toInt() );
}
