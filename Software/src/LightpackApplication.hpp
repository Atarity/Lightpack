/*
 * LightpackApplication.hpp
 *
 *  Created on: 06.09.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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

#include <QApplication>
#include "SettingsWindow.hpp"
#include "ApiServer.hpp"
#include "LedDeviceFactory.hpp"

class LightpackApplication : public QApplication
{
    Q_OBJECT
public:
    LightpackApplication(const QString & appDirPath, int &argc, char **argv);

    enum ErrorCodes {
        OK_ErrorCode = 0,
        WrongCommandLineArgument_ErrorCode,
        AppDirectoryCreationFail_ErrorCode,
        OpenLogsFail_ErrorCode,
        QFatalMessageHandler_ErrorCode,
        // Append new ErrorCodes here
        JustEpicFail_ErrorCode = 93
    };

signals:
    void clearColorBuffers();
public slots:

private slots:
    void backlightStatusChanged(Backlight::Status);
private:
    void processCommandLineArguments();
    void printHelpMessage() const;
    void printVersionsSoftwareQtOS() const;
    void checkSystemTrayAvailability() const;
    void startApiServer();
    void startLedDeviceFactory();
    void connectApiServerAndLedDeviceSignalsSlots();
    void disconnectApiServerAndLedDeviceSignalsSlots();

private:
    SettingsWindow *m_settingsWindow;
    ApiServer *m_apiServer;
    LedDeviceFactory *m_ledDeviceFactory;
    QThread *m_ledDeviceFactoryThread;
    QThread *m_apiServerThread;

    QString m_applicationDirPath;
    bool m_isDebugLevelObtainedFromCmdArgs;
    bool m_isApiServerConnectedToLedDeviceSignalsSlots;
};
