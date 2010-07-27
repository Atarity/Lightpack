/*
 * mainwindow.h
 *
 *  Created on: 26.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

#include "ambilightusb.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void ambilightOn();
    void ambilightOff();
    void showSettings();
    void timerForUsbPoll();
    void usbTimerDelayMsChange();
    void usbTimerReconnectDelayMsChange();


private:
    void createTrayIcon();
    void createActions();

    Ui::MainWindow *ui;

    QAction *onAmbilightAction;
    QAction *offAmbilightAction;
    QAction *settingsAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QTimer *timer;

    ambilightUsb *ambilight_usb;
    bool isAmbilightOn;
    bool isErrorState;

    int usbTimerDelayMs;
    int usbTimerReconnectDelayMs;
};

#endif // MAINWINDOW_H
