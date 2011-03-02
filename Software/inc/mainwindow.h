/*
 * mainwindow.h
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

    void appendLogsLine(const QString & line);
    void setLogsFilePath(const QString & filePath);

signals:
    void settingsProfileChanged();


public slots:
    void ambilightUsbSuccess(bool isSuccess);
    void refreshAmbilightEvaluated(double updateResultMs);

protected:
    virtual void changeEvent(QEvent *e);
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showAbout(); /* using in actions */
    void showSettings(); /* using in actions */
    void ambilightOn(); /* using in actions */
    void ambilightOff(); /* using in actions */
    void quit(); /* using in actions */

    void settingsSoftwareOptionsChange();
    void settingsHardwareTimerOptionsChange();
    void settingsHardwareColorDepthOptionChange();
    void settingsHardwareChangeColorsIsSmooth(bool isSmooth);

    void openLogsFile();
    void openSettingsFile();

    void profileRename();
    void profileSwitch(const QString & configName);
    void profileNew();
    void profileResetToDefaultCurrent();
    void profileDeleteCurrent();
    void settingsProfileChanged_UpdateUI();

private:    
    void connectSignalsSlots();

    void trayAmbilightOn();
    void trayAmbilightOff();
    void trayAmbilightError();

    void createTrayIcon();
    void createActions();
    void loadSettingsToMainWindow();

    void profilesFindAll();
    void profileLoadLast();

    void updatePwmFrequency();

private:
    AmbilightUsb *ambilightUsb;
    GrabDesktopWindowLeds *grabDesktopWindowLeds;

    bool isAmbilightOn; /* is grab desktop window ON */
    bool isErrorState;

    // Evaluated frequency of the PWM generation
    double pwmFrequency;

    QString logsFilePath;

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
