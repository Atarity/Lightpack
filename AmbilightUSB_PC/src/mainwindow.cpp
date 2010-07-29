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
    timer = new QTimer(this);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(timer, SIGNAL(timeout()), this, SLOT(timerForUsbPoll()));
    connect(ui->spinBox_UpdateDelay, SIGNAL(valueChanged(int)), this, SLOT(usbTimerDelayMsChange()));
    connect(ui->spinBox_ReconnectDelay, SIGNAL(valueChanged(int)), this, SLOT(usbTimerReconnectDelayMsChange()));
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SLOT(close()));

    usbTimerDelayMsChange();
    usbTimerReconnectDelayMsChange();

    isErrorState = false;
    isAmbilightOn = false;
    ambilightOn();
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
        return;
    }

    if(!isAmbilightOn){
        trayAmbilightOn();

        timer->start(usbTimerDelayMs);
        isAmbilightOn = true;
    }
}

void MainWindow::ambilightOff()
{
    if(isAmbilightOn){
        trayAmbilightOff();

        timer->stop();
        if(!isErrorState){
            ambilight_usb->offLeds();
        }else{
            isErrorState = false;
        }
        isAmbilightOn = false;
    }
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
    this->show();
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
    usbTimerDelayMs = 1000 / ui->spinBox_UpdateDelay->value(); /* hz to ms */
}

void MainWindow::usbTimerReconnectDelayMsChange()
{
    usbTimerReconnectDelayMs = ui->spinBox_ReconnectDelay->value() * 1000; /* sec to ms */
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
