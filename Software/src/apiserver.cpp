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
#include "../inc/mainwindow.h"

#include "settings.h"
#include "timeevaluations.h"

// get
// getstatus - on off
// getstatusapi - busy idle
// getprofiles - list name profiles
// getprofile - current name profile

// commands
// lock - begin work with api (disable capture,backlight)
// unlock - end work with api (enable capture,backlight)
// setcolor:1-r,g,b;5-r,g,b;   numbering starts with 1
// setgamma:2.00 - set gamma for setcolor
// setsmooth:100 - set smooth in device
// setprofile:<name> - set profile
// setstatus:on - set status (on, off)

ApiServer::ApiServer(QObject *parent)
    : QTcpServer(parent)
{
    activeClient = NULL;

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        colorsNew     << 0;
    }

    isTaskSetColorDone = true;

    apiTaskThread = new QThread();
    apiSetColorTask = new ApiServerSetColorTask();

    connect(apiSetColorTask, SIGNAL(taskDone(QList<QRgb>)), this, SIGNAL(updateLedsColors(QList<QRgb>)), Qt::QueuedConnection);

    connect(apiSetColorTask, SIGNAL(taskIsSuccess(bool)), this, SLOT(taskSetColorIsSuccess(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(startTask(QByteArray)), apiSetColorTask, SLOT(startTask(QByteArray)), Qt::QueuedConnection);

    apiSetColorTask->moveToThread(apiTaskThread);
    apiTaskThread->start();
}

void ApiServer::incomingConnection(int socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    ClientSettings cs;
    cs.gamma = GAMMA_CORRECTION_DEFAULT_VALUE;
    cs.smooth = FW_SMOOTH_SLOWDOWN_DEFAULT;
    cs.auth = false;
    clients.insert(client,cs);

    client->write(QString("version:%1\n").arg(VERSION_API).toUtf8());

    DEBUG_LOW_LEVEL << "New client from:" << client->peerName();// client->peerAddress().toString();

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void ApiServer::taskSetColorIsSuccess(bool /*isSuccess*/)
{
    isTaskSetColorDone = true;
}

void ApiServer::readyRead()
{
    API_DEBUG_OUT << Q_FUNC_INFO << "ApiServer thread id:" << this->thread()->currentThreadId();

    QTcpSocket *client = (QTcpSocket*)sender();

    while (client->canReadLine())
    {
        QByteArray buffer = client->readLine().trimmed();

        if (buffer.startsWith("setcolor:"))
        {
            // setcolor:2-0,255,0
            // setcolor:2-0,255,0;3-0,255,0;6-0,255,0;
            buffer.remove(0, buffer.indexOf(':') + 1);
            API_DEBUG_OUT << QString(buffer);

            if (isTaskSetColorDone)
            {
                isTaskSetColorDone = false;

                emit startTask(buffer);
            } else {
                qWarning() << Q_FUNC_INFO << "Task setcolor is not completed (you should increase the delay to not skip commands), skip setcolor.";
            }
        }
        else if (buffer.startsWith("lock"))
        {
            API_DEBUG_OUT << "lock not implemented";
        }
        else if (buffer.startsWith("unlock"))
        {
            API_DEBUG_OUT << "unlock not implemented";
        }
        else
        {
            API_DEBUG_OUT << "error read buffer:" << QString(buffer);
        }

        client->write("nop");
    }
}

void ApiServer::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    DEBUG_LOW_LEVEL << "Client disconnected:" << client->peerAddress().toString();
    if (activeClient==client)
    {
        activeClient = NULL;
        //        MainWindow *mw = (MainWindow*)parent();
        //        mw->m_grabManager->setAmbilightOn(mw->m_isAmbilightOn,true);
    }
    clients.remove(client);
}
