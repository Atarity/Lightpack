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
#include "Settings.hpp"
#include "GrabManager.hpp"
#include "MoodLampManager.hpp"
#include "SpeedTest.hpp"
#include "ColorButton.hpp"
#include "enums.hpp"


#include "hotkeys/qkeysequencewidget/src/qkeysequencewidget.h"

namespace Ui {
    class SettingsWindow;
}

class GrabManager; // forward declaration
class PyPlugin;

class SettingsWindow : public QMainWindow {
    Q_OBJECT
public:
    SettingsWindow(QWidget *parent = 0);
    SettingsWindow(bool noGUI);
    ~SettingsWindow();

public:
    void startBacklight();
    void createTrayIcon();
    void connectSignalsSlots();
    QWidget* getSettingBox();

signals:
    void switchOffLeds();
    void switchOnLeds();
    void showLedWidgets(bool visible);
    void setColoredLedWidget(bool colored);
    void updateLedsColors(const QList<QRgb> &);
    void updateRefreshDelay(int value);
    void updateColorDepth(int value);
    void updateSmoothSlowdown(int value);
    void updateSlowdown(int value);
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
    void updateApiDeviceNumberOfLeds(int value);
    void getPluginConsole();
    void reloadPlugins();

public slots:
    void ledDeviceOpenSuccess(bool isSuccess);
    void ledDeviceCallSuccess(bool isSuccess);
    void ledDeviceFirmwareVersionResult(const QString & fwVersion);
    void refreshAmbilightEvaluated(double updateResultMs);

    void setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus status,  QList<QString> modules);
    void setBacklightStatus(Backlight::Status);
    void setModeChanged(Lightpack::Mode);
    void backlightOn(); /* using in actions */
    void backlightOff(); /* using in actions */
    void profilesLoadAll();
    void profileSwitch(const QString & configName);
    void handleProfileLoaded(const QString & configName);
    void profileSwitchCombobox(QString profile);
    void updateVirtualLedsColors(const QList<QRgb> & colors);
    void requestBacklightStatus();
    void onApiServer_ErrorOnStartListening(QString errorMessage);
    void onPingDeviceEverySecond_Toggled(bool state);
    void processMessage(const QString &message);

    void updatePlugin(QList<PyPlugin*> plugins);


protected:
    virtual void changeEvent(QEvent *e);
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void onLightpackModes_Activated(int index);
    void onMoodLampColor_changed(QColor color);
    void onMoodLampSpeed_valueChanged(int value);
    void onMoodLampLiquidMode_Toggled(bool isConstantColor);
    void onTrayIcon_Activated(QSystemTrayIcon::ActivationReason reason);
    void onTrayIcon_MessageClicked();
    void showAbout(); /* using in actions */
    void showSettings(); /* using in actions */
    void hideSettings(); /* using in iconActivated(..) */
    void onPostInit();

    void changePage(int page);

    void quit(); /* using in actions */

    void toggleBacklight();
    void nextProfile();
    void prevProfile();

    void onGrabberChanged();
    void onGrabSlowdown_valueChanged(int value);
    void onGrabMinLevelOfSensivity_valueChanged(int value);
    void onGrabIsAvgColors_toggled(bool state);

    void onDeviceRefreshDelay_valueChanged(int value);
    void onDeviceSmooth_valueChanged(int value);
    void onDeviceBrightness_valueChanged(int value);
    void onDeviceColorDepth_valueChanged(int value);
    void onDeviceConnectedDevice_currentIndexChanged(QString value);
    void onLightpackNumberOfLeds_valueChanged(int value);
    void onAdalightNumberOfLeds_valueChanged(int value);
    void onArdulightNumberOfLeds_valueChanged(int value);
    void onVirtualNumberOfLeds_valueChanged(int value);
    void onAdalightSerialPort_editingFinished();
    void onAdalightSerialPortBaudRate_valueChanged(QString value);
    void onArdulightSerialPort_editingFinished();
    void onArdulightSerialPortBaudRate_valueChanged(QString value);
    void onDeviceGammaCorrection_valueChanged(double value);
    void onSliderDeviceGammaCorrection_valueChanged(int value);
    void onDeviceSendDataOnlyIfColorsChanged_toggled(bool state);
    void onColorSequence_valueChanged(QString value);
    void onDx1011CaptureEnabledChanged(bool isEnabled);

    void onShowLedWidgets_Toggled(bool checked);
    void onSetColoredLedWidgets();
    void onSetWhiteLedWidgets();

    void openCurrentProfile();

    void profileRename();
    void profileTraySwitch();
    void profileNew();
    void profileResetToDefaultCurrent();
    void profileDeleteCurrent();
    void settingsProfileChanged_UpdateUI(const QString &profileName);

    void loadTranslation(const QString & language);

    void startTestsClick();

    void onExpertModeEnabled_Toggled(bool isEnabled);
    void onSwitchOffAtClosing_Toggled(bool isEnabled);
    void onEnableApi_Toggled(bool isEnabled);
    void onApiKey_EditingFinished();
    void onGenerateNewApiKey_Clicked();
    void onSetApiPort_Clicked();
    void onLoggingLevel_valueChanged(int value);

    void pluginSwitch(int index);
    void viewPluginConsole();

    void on_list_Plugins_itemClicked(QListWidgetItem*);
    void on_pushButton_clicked();
    void MoveUpPlugin();
    void MoveDownPlugin();

    void on_pushButton_LightpackSmoothnessHelp_clicked();
    void on_pushButton_LightpackColorDepthHelp_clicked();
    void on_pushButton_LightpackRefreshDelayHelp_clicked();

    void on_pushButton_GammaCorrectionHelp_clicked();
    void on_tableWidget_Hotkeys_itemSelectionChanged();
    void onKeySequenceChanged(const QKeySequence &sequence);
    void onHotkeyChanged(const QString &actionName, const QKeySequence &sequence, const QKeySequence &oldKeySequence);

