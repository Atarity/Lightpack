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

// Immediatly after successful connection server sends client ApiVersion
const char * ApiServer::ApiVersion = "version:"API_VERSION"\n";
const char * ApiServer::CmdUnknown = "unknown command\n";

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

const int ApiServer::SignalWaitTimeoutMs = 1000; // 1 second

ApiServer::ApiServer(QObject *parent)
    : QTcpServer(parent)
{
    m_lockedClient = NULL;
    m_isRequestBacklightStatusDone = true;
    m_backlightStatus = Backlight::StatusUnknown;

    initApiSetColorTask();
}

ApiServer::ApiServer(quint16 port, QObject *parent)
    : QTcpServer(parent)
{
    m_lockedClient = NULL;
    m_isRequestBacklightStatusDone = true;
    m_backlightStatus = Backlight::StatusUnknown;

    initApiSetColorTask();

    bool ok = listen(QHostAddress::Any, port);
    if (!ok)
    {
        qFatal("%s listen(Any, %d) fail", Q_FUNC_INFO, port);
    }
}

void ApiServer::incomingConnection(int socketDescriptor)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketDescriptor);

    ClientInfo cs;
    cs.isAuthorized = false;
    cs.gamma = GAMMA_CORRECTION_DEFAULT_VALUE;

    m_clients.insert(client, cs);

    client->write(ApiVersion);

    DEBUG_LOW_LEVEL << "Incoming connection from:" << client->peerAddress().toString();

    connect(client, SIGNAL(readyRead()), this, SLOT(clientReadyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
}

void ApiServer::clientDisconnected()
{
    QTcpSocket *client = dynamic_cast<QTcpSocket*>(sender());

    DEBUG_LOW_LEVEL << "Client disconnected:" << client->peerAddress().toString();

    if (m_lockedClient == client)
    {
        m_lockedClient = NULL;

        // TODO: Send MainWindow unlock signal
    }
    m_clients.remove(client);

    disconnect(client, SIGNAL(readyRead()), this, SLOT(clientReadyRead()));
    disconnect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));

    client->deleteLater();
}

