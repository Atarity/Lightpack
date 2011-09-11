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

namespace Ui {
    class SettingsWindow;
}

class SettingsWindow : public QMainWindow {
    Q_OBJECT
public:
    SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

public:
    enum AppMainStatus {
        Status_Off = 0,
        Status_On = -1,
        Status_DeviceError = -2
    };

public:
    void startAmbilight();

signals:
    void settingsProfileChanged();
    void updateLedsColors(const QList<QRgb> &);
    void updateTimerOptions(int prescallerIndex, int outputCompareRegValue);
    void updateColorDepth(int value);
    void updateSmoothSlowdown(int value);
    void requestFirmwareVersion();
    void recreateLedDevice();


public slots:
    void ledDeviceCallSuccess(bool isSuccess);
    void ledDeviceGetFirmwareVersion(const QString & fwVersion);
    void refreshAmbilightEvaluated(double updateResultMs);
    void ambilightOn(); /* using in actions */
    void ambilightOff(); /* using in actions */
    void profilesLoadAll();
    void profileSwitch(const QString & configName);
    void profileSwitchCombobox(QString profile);
    void updateGrabbedColors(const QList<QRgb> & colors);



protected:
    virtual void changeEvent(QEvent *e);
    virtual void closeEvent(QCloseEvent *event);
    virtual void paintEvent(QPaintEvent * event);

private slots:
    void on_horizontalSlider_Speed_valueChanged(int value);
    void on_horizontalSlider_Brightness_valueChanged(int value);
    void onMoodLampModeChanged(bool isConstantColor);
    void onCbModesChanged(int index);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showAbout(); /* using in actions */
    void showSettings(); /* using in actions */
    void hideSettings(); /* using in iconActivated(..) */

    void quit(); /* using in actions */

    void grabAmbilightOnOff();

    void settingsHardwareTimerOptionsChange();
    void settingsHardwareSetColorDepth(int value);
    void settingsHardwareSetSmoothSlowdown(int value);
    void settingsHardwareSetBrightness(int value);

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

    void setAvgColorOnAllLEDs(int value);

    void onMoodLampColorChanged(QColor color);
    void onExpertModeEnabledChanged(bool isEnabled);
    void onCheckBox_ConnectVirtualDeviceToggled(bool isEnabled);

private:
    void connectSignalsSlots();
//    void connectLedDeviceSignalsSlots();
//    void disconnectLedDeviceSignalsSlots();

    void updateTrayAndActionStates();
    void updateExpertModeWidgetsVisibility();

    void createTrayIcon();
    void createActions();
    void loadSettingsToMainWindow();

    void grabSwitchQtWinAPI();

    void profileLoadLast();
    void profileTraySync();

    void initLanguages();

    void openFile(const QString &filePath);

    void updatePwmFrequency();

    void initLabelsForGrabbedColors();

    void updateCbModesPosition();

    IGrabber * createGrabber(GrabMode grabMode);

public:
    bool m_isAmbilightOn; /* is grab desktop window ON */
    GrabManager *m_grabManager;
//    ILedDevice *ledDevice;

private:

    AboutDialog *m_aboutDialog;
    SpeedTest *speedTest;   


    bool m_isErrorState;

    GrabMode getGrabMode();

    // Evaluated frequency of the PWM generation
    double pwmFrequency;

    QList<QLabel *> labelsGrabbedColors;

    Ui::SettingsWindow *ui;

    QAction *m_onAmbilightAction;
    QAction *m_offAmbilightAction;
    QAction *m_settingsAction;
    QAction *m_aboutAction;
    QAction *m_quitAction;

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;
    QMenu *profilesMenu;

    QTranslator *translator;
};
