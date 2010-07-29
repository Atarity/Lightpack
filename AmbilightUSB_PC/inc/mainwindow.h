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

#include <QtGui>
#include "aboutdialog.h"
#include "settings.h"
#include "ambilightusb.h"
#include "rectgetpixel.h"


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

    void showAbout();
    void showSettings();

    void timerForUsbPoll();
    void usbTimerDelayMsChange();
    void usbTimerReconnectDelayMsChange();
    void settingsShowPixelsForAmbilight(bool state);
    void settingsShowPixelsWithTransparentBackground(bool state);
    void settingsStepXChange();
    void settingsStepYChange();
    void settingsWidthAmbilightChange();
    void settingsHeightAmbilightChange();


private:
    void trayAmbilightOn();
    void trayAmbilightOff();
    void trayAmbilightError();

    void createTrayIcon();
    void createActions();
    void loadSettingsToForm();

    Ui::MainWindow *ui;

    QAction *onAmbilightAction;
    QAction *offAmbilightAction;
    QAction *settingsAction;
    QAction *aboutAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QTimer *timer;

    ambilightUsb *ambilight_usb;
    RectGetPixel *rect_get_pixel;
    bool isAmbilightOn;
    bool isErrorState;

    int usbTimerDelayMs;
    int usbTimerReconnectDelayMs;
};

#endif // MAINWINDOW_H