private:
    void updateTrayAndActionStates();    
    void updateExpertModeWidgetsVisibility();
    void updateDeviceTabWidgetsVisibility();
    void setDeviceTabWidgetsVisibility(DeviceTab::Options options);
    void syncLedDeviceWithSettingsWindow();
    void setMaximumNumberOfLeds(MaximumNumberOfLeds::Devices maximumNumberOfLeds);
    MaximumNumberOfLeds::Devices getLightpackMaximumNumberOfLeds();
    int getLigtpackFirmwareVersionMajor();


    void createActions();
    void updateUiFromSettings();

    void profileTraySync();

    void initLanguages();

    void openFile(const QString &filePath);

    void initGrabbersRadioButtonsVisibility();
    void initVirtualLeds(int ledsCount);
    void initConnectedDeviceComboBox();
    void initSerialPortBaudRateComboBox();    

    void adjustSizeAndMoveCenter();

    void setupHotkeys();
    void registerHotkey(const QString &actionName, const QString &description, const QString &hotkey);

    void setFirmwareVersion(const QString &firmwareVersion);
    void versionsUpdate();

    void savePriorityPlugin();
    void showHelpOf(QObject *object);
    QString getSlotName(const QString &actionName);


private:
    Ui::SettingsWindow *ui;
    // Main backlight status for all modes (Grab, MoodLamp, etc.)
    Backlight::Status m_backlightStatus;
    DeviceLocked::DeviceLockStatus m_deviceLockStatus;
    QList<QString> m_deviceLockKey;
    QString m_deviceLockModule;
    Lightpack::Mode m_lightpackMode;

    SpeedTest *m_speedTest;

    Grab::GrabberType getSelectedGrabberType();

    bool isDx1011CaptureEnabled();

    QList<QLabel *> m_labelsGrabbedColors;

    QAction *m_switchOnBacklightAction;
    QAction *m_switchOffBacklightAction;
    QAction *m_settingsAction;
    QAction *m_quitAction;

    bool m_isHotkeySelectionChanging;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;
    QMenu *m_profilesMenu;

    QLabel *labelProfile;
    QLabel *labelDevice;
    QLabel *labelFPS;

    enum TrayMessages
    {
        Tray_UpdateFirmwareMessage,
        Tray_AnotherInstanceMessage
    } m_trayMessage;

    QTranslator *m_translator;

    QString m_deviceFirmwareVersion;
    static const QString DeviceFirmvareVersionUndef;
    static const QString LightpackDownloadsPageUrl;
    static const unsigned AmbilightModeIndex;
    static const unsigned MoodLampModeIndex;

    QKeySequenceWidget *m_keySequenceWidget;

    QString fimwareVersion;

    QList<PyPlugin*> _plugins;
    static bool toPriority(PyPlugin* s1 , PyPlugin* s2 );
};


