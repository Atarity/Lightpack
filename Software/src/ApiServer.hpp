/*
 * ApiServer.hpp
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

#pragma once

#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>
#include <QRgb>
#include "apidebug.h"
#include "inlinemath.hpp"
#include "ApiServerSetColorTask.hpp"

#define VERSION_API      "1.1"

struct ClientSettings
{
    int smooth;
    double gamma;
    bool auth;
};

class ApiServer : public QTcpServer
{
    Q_OBJECT

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


public:
    ApiServer(QObject *parent = 0);

signals:
    void requestStatus();

    void updateLedsColors(const QList<QRgb> & colors);
    void startTask(QByteArray buffer);

private slots:
    void readyRead();
    void taskSetColorIsSuccess(bool isSuccess);
    void disconnected();

protected:
    void incomingConnection(int socketfd);

private:
    //QSet<QTcpSocket*> clients;
    QMap<QTcpSocket*,ClientSettings>clients;
    QTcpSocket* activeClient;
    QList<QRgb> colorsNew;

    QString getCommand(const QString &  str);
    QString getArg(const QString &  str);


private:
    QString m_apiKey;

    QThread *m_apiTaskThread;
    ApiServerSetColorTask *m_apiSetColorTask;
    bool m_isTaskSetColorDone;
};
