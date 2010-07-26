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

#define USB_TIMER_MS 100

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

//    void messageClicked();
//    void showMessage();
//    void setIcon(int index);


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
};

#endif // MAINWINDOW_H
