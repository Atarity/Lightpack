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

#include "settings.h"
#include "timeevaluations.h"

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
const char * ApiServer::ApiVersion = "version:1.2\n";

const char * ApiServer::CmdGetStatus = "getstatus";
const char * ApiServer::CmdResultStatus_On = "status:on\n";
const char * ApiServer::CmdResultStatus_Off = "status:off\n";
const char * ApiServer::CmdResultStatus_DeviceError = "status:error\n";

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

ApiServer::ApiServer(QObject *parent)
    : QTcpServer(parent)
{
    m_lockedClient = NULL;
    initApiSetColorTask();
}

ApiServer::ApiServer(quint16 port, QObject *parent)
    : QTcpServer(parent)
{
    m_lockedClient = NULL;
    initApiSetColorTask();

    bool ok = listen(QHostAddress::Any, port);
    if (!ok)
    {
        qFatal("%s listen(Any, %d) fail", Q_FUNC_INFO, port);
    }
}

void ApiServer::incomingConnection(int socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    ClientSettings cs;
    cs.gamma = GAMMA_CORRECTION_DEFAULT_VALUE;
    cs.smooth = FW_SMOOTH_SLOWDOWN_DEFAULT;
    cs.auth = false;
    m_clients.insert(client,cs);

    client->write(ApiVersion);

    DEBUG_LOW_LEVEL << "New client from:" << client->peerAddress().toString();

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void ApiServer::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    DEBUG_LOW_LEVEL << "Client disconnected:" << client->peerAddress().toString();
    if (m_lockedClient == client)
    {
        m_lockedClient = NULL;

        // TODO: Send MainWindow unlock signal
    }
    m_clients.remove(client);
}

void ApiServer::readyRead()
{
    API_DEBUG_OUT << Q_FUNC_INFO << "ApiServer thread id:" << this->thread()->currentThreadId();

    QTcpSocket *client = (QTcpSocket *)sender();

    while (client->canReadLine())
    {
        QByteArray buffer = client->readLine().trimmed();

        if (buffer == CmdGetStatusAPI)
        {
            client->write(CmdResultStatusAPI_Idle);
        }
        else if (buffer.startsWith(CmdSetColor))
        {
            buffer.remove(0, buffer.indexOf(':') + 1);
            API_DEBUG_OUT << QString(buffer);

            if (m_isTaskSetColorDone)
            {
                m_isTaskSetColorDone = false;

                emit startTask(buffer);
            } else {
                qWarning() << Q_FUNC_INFO << "Task setcolor is not completed (you should increase the delay to not skip commands), skip setcolor.";
            }
        }
        else if (buffer == CmdLock)
        {
            API_DEBUG_OUT << "lock not implemented";
        }
        else if (buffer == CmdUnlock)
        {
            API_DEBUG_OUT << "unlock not implemented";
        }
        else
        {
            API_DEBUG_OUT << "error read buffer:" << QString(buffer);
        }        
    }
}

void ApiServer::taskSetColorIsSuccess(bool /*isSuccess*/)
{
    m_isTaskSetColorDone = true;
}

void ApiServer::answerAppMainStatus(MainWindow::AppMainStatus status)
{
    m_appMainStatus = status;
    m_isAnswerAppMainStatusDone = true;
}

void ApiServer::initApiSetColorTask()
{
    m_isTaskSetColorDone = true;

    m_apiSetColorTaskThread = new QThread();
    m_apiSetColorTask = new ApiServerSetColorTask();

    connect(m_apiSetColorTask, SIGNAL(taskDone(QList<QRgb>)), this, SIGNAL(updateLedsColors(QList<QRgb>)), Qt::QueuedConnection);

    connect(m_apiSetColorTask, SIGNAL(taskIsSuccess(bool)), this, SLOT(taskSetColorIsSuccess(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(startTask(QByteArray)), m_apiSetColorTask, SLOT(startTask(QByteArray)), Qt::QueuedConnection);

    m_apiSetColorTask->moveToThread(m_apiSetColorTaskThread);
    m_apiSetColorTaskThread->start();
}
