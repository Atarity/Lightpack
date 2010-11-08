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
#include "aboutdialog.h"            /* About dialog */
#include "settings.h"               /* QSettings */
#include "ambilightusb.h"           /* class AmbilightUsb */
#include "grabdesktopwindowleds.h"  /* class GrabDesktopWindowLeds */

#include "ledcolors.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void ambilightUsbSuccess(bool isSuccess);
    void refreshAmbilightEvaluated(double updateResultMs);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showAbout(); /* using in actions */
    void showSettings(); /* using in actions */
    void ambilightOn(); /* using in actions */
    void ambilightOff(); /* using in actions */

    void settingsSoftwareOptionsChange();
    void settingsHardwareTimerOptionsChange();
    void settingsHardwareColorDepthOptionChange();



private:    
    void connectSignalsSlots();

    void trayAmbilightOn();
    void trayAmbilightOff();
    void trayAmbilightError();

    void createTrayIcon();
    void createActions();
    void loadSettingsToMainWindow();        

    void updatePwmFrequency();

private:
    AmbilightUsb *ambilightUsb;
    GrabDesktopWindowLeds *grabDesktopWindowLeds;

    bool isAmbilightOn; /* is grab desktop window ON */
    bool isErrorState;

    // Evaluated frequency of the PWM generation
    double pwmFrequency;


    Ui::MainWindow *ui;

    QAction *onAmbilightAction;
    QAction *offAmbilightAction;
    QAction *settingsAction;
    QAction *aboutAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;    
};

#endif // MAINWINDOW_H
