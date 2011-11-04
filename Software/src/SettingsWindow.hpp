/*
 * SettingsWindow.hpp
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


#pragma once

#include <QtGui>
#include "AboutDialog.hpp"
#include "Settings.hpp"
#include "GrabManager.hpp"
#include "SpeedTest.hpp"
#include "ColorButton.hpp"
#include "enums.hpp"

namespace Ui {
    class SettingsWindow;
}

class GrabManager; // forward declaration

class SettingsWindow : public QMainWindow {
    Q_OBJECT
public:
    SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

public:
    void startBacklight();

signals:
    void settingsProfileChanged();
    void offLeds();
    void updateLedsColors(const QList<QRgb> &);
    void updateRefreshDelay(int value);
    void updateColorDepth(int value);
    void updateSmoothSlowdown(int value);
    void updateGamma(double value);
    void updateBrightness(int percent);
    void requestFirmwareVersion();
    void recreateLedDevice();
    void resultBacklightStatus(Backlight::Status);
    void backlightStatusChanged(Backlight::Status);
    void enableApiServer(bool isEnabled);
    void enableApiAuth(bool isEnabled);
    void updateApiPort(int port);
    void updateApiKey(QString key);

public slots:
    void ledDeviceCallSuccess(bool isSuccess);
    void ledDeviceGetFirmwareVersion(const QString & fwVersion);
    void refreshAmbilightEvaluated(double updateResultMs);

    void setDeviceLockViaAPI(Api::DeviceLockStatus status);
    void setBacklightStatus(Backlight::Status);
    void backlightOn(); /* using in actions */
    void backlightOff(); /* using in actions */
    void profilesLoadAll();
    void profileSwitch(const QString & configName);
    void profileSwitchCombobox(QString profile);
    void updateGrabbedColors(const QList<QRgb> & colors);
    void requestBacklightStatus();
    void onApiServer_ErrorOnStartListening(QString errorMessage);

protected:
    virtual void changeEvent(QEvent *e);
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void onLightpackModes_Activated(int index);
    void onMoodLamp_ColorButton_ColorChanged(QColor color);
    void onMoodLamp_Speed_valueChanged(int value);
    void onMoodLamp_LiquidMode_Toggled(bool isConstantColor);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showAbout(); /* using in actions */
    void showSettings(); /* using in actions */
    void hideSettings(); /* using in iconActivated(..) */

    void quit(); /* using in actions */

    void switchBacklightOnOff();

    void onDeviceRefreshDelay_valueChanged(int value);
    void onDeviceSmooth_valueChanged(int value);
    void onDeviceBrightness_valueChanged(int value);
    void onDeviceGammaCorrection_valueChanged(double value);

    void openCurrentProfile();

    void profileRename();
    void profileTraySwitch();
    void profileNew();
    void profileResetToDefaultCurrent();
    void profileDeleteCurrent();
    void settingsProfileChanged_UpdateUI();

    void loadTranslation(const QString & language);

    void onGrabModeChanged();
    void startTestsClick();

    void onCheckBox_ExpertModeEnabled_Toggled(bool isEnabled);
    void onCheckBox_ConnectVirtualDevice_Toggled(bool isEnabled);
    void onGroupBox_EnableApi_Toggled(bool isEnabled);
    void onButton_GenerateNewApiKey_Clicked();
    void onButton_SetApiPort_Clicked();
    void onCheckBox_IsApiAuthEnabled_Toggled(bool isEnabled);

private:
    void connectSignalsSlots();   

    void updateTrayAndActionStates();
    void updateExpertModeWidgetsVisibility();

    void syncLedDeviceWithSettingsWindow();

    void createTrayIcon();
    void createActions();
    void updateUiFromSettings();

    void grabSwitchQtWinAPI();

    void profileLoadLast();
    void profileTraySync();

    void initLanguages();

    void openFile(const QString &filePath);

    void initLabelsForGrabbedColors();

    IGrabber * createGrabber(Grab::Mode grabMode);

private:
    // Main backlight status for all modes (Grab, MoodLamp, etc.)
    Backlight::Status m_backlightStatus;
    Api::DeviceLockStatus m_deviceLockStatus;

    GrabManager *m_grabManager;
    AboutDialog *m_aboutDialog;
    SpeedTest *speedTest;

    Grab::Mode getGrabMode();

    QList<QLabel *> labelsGrabbedColors;

    Ui::SettingsWindow *ui;

    QAction *m_switchOnBacklightAction;
    QAction *m_switchOffBacklightAction;
    QAction *m_settingsAction;
    QAction *m_aboutAction;
    QAction *m_quitAction;

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;
    QMenu *profilesMenu;

    QTranslator *translator;

    static const unsigned ModeAmbilightIndex;
    static const unsigned ModeMoodLampIndex;
};
