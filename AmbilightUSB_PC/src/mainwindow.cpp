/*
 * mainwindow.cpp
 *
 *  Created on: 26.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 */

#include <QtGui>

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
    case QSystemTrayIcon::Trigger: break;
    case QSystemTrayIcon::DoubleClick:
        if(isAmbilightOn){
            ambilightOff();
        }else{
            ambilightOn();
        }
        break;
    case QSystemTrayIcon::MiddleClick:
        QMessageBox::information(0, tr("Systray"), tr("Hello. MiddleClick!"));
        break;
    default:
        ;
    }
}

void MainWindow::ambilightOn()
{
    if(isErrorState){
        timer->start(USB_TIMER_MS);
        isAmbilightOn = true;
        return;
    }

    if(!isAmbilightOn){
        trayIcon->setIcon(QIcon(":/res/on.png"));
        trayIcon->setToolTip("Ambilight USB. On state.");

        timer->start(USB_TIMER_MS);
        isAmbilightOn = true;
    }
}

void MainWindow::ambilightOff()
{
    if(isAmbilightOn){
        trayIcon->setIcon(QIcon(":/res/off.png"));
        trayIcon->setToolTip("Ambilight USB. Off state.");

        timer->stop();
        if(!isErrorState){
            ambilight_usb->offLeds();
        }else{
            isErrorState = false;
        }
        isAmbilightOn = false;
    }
}

void MainWindow::showSettings()
{
    this->move(QApplication::desktop()->width() / 2 - this->width() / 2,
            QApplication::desktop()->height() / 2 - this->height() / 2);
    this->show();
}


void MainWindow::timerForUsbPoll()
{
    if(ambilight_usb->updateColorsIfChanges()){
        if(isErrorState){
            isErrorState = false;
            trayIcon->setIcon(QIcon(":/res/on.png"));
            trayIcon->setToolTip("Ambilight USB. On state.");
            qWarning() << "Ambilight USB. On state.";
        }
        timer->start( USB_TIMER_MS );
    }else{
        if(!isErrorState){
            isErrorState = true;
            trayIcon->setIcon(QIcon(":/res/error.png"));
            trayIcon->setToolTip("Ambilight USB. Error state.");
            qWarning() << "Ambilight USB. Error state.";
        }

        timer->start( USB_RECONNECT_TIMER_MS );
    }
}



void MainWindow::createActions()
{
    onAmbilightAction = new QAction(tr("&On"), this);
    connect(onAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOn()));

    offAmbilightAction = new QAction(tr("O&ff"), this);
    connect(offAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOff()));

    settingsAction = new QAction(tr("&Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

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
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}
