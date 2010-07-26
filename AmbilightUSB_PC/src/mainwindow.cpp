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
    //this->hide();

    createActions();
    createTrayIcon();

    timer = new QTimer(this);
    timer->start(USB_TIMER_MS);
    ambilight_usb = new ambilightUsb();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(timer, SIGNAL(timeout()), this, SLOT(timerForUsbPoll()));

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
    QApplication::exit();
//    if (trayIcon->isVisible()) {
//        hide();
//        event->ignore();
//    }
}


void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        // TODO
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
    trayIcon->setIcon(QIcon(":/res/on.png"));
    trayIcon->setToolTip("Ambilight USB ON");
    // TODO
}

void MainWindow::ambilightOff()
{
    trayIcon->setIcon(QIcon(":/res/off.png"));
    trayIcon->setToolTip("Ambilight USB OFF");
    // TODO
}

void MainWindow::showSettings()
{
    QMessageBox::information(0, tr("Settings"), tr("TODO"));
}


void MainWindow::timerForUsbPoll()
{
    ambilight_usb->updateColorsIfChanges();
    timer->start( USB_TIMER_MS );
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