void ApiServer::clientReadyRead()
{
    API_DEBUG_OUT << Q_FUNC_INFO << "ApiServer thread id:" << this->thread()->currentThreadId();

    QTcpSocket *client = dynamic_cast<QTcpSocket*>(sender());

    while (client->canReadLine())
    {
        QByteArray buffer = client->readLine().trimmed();
        API_DEBUG_OUT << buffer;

        QString result = CmdUnknown;

        if (buffer == CmdGetStatus)
        {
            API_DEBUG_OUT << CmdGetStatus;

            if (m_isRequestBacklightStatusDone)
            {
                m_isRequestBacklightStatusDone = false;
                m_backlightStatus = Backlight::StatusUnknown;

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
                    switch (m_backlightStatus)
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
        else if (buffer == CmdGetStatusAPI)
        {
            API_DEBUG_OUT << CmdGetStatusAPI;

            if (m_lockedClient != NULL)
                result = CmdResultStatusAPI_Busy;
            else
                result = CmdResultStatusAPI_Idle;
        }
        else if (buffer == CmdGetProfiles)
        {
            API_DEBUG_OUT << CmdGetProfiles;

            QStringList profiles = Settings::findAllProfiles();

            result = ApiServer::CmdResultProfiles;

            for (int i = 0; i < profiles.count(); i++)
                result += profiles[i] + ";";
            result += "\n";
        }
        else if (buffer == CmdGetProfile)
        {
            API_DEBUG_OUT << CmdGetProfile;

            result = CmdResultProfile + Settings::getCurrentProfileName() + "\n";
        }
        else if (buffer == CmdLock)
        {
            API_DEBUG_OUT << CmdLock;

            if (m_lockedClient == NULL)
            {
                m_lockedClient = client;
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
        else if (buffer == CmdUnlock)
        {
            API_DEBUG_OUT << CmdUnlock;

            if (m_lockedClient == NULL)
            {
                result = CmdResultUnlock_NotLocked;
            } else {
                if (m_lockedClient == client)
                    m_lockedClient = NULL;
                result = CmdResultUnlock_Success;
            }
        }
        else if (buffer.startsWith(CmdSetColor))
        {
            API_DEBUG_OUT << CmdSetColor;
            result = CmdSetColor;            

            if (m_lockedClient == client)
            {
                buffer.remove(0, buffer.indexOf(':') + 1);
                API_DEBUG_OUT << QString(buffer);                               

                if (m_isTaskSetColorDone)
                {
                    m_isTaskSetColorDone = false;
                    m_isTaskSetColorParseSuccess = false;

                    // Start task
                    emit startTask(buffer, m_clients.value(client).gamma);

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
                            result += CmdSetResult_Ok;
                        else
                            result += CmdSetResult_Error;
                    } else {
                        m_isTaskSetColorDone = true; // cmd setcolor is available
                        result += CmdSetResult_Error;
                        qWarning() << Q_FUNC_INFO << "Timeout waiting taskIsSuccess() signal from m_apiSetColorTask";
                    }
                } else {
                    qWarning() << Q_FUNC_INFO << "Task setcolor is not completed (you should increase the delay to not skip commands), skip setcolor.";
                }
            }
            else if (m_lockedClient == NULL)
            {
                result += CmdSetResult_NotLocked;
            }
            else // m_lockedClient != client
            {
                result += CmdSetResult_Busy;
            }
        }
        else if (buffer.startsWith(CmdSetGamma))
        {
            API_DEBUG_OUT << CmdSetGamma;
            result = CmdSetGamma;

            if (m_lockedClient == client)
            {
                buffer.remove(0, buffer.indexOf(':') + 1);
                API_DEBUG_OUT << QString(buffer);

                // Gamma can contain max five chars (0.00 -- 10.00)
                if (buffer.length() > 5)
                {
                    API_DEBUG_OUT << CmdSetGamma << "Error (gamma max 5 chars)";
                    result += CmdSetResult_Error;
                } else {
                    // Try to convert gamma string to double
                    bool ok = false;
                    double gamma = QString(buffer).toDouble(&ok);

                    if (ok)
                    {
                        if (gamma <= GAMMA_MAX_VALUE && gamma >= GAMMA_MIN_VALUE)
                        {
                            API_DEBUG_OUT << CmdSetGamma << "OK:" << gamma;
                            m_clients[client].gamma = gamma;
                            result += CmdSetResult_Ok;
                        } else {
                            API_DEBUG_OUT << CmdSetGamma << "Error (max min test fail):" << gamma;
                            result += CmdSetResult_Error;
                        }
                    } else {
                        API_DEBUG_OUT << CmdSetGamma << "Error (convert fail):" << gamma;
                        result += CmdSetResult_Error;
                    }
                }
            }
            else if (m_lockedClient == NULL)
            {
                result += CmdSetResult_NotLocked;
            }
            else // m_lockedClient != client
            {
                result += CmdSetResult_Busy;
            }
        }
        else
        {
            qWarning() << Q_FUNC_INFO << CmdUnknown;
        }

        API_DEBUG_OUT << result;
        client->write(result.toUtf8());
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
    m_backlightStatus = status;
}

void ApiServer::initApiSetColorTask()
{
    m_isTaskSetColorDone = true;

    m_apiSetColorTaskThread = new QThread();
    m_apiSetColorTask = new ApiServerSetColorTask();

    connect(m_apiSetColorTask, SIGNAL(taskDone(QList<QRgb>)), this, SIGNAL(updateLedsColors(QList<QRgb>)), Qt::QueuedConnection);

    connect(m_apiSetColorTask, SIGNAL(taskIsSuccess(bool)), this, SLOT(taskSetColorIsSuccess(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(startTask(QByteArray,double)), m_apiSetColorTask, SLOT(startTask(QByteArray,double)), Qt::QueuedConnection);

    m_apiSetColorTask->moveToThread(m_apiSetColorTaskThread);
    m_apiSetColorTaskThread->start();
}
