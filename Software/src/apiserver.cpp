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

#include "../../CommonHeaders/LEDS_COUNT.h"

using namespace SettingsScope;

// get
// getstatus - on off
// getstatusapi - busy idle
// getprofiles - list name profiles
// getprofile - current name profile

// commands
// lock - begin work with api (disable capture, backlight)
// unlock - end work with api (enable capture, backlight)
//
// setcolor:1-r,g,b;5-r,g,b;   numbering starts with 1
// setcolor:2-0,255,0
// setcolor:2-0,255,0;3-0,255,0;6-0,255,0;
//
// setgamma:2.00 - set gamma for setcolor
// setsmooth:100 - set smooth in device
// setprofile:<name> - set profile
// setstatus:on - set status (on, off)
//
// exit - close connection

// Immediatly after successful connection server sends client ApiVersion
const char * ApiServer::ApiVersion = "version:"API_VERSION"\n";
const char * ApiServer::CmdUnknown = "unknown command\n";
const char * ApiServer::CmdExit = "exit";

const char * ApiServer::CmdApiKey = "apikey:";
const char * ApiServer::CmdApiKeyResult_Ok = "ok\n";
const char * ApiServer::CmdApiKeyResult_Fail = "fail\n";
const char * ApiServer::CmdApiCheck_AuthRequired = "authorization required\n";

const char * ApiServer::CmdGetStatus = "getstatus";
const char * ApiServer::CmdResultStatus_On = "status:on\n";
const char * ApiServer::CmdResultStatus_Off = "status:off\n";
const char * ApiServer::CmdResultStatus_DeviceError = "status:device error\n";
const char * ApiServer::CmdResultStatus_Unknown = "status:unknown\n";

const char * ApiServer::CmdGetStatusAPI = "getstatusapi";
const char * ApiServer::CmdResultStatusAPI_Busy = "statusapi:busy\n";
const char * ApiServer::CmdResultStatusAPI_Idle = "statusapi:idle\n";

const char * ApiServer::CmdGetProfiles = "getprofiles";
// Necessary to add a new line after filling results!
const char * ApiServer::CmdResultProfiles = "profiles:";

const char * ApiServer::CmdGetProfile = "getprofile";
// Necessary to add a new line after filling results!
const char * ApiServer::CmdResultProfile = "profile:";

const char * ApiServer::CmdLock = "lock";
const char * ApiServer::CmdResultLock_Success = "lock:success\n";
const char * ApiServer::CmdResultLock_Busy = "lock:busy\n";

const char * ApiServer::CmdUnlock = "unlock";
const char * ApiServer::CmdResultUnlock_Success = "unlock:success\n";
const char * ApiServer::CmdResultUnlock_NotLocked = "unlock:not locked\n";

// Set-commands works only after success lock
// Set-commands can return, after self-name, only this results:
const char * ApiServer::CmdSetResult_Ok = "ok\n";
const char * ApiServer::CmdSetResult_Error = "error\n";
const char * ApiServer::CmdSetResult_Busy = "busy\n";
const char * ApiServer::CmdSetResult_NotLocked = "not locked\n";

// Set-commands contains at end semicolon!!!
const char * ApiServer::CmdSetColor = "setcolor:";
const char * ApiServer::CmdSetGamma = "setgamma:";
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
}

ApiServer::ApiServer(quint16 port, QObject *parent)
    : QTcpServer(parent)
{
    // This constructor is for using in ApiTests

    initPrivateVariables();
    initApiSetColorTask();

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
    cs.gamma = SettingsScope::Profile::Device::GammaDefault;

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
            result += "\n";
        }
        else if (cmdBuffer == CmdGetProfile)
        {
            API_DEBUG_OUT << CmdGetProfile;

            result = CmdResultProfile + Settings::getCurrentProfileName() + "\n";
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
                    emit startTask(cmdBuffer, m_clients.value(client).gamma);

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

                            m_clients[client].gamma = gamma;

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

    connect(m_apiSetColorTask, SIGNAL(taskDone(QList<QRgb>)), this, SIGNAL(updateLedsColors(QList<QRgb>)), Qt::QueuedConnection);

    connect(m_apiSetColorTask, SIGNAL(taskIsSuccess(bool)), this, SLOT(taskSetColorIsSuccess(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(startTask(QByteArray,double)), m_apiSetColorTask, SLOT(startTask(QByteArray,double)), Qt::QueuedConnection);

    connect(this, SIGNAL(clearColorBuffers()), m_apiSetColorTask, SLOT(clearColorBuffers()));

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
