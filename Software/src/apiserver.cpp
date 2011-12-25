/*
 * ApiServer.cpp
 *
 *  Created on: 07.09.2011
 *     Authors: Andrey Isupov && Mike Shatohin
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

#include <QtNetwork>
#include <stdlib.h>

#include "ApiServer.hpp"
#include "ApiServerSetColorTask.hpp"
#include "Settings.hpp"
#include "TimeEvaluations.hpp"
#include "version.h"

using namespace SettingsScope;

// Immediatly after successful connection server sends to client -- ApiVersion
const char * ApiServer::ApiVersion = "version:"API_VERSION"\n\r";
const char * ApiServer::CmdUnknown = "unknown command\n\r";
const char * ApiServer::CmdExit = "exit";
const char * ApiServer::CmdHelp = "help";

const char * ApiServer::CmdApiKey = "apikey:";
const char * ApiServer::CmdApiKeyResult_Ok = "ok\n\r";
const char * ApiServer::CmdApiKeyResult_Fail = "fail\n\r";
const char * ApiServer::CmdApiCheck_AuthRequired = "authorization required\n\r";

const char * ApiServer::CmdGetStatus = "getstatus";
const char * ApiServer::CmdResultStatus_On = "status:on\n\r";
const char * ApiServer::CmdResultStatus_Off = "status:off\n\r";
const char * ApiServer::CmdResultStatus_DeviceError = "status:device error\n\r";
const char * ApiServer::CmdResultStatus_Unknown = "status:unknown\n\r";

const char * ApiServer::CmdGetStatusAPI = "getstatusapi";
const char * ApiServer::CmdResultStatusAPI_Busy = "statusapi:busy\n\r";
const char * ApiServer::CmdResultStatusAPI_Idle = "statusapi:idle\n\r";

const char * ApiServer::CmdGetProfiles = "getprofiles";
// Necessary to add a new line after filling results!
const char * ApiServer::CmdResultProfiles = "profiles:";

const char * ApiServer::CmdGetProfile = "getprofile";
// Necessary to add a new line after filling results!
const char * ApiServer::CmdResultProfile = "profile:";

const char * ApiServer::CmdGetCountLeds = "getcountleds";
// Necessary to add a new line after filling results!
const char * ApiServer::CmdResultCountLeds = "getcountleds:";

const char * ApiServer::CmdLock = "lock";
const char * ApiServer::CmdResultLock_Success = "lock:success\n\r";
const char * ApiServer::CmdResultLock_Busy = "lock:busy\n\r";

const char * ApiServer::CmdUnlock = "unlock";
const char * ApiServer::CmdResultUnlock_Success = "unlock:success\n\r";
const char * ApiServer::CmdResultUnlock_NotLocked = "unlock:not locked\n\r";

// Set-commands works only after success lock
// Set-commands can return, after self-name, only this results:
const char * ApiServer::CmdSetResult_Ok = "ok\n\r";
const char * ApiServer::CmdSetResult_Error = "error\n\r";
const char * ApiServer::CmdSetResult_Busy = "busy\n\r";
const char * ApiServer::CmdSetResult_NotLocked = "not locked\n\r";

// Set-commands contains at end semicolon!!!
const char * ApiServer::CmdSetColor = "setcolor:";
const char * ApiServer::CmdSetGamma = "setgamma:";
const char * ApiServer::CmdSetBrightness = "setbrightness:";
const char * ApiServer::CmdSetSmooth = "setsmooth:";
const char * ApiServer::CmdSetProfile = "setprofile:";

const char * ApiServer::CmdSetStatus = "setstatus:";
const char * ApiServer::CmdSetStatus_On = "on";
const char * ApiServer::CmdSetStatus_Off = "off";

const int ApiServer::SignalWaitTimeoutMs = 1000; // 1 second

ApiServer::ApiServer(QObject *parent)
    : QTcpServer(parent)
{    
    initPrivateVariables();
    initApiSetColorTask();
    initHelpMessage();
}

ApiServer::ApiServer(quint16 port, QObject *parent)
    : QTcpServer(parent)
{
    // This constructor is for using in ApiTests

    initPrivateVariables();
    initApiSetColorTask();
    initHelpMessage();

    m_apiPort = port;

    bool ok = listen(QHostAddress::Any, m_apiPort);
    if (!ok)
    {
        qFatal("%s listen(Any, %d) fail", Q_FUNC_INFO, m_apiPort);
    }
}

void ApiServer::firstStart()
{
    // Call this function after connect all nesessary signals/slots

    if (Settings::isApiEnabled())
    {
        startListening();
    }
}

void ApiServer::enableApiServer(bool isEnabled)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isEnabled;

    if (isEnabled)
        startListening();
    else
        stopListening();
}

void ApiServer::enableApiAuth(bool isEnabled)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isEnabled;

    m_isAuthEnabled = isEnabled;
}

void ApiServer::updateApiPort(int port)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << port;

    m_apiPort = port;

    stopListening();
    startListening();
}

void ApiServer::updateApiKey(QString key)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << key;

    m_apiAuthKey = key;
}

void ApiServer::incomingConnection(int socketDescriptor)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketDescriptor);

    ClientInfo cs;
    cs.isAuthorized = false;

    m_clients.insert(client, cs);

    client->write(ApiVersion);

    DEBUG_LOW_LEVEL << "Incoming connection from:" << client->peerAddress().toString();

    connect(client, SIGNAL(readyRead()), this, SLOT(clientProcessCommands()));
    connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
}

void ApiServer::clientDisconnected()
{
    QTcpSocket *client = dynamic_cast<QTcpSocket*>(sender());

    DEBUG_LOW_LEVEL << "Client disconnected:" << client->peerAddress().toString();

    if (m_lockedClient == client)
    {
        m_lockedClient = NULL;

        emit updateDeviceLockStatus(Api::DeviceUnlocked);
    }
    m_clients.remove(client);

    disconnect(client, SIGNAL(readyRead()), this, SLOT(clientProcessCommands()));
    disconnect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));

    client->deleteLater();
}

void ApiServer::clientProcessCommands()
{
    API_DEBUG_OUT << Q_FUNC_INFO << "ApiServer thread id:" << this->thread()->currentThreadId();

    QTcpSocket *client = dynamic_cast<QTcpSocket*>(sender());

    while (m_clients.contains(client) && client->canReadLine())
    {
        QByteArray cmdBuffer = client->readLine().trimmed();
        API_DEBUG_OUT << cmdBuffer;

        QString result = CmdUnknown;

        if (cmdBuffer.startsWith(CmdApiKey))
        {
            API_DEBUG_OUT << CmdApiKey;

            if (m_isAuthEnabled)
            {
                cmdBuffer.remove(0, cmdBuffer.indexOf(':') + 1);
                API_DEBUG_OUT << QString(cmdBuffer);

                if (cmdBuffer == m_apiAuthKey)
                {
                    API_DEBUG_OUT << CmdApiKey << "OK";

                    m_clients[client].isAuthorized = true;

                    result = CmdApiKeyResult_Ok;
                } else {
                    API_DEBUG_OUT << CmdApiKey << "Api key is not valid:" << QString(cmdBuffer);

                    m_clients[client].isAuthorized = false;

                    result = CmdApiKeyResult_Fail;
                }
            }
            else
            {
                API_DEBUG_OUT << CmdApiKey << "Authorization is disabled, return ok;";
                result = CmdApiKeyResult_Ok;
            }

            writeData(client, result);
            return;
        }

        if (m_isAuthEnabled && m_clients[client].isAuthorized == false)
        {
            writeData(client, CmdApiCheck_AuthRequired);
            return;
        }

        // We are working only with authorized clients!

        if (cmdBuffer == CmdGetStatus)
        {
            API_DEBUG_OUT << CmdGetStatus;

            if (m_isRequestBacklightStatusDone)
            {
                m_isRequestBacklightStatusDone = false;
                m_backlightStatusResult = Backlight::StatusUnknown;

                emit requestBacklightStatus();

                // Wait signal from SettingsWindow with status of backlight
                // or if timeout -- result will be unknown
                m_time.restart();
                while (m_isRequestBacklightStatusDone == false && m_time.elapsed() < SignalWaitTimeoutMs)
                {
                    QApplication::processEvents(QEventLoop::WaitForMoreEvents, SignalWaitTimeoutMs);
                }

                if (m_isRequestBacklightStatusDone)
                {
                    switch (m_backlightStatusResult)
                    {
                    case Backlight::StatusOn:
                        result = CmdResultStatus_On;
                        break;
                    case Backlight::StatusOff:
                        result = CmdResultStatus_Off;
                        break;
                    case Backlight::StatusDeviceError:
                        result = CmdResultStatus_DeviceError;
                        break;
                    default:
                        result = CmdResultStatus_Unknown;
                        break;
                    }
                } else {
                    m_isRequestBacklightStatusDone = true;
                    result = CmdResultStatus_Unknown;
                    qWarning() << Q_FUNC_INFO << "Timeout waiting resultBacklightStatus() signal from SettingsWindow";
                }
            }
        }
        else if (cmdBuffer == CmdGetStatusAPI)
        {
            API_DEBUG_OUT << CmdGetStatusAPI;

            if (m_lockedClient != NULL)
                result = CmdResultStatusAPI_Busy;
            else
                result = CmdResultStatusAPI_Idle;
        }
        else if (cmdBuffer == CmdGetProfiles)
        {
            API_DEBUG_OUT << CmdGetProfiles;

            QStringList profiles = Settings::findAllProfiles();

            result = ApiServer::CmdResultProfiles;

            for (int i = 0; i < profiles.count(); i++)
                result += profiles[i] + ";";
            result += "\n\r";
        }
        else if (cmdBuffer == CmdGetProfile)
        {
            API_DEBUG_OUT << CmdGetProfile;

            result = CmdResultProfile + Settings::getCurrentProfileName() + "\n\r";
        }
        else if (cmdBuffer == CmdGetCountLeds)
        {
            API_DEBUG_OUT << CmdGetCountLeds;

            result = QString("%1%2\n\r").arg(CmdResultCountLeds).arg(Settings::getNumberOfLeds(Settings::getConnectedDevice()));
        }
        else if (cmdBuffer == CmdLock)
        {
            API_DEBUG_OUT << CmdLock;

            if (m_lockedClient == NULL)
            {
                m_lockedClient = client;

                emit updateDeviceLockStatus(Api::DeviceLocked);

                result = CmdResultLock_Success;                
            } else {
                if (m_lockedClient == client)
                {
                    result = CmdResultLock_Success;
                } else {
                    result = CmdResultLock_Busy;
                }
            }
        }
        else if (cmdBuffer == CmdUnlock)
        {
            API_DEBUG_OUT << CmdUnlock;

            if (m_lockedClient == NULL)
            {
                result = CmdResultUnlock_NotLocked;
            } else {
                if (m_lockedClient == client)
                {
                    m_lockedClient = NULL;
                    emit updateDeviceLockStatus(Api::DeviceUnlocked);
                }
                result = CmdResultUnlock_Success;               
            }
        }
        else if (cmdBuffer.startsWith(CmdSetColor))
        {
            API_DEBUG_OUT << CmdSetColor;

            if (m_lockedClient == client)
            {
                cmdBuffer.remove(0, cmdBuffer.indexOf(':') + 1);
                API_DEBUG_OUT << QString(cmdBuffer);

                if (m_isTaskSetColorDone)
                {
                    m_isTaskSetColorDone = false;
                    m_isTaskSetColorParseSuccess = false;

                    // Start task
                    emit startParseSetColorTask(cmdBuffer);

                    // Wait signal from m_apiSetColorTask with success or fail result of parsing buffer.
                    // After SignalWaitTimeoutMs milliseconds, the cycle of waiting will end and the
                    // variable m_isTaskSetColorDone will be reset to 'true' state.
                    // Also in cycle we process requests from over clients, if this request is setcolor when
                    // it will be ignored because of m_isTaskSetColorDone == false

                    m_time.restart();

                    while (m_isTaskSetColorDone == false && m_time.elapsed() < SignalWaitTimeoutMs)
                    {
                        QApplication::processEvents(QEventLoop::WaitForMoreEvents, SignalWaitTimeoutMs);
                    }

                    if (m_isTaskSetColorDone)
                    {
                        if (m_isTaskSetColorParseSuccess)
                            result = CmdSetResult_Ok;
                        else
                            result = CmdSetResult_Error;
                    } else {
                        m_isTaskSetColorDone = true; // cmd setcolor is available
                        result = CmdSetResult_Error;
                        qWarning() << Q_FUNC_INFO << "Timeout waiting taskIsSuccess() signal from m_apiSetColorTask";
                    }
                } else {
                    qWarning() << Q_FUNC_INFO << "Task setcolor is not completed (you should increase the delay to not skip commands), skip setcolor.";
                }
            }
            else if (m_lockedClient == NULL)
            {
                result = CmdSetResult_NotLocked;
            }
            else // m_lockedClient != client
            {
                result = CmdSetResult_Busy;
            }
        }
        else if (cmdBuffer.startsWith(CmdSetGamma))
        {
            API_DEBUG_OUT << CmdSetGamma;

            if (m_lockedClient == client)
            {
                cmdBuffer.remove(0, cmdBuffer.indexOf(':') + 1);
                API_DEBUG_OUT << QString(cmdBuffer);

                // Gamma can contain max five chars (0.00 -- 10.00)
                if (cmdBuffer.length() > 5)
                {
                    API_DEBUG_OUT << CmdSetGamma << "Error (gamma max 5 chars)";
                    result = CmdSetResult_Error;
                } else {
                    // Try to convert gamma string to double
                    bool ok = false;
                    double gamma = QString(cmdBuffer).toDouble(&ok);

                    if (ok)
                    {
                        if (gamma >= Profile::Device::GammaMin && gamma <= Profile::Device::GammaMax)
                        {
                            API_DEBUG_OUT << CmdSetGamma << "OK:" << gamma;

                            emit updateGamma(gamma);

                            result = CmdSetResult_Ok;
                        } else {
                            API_DEBUG_OUT << CmdSetGamma << "Error (max min test fail):" << gamma;
                            result = CmdSetResult_Error;
                        }
                    } else {
                        API_DEBUG_OUT << CmdSetGamma << "Error (convert fail):" << gamma;
                        result = CmdSetResult_Error;
                    }
                }
            }
            else if (m_lockedClient == NULL)
            {
                result = CmdSetResult_NotLocked;
            }
            else // m_lockedClient != client
            {
                result = CmdSetResult_Busy;
            }
        }
        else if (cmdBuffer.startsWith(CmdSetBrightness))
        {
            API_DEBUG_OUT << CmdSetBrightness;

            if (m_lockedClient == client)
            {
                cmdBuffer.remove(0, cmdBuffer.indexOf(':') + 1);
                API_DEBUG_OUT << QString(cmdBuffer);

                // Brightness can contain max three chars (0 -- 100)
                if (cmdBuffer.length() > 3)
                {
                    API_DEBUG_OUT << CmdSetBrightness << "Error (smooth max 3 chars)";
                    result = CmdSetResult_Error;
                } else {
                    // Try to convert smooth string to int
                    bool ok = false;
                    int brightness = QString(cmdBuffer).toInt(&ok);

                    if (ok)
                    {
                        if (brightness >= Profile::Device::BrightnessMin && brightness <= Profile::Device::BrightnessMax)
                        {
                            API_DEBUG_OUT << CmdSetBrightness << "OK:" << brightness;

                            emit updateBrightness(brightness);

                            result = CmdSetResult_Ok;
                        } else {
                            API_DEBUG_OUT << CmdSetBrightness << "Error (max min test fail):" << brightness;
                            result = CmdSetResult_Error;
                        }
                    } else {
                        API_DEBUG_OUT << CmdSetBrightness << "Error (convert fail):" << brightness;
                        result = CmdSetResult_Error;
                    }
                }
            }
            else if (m_lockedClient == NULL)
            {
                result = CmdSetResult_NotLocked;
            }
            else // m_lockedClient != client
            {
                result = CmdSetResult_Busy;
            }
        }
        else if (cmdBuffer.startsWith(CmdSetSmooth))
        {
            API_DEBUG_OUT << CmdSetSmooth;

            if (m_lockedClient == client)
            {
                cmdBuffer.remove(0, cmdBuffer.indexOf(':') + 1);
                API_DEBUG_OUT << QString(cmdBuffer);

                // Smooth can contain max three chars (0 -- 255)
                if (cmdBuffer.length() > 3)
                {
                    API_DEBUG_OUT << CmdSetSmooth << "Error (smooth max 3 chars)";
                    result = CmdSetResult_Error;
                } else {
                    // Try to convert smooth string to int
                    bool ok = false;
                    int smooth = QString(cmdBuffer).toInt(&ok);

                    if (ok)
                    {
                        if (smooth >= Profile::Device::SmoothMin && smooth <= Profile::Device::SmoothMax)
                        {
                            API_DEBUG_OUT << CmdSetSmooth << "OK:" << smooth;

                            emit updateSmooth(smooth);

                            result = CmdSetResult_Ok;
                        } else {
                            API_DEBUG_OUT << CmdSetSmooth << "Error (max min test fail):" << smooth;
                            result = CmdSetResult_Error;
                        }
                    } else {
                        API_DEBUG_OUT << CmdSetSmooth << "Error (convert fail):" << smooth;
                        result = CmdSetResult_Error;
                    }
                }
            }
            else if (m_lockedClient == NULL)
            {
                result = CmdSetResult_NotLocked;
            }
            else // m_lockedClient != client
            {
                result = CmdSetResult_Busy;
            }
        }
        else if (cmdBuffer.startsWith(CmdSetProfile))
        {
            API_DEBUG_OUT << CmdSetProfile;

            if (m_lockedClient == client)
            {
                cmdBuffer.remove(0, cmdBuffer.indexOf(':') + 1);
                API_DEBUG_OUT << QString(cmdBuffer);

                QString setProfileName = QString(cmdBuffer);
                QStringList profiles = Settings::findAllProfiles();

                if (profiles.contains(setProfileName))
                {
                    API_DEBUG_OUT << CmdSetProfile << "OK:" << setProfileName;

                    emit updateProfile(setProfileName);

                    result = CmdSetResult_Ok;
                } else {
                    API_DEBUG_OUT << CmdSetProfile << "Error (profile not found):" << setProfileName;
                    result = CmdSetResult_Error;
                }
            }
            else if (m_lockedClient == NULL)
            {
                result = CmdSetResult_NotLocked;
            }
            else // m_lockedClient != client
            {
                result = CmdSetResult_Busy;
            }
        }
        else if (cmdBuffer.startsWith(CmdSetStatus))
        {
            API_DEBUG_OUT << CmdSetStatus;

            if (m_lockedClient == client)
            {
                cmdBuffer.remove(0, cmdBuffer.indexOf(':') + 1);
                API_DEBUG_OUT << QString(cmdBuffer);

                Backlight::Status status = Backlight::StatusUnknown;

                if (cmdBuffer == CmdSetStatus_On)
                    status = Backlight::StatusOn;
                else if (cmdBuffer == CmdSetStatus_Off)
                    status = Backlight::StatusOff;

                if (status != Backlight::StatusUnknown)
                {
                    API_DEBUG_OUT << CmdSetStatus << "OK:" << status;

                    emit updateStatus(status);

                    result = CmdSetResult_Ok;
                } else {
                    API_DEBUG_OUT << CmdSetStatus << "Error (status not recognized):" << status;
                    result = CmdSetResult_Error;
                }
            }
            else if (m_lockedClient == NULL)
            {
                result = CmdSetResult_NotLocked;
            }
            else // m_lockedClient != client
            {
                result = CmdSetResult_Busy;
            }
        }
        else if (cmdBuffer == CmdExit)
        {
            writeData(client, "Goodbye!\n");
            if (m_clients.contains(client))
                client->close();
            return;
        }
        else if (cmdBuffer == CmdHelp)
        {
            writeData(client, m_helpMessage);
            result = "";
        }
        else            
        {
            qWarning() << Q_FUNC_INFO << CmdUnknown << cmdBuffer;
        }

        writeData(client, result);
    }
}

void ApiServer::taskSetColorIsSuccess(bool isSuccess)
{
    m_isTaskSetColorDone = true;
    m_isTaskSetColorParseSuccess = isSuccess;
}

void ApiServer::resultBacklightStatus(Backlight::Status status)
{
    m_isRequestBacklightStatusDone = true;
    m_backlightStatusResult = status;
}

void ApiServer::initPrivateVariables()
{
    m_apiPort = Settings::getApiPort();
    m_apiAuthKey = Settings::getApiAuthKey();
    m_isAuthEnabled = Settings::isApiAuthEnabled();

    m_lockedClient = NULL;
    m_isRequestBacklightStatusDone = true;
    m_backlightStatusResult = Backlight::StatusUnknown;
}

void ApiServer::initApiSetColorTask()
{
    m_isTaskSetColorDone = true;

    m_apiSetColorTaskThread = new QThread();
    m_apiSetColorTask = new ApiServerSetColorTask();

    connect(m_apiSetColorTask, SIGNAL(taskParseSetColorDone(QList<QRgb>)), this, SIGNAL(updateLedsColors(QList<QRgb>)), Qt::QueuedConnection);

    connect(m_apiSetColorTask, SIGNAL(taskParseSetColorIsSuccess(bool)), this, SLOT(taskSetColorIsSuccess(bool)), Qt::QueuedConnection);

    connect(this, SIGNAL(startParseSetColorTask(QByteArray)), m_apiSetColorTask, SLOT(startParseSetColorTask(QByteArray)), Qt::QueuedConnection);
    connect(this, SIGNAL(updateApiDeviceNumberOfLeds(int)),   m_apiSetColorTask, SLOT(setApiDeviceNumberOfLeds(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(clearColorBuffers()),                m_apiSetColorTask, SLOT(reinitColorBuffers()));

    m_apiSetColorTask->moveToThread(m_apiSetColorTaskThread);
    m_apiSetColorTaskThread->start();
}

void ApiServer::startListening()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << m_apiPort;

    bool ok = listen(QHostAddress::Any, m_apiPort);

    if (ok == false)
    {
        QString errorStr = tr("API server unable to start (port: %1): %2.")
                .arg(m_apiPort).arg(errorString());

        qCritical() << Q_FUNC_INFO << errorStr;

        emit errorOnStartListening(errorStr);
    }
}

void ApiServer::stopListening()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    // Closes the server. The server will no longer listen for incoming connections.
    close();

    m_lockedClient = NULL;

    emit updateDeviceLockStatus(Api::DeviceUnlocked);

    QMap<QTcpSocket*, ClientInfo>::iterator i;
    for (i = m_clients.begin(); i != m_clients.end(); ++i)
    {
        QTcpSocket * client = dynamic_cast<QTcpSocket*>(i.key());

        disconnect(client, SIGNAL(readyRead()), this, SLOT(clientProcessCommands()));
        disconnect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));

        client->abort();
        client->deleteLater();
    }

    m_clients.clear();
}

void ApiServer::writeData(QTcpSocket* client, const QString & data)
{
    if (m_clients.contains(client) == false)
    {
        API_DEBUG_OUT << Q_FUNC_INFO << "client disconected, cancel writing buffer = " << data;
        return;
    }

    API_DEBUG_OUT << Q_FUNC_INFO << data;
    client->write(data.toUtf8());
}

QString ApiServer::formatHelp(const QString & cmd)
{
    return QString("\t\t \"%1\" \n\r").arg(cmd.trimmed());
}

QString ApiServer::formatHelp(const QString & cmd, const QString & description)
{
    return QString(
                QString(80,'.') + "\n\r"
                "%1 \n\r"             // Command
                "\t %2 \n\r"          // Description
                ).arg(cmd).arg(description);
}

QString ApiServer::formatHelp(const QString & cmd, const QString & description, const QString & results)
{
    return QString(
                formatHelp(cmd, description) +
                "\t Results: \n\r"    // Return values
                "%1"
                ).arg(results);
}

QString ApiServer::formatHelp(const QString & cmd, const QString & description, const QString & examples, const QString & results)
{
    return QString(
                formatHelp(cmd, description) +
                "\t Examples: \n\r"
                "%1"
                "\t Results: \n\r"    // Return values
                "%2"
                ).arg(examples).arg(results);
}

void ApiServer::initHelpMessage()
{
    m_helpMessage += "\n\r";
    m_helpMessage += "Lightpack " VERSION_STR ". API Server " API_VERSION "\n\r";
    m_helpMessage += "\n\r";

    m_helpMessage += formatHelp(
                CmdApiKey,
                "Command for enter an authorization key (see key in GUI)",
                formatHelp(CmdApiKey + QString("{1ccf5dca-119d-45a0-a683-7d90a00c418f}")) +
                formatHelp(CmdApiKey + QString("IDDQD")),
                formatHelp(CmdApiKeyResult_Ok) +
                formatHelp(CmdApiKeyResult_Fail)
                );

    m_helpMessage += formatHelp(
                CmdLock,
                "Opens access to set-commands, If success - suspends capture and blocking access for other clients to set-commands.",
                formatHelp(CmdResultLock_Success) +
                formatHelp(CmdResultLock_Busy)
                );

    m_helpMessage += formatHelp(
                CmdUnlock,
                "Closes access to set-commands. Restores device settings from the current profile, and continues the normal execution of the application.",
                formatHelp(CmdResultUnlock_Success) +
                formatHelp(CmdResultUnlock_NotLocked)
                );

    // Get-commands
    m_helpMessage += formatHelp(
                CmdGetStatus,
                "Get status of the backlight",
                formatHelp(CmdResultStatus_On) +
                formatHelp(CmdResultStatus_Off) +
                formatHelp(CmdResultStatus_DeviceError) +
                formatHelp(CmdResultStatus_Unknown)
                );
    m_helpMessage += formatHelp(
                CmdGetStatusAPI,
                "Get status of the lightpack API",
                formatHelp(CmdResultStatusAPI_Busy) +
                formatHelp(CmdResultStatusAPI_Idle)
                );
    m_helpMessage += formatHelp(
                CmdGetProfile,
                "Get the name of the current profile",
                formatHelp(CmdResultProfile + QString("SampleProfileName"))
                );
    m_helpMessage += formatHelp(
                CmdGetProfiles,
                "Get names of the all available profiles",
                formatHelp(CmdResultProfiles + QString("Lightpack;New profile 1;New profile 2;"))
                );
    m_helpMessage += formatHelp(
                CmdGetCountLeds,
                "Get count leds of the current profile",
                formatHelp(CmdResultCountLeds + QString("10"))
                );

    // Set-commands

    QString helpCmdSetResults =
            formatHelp(CmdSetResult_Ok) +
            formatHelp(CmdSetResult_Error) +
            formatHelp(CmdSetResult_Busy) +
            formatHelp(CmdSetResult_NotLocked);

    m_helpMessage += formatHelp(
                CmdSetColor,
                "Set colors on several LEDs. Format: \"N-R,G,B;\", where N - number of led, R, G, B - red, green and blue color components. Works only on locking time (see lock).",
                formatHelp(CmdSetColor + QString("1-255,255,30;")) +
                formatHelp(CmdSetColor + QString("1-255,255,30;2-12,12,12;3-1,2,3;")),
                helpCmdSetResults);

    m_helpMessage += formatHelp(
                CmdSetGamma,
                QString("Set device gamma correction value [%1 - %2]. Works only on locking time (see lock).")
                .arg(SettingsScope::Profile::Device::GammaMin)
                .arg(SettingsScope::Profile::Device::GammaMax),
                formatHelp(CmdSetGamma + QString("2.5")),
                helpCmdSetResults);

    m_helpMessage += formatHelp(
                CmdSetBrightness,
                QString("Set device brightness value [%1 - %2]. Works only on locking time (see lock).")
                .arg(SettingsScope::Profile::Device::BrightnessMin)
                .arg(SettingsScope::Profile::Device::BrightnessMax),
                formatHelp(CmdSetBrightness + QString("0")) +
                formatHelp(CmdSetBrightness + QString("93")),
                helpCmdSetResults);

    m_helpMessage += formatHelp(
                CmdSetSmooth,
                QString("Set device smooth value [%1 - %2]. Works only on locking time (see lock).")
                .arg(SettingsScope::Profile::Device::SmoothMin)
                .arg(SettingsScope::Profile::Device::SmoothMax),
                formatHelp(CmdSetSmooth + QString("10")) +
                formatHelp(CmdSetSmooth + QString("128")),
                helpCmdSetResults);

    m_helpMessage += formatHelp(
                CmdSetProfile,
                QString("Set current profile. Works only on locking time (see lock)."),
                formatHelp(CmdSetProfile + QString("Lightpack")) +
                formatHelp(CmdSetProfile + QString("16x9")),
                helpCmdSetResults);

    m_helpMessage += formatHelp(
                CmdSetStatus,
                QString("Set backlight status. Works only on locking time (see lock)."),
                formatHelp(CmdSetStatus + QString(CmdSetStatus_On)) +
                formatHelp(CmdSetStatus + QString(CmdSetStatus_Off)),
                helpCmdSetResults);

    m_helpMessage += formatHelp(CmdExit, "Closes connection");

    m_helpMessage += "\n";
}
