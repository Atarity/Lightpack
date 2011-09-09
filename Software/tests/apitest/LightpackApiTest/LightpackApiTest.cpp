/*
 * LightpackApiTest.cpp
 *
 *  Created on: 07.09.2011
 *      Author: Mike Shatohin
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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtNetwork>

#include "debug.h"
#include "ApiServer.hpp"

#define VERSION_API_TESTS   "1.2"

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

class LightpackApiTest : public QObject
{
    Q_OBJECT

public:
    LightpackApiTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_ApiVersion();

    void testCase_GetStatus();
    void testCase_GetStatusAPI();
    void testCase_GetProfiles();
    void testCase_GetProfile();

    void testCase_Lock();
    void testCase_Unlock();

    void testCase_SetColor();
    void testCase_SetGamma();
    void testCase_SetSmooth();
    void testCase_SetProfile();
    void testCase_SetStatus();

//    void testCase_CmdLock_data();

private:
    QByteArray socketReadLine(QTcpSocket * socket, bool *ok);
    void socketWriteCmd(QTcpSocket * socket, const char * cmd);

private:
    ApiServer *m_apiServer;
    QThread *m_apiServerThread;
};

LightpackApiTest::LightpackApiTest()
{
    // Register QMetaType for Qt::QueuedConnection
    qRegisterMetaType< QList<QRgb> >("QList<QRgb>");
}

void LightpackApiTest::initTestCase()
{
    // Start Api Server in separate thread for access by QTcpSocket-s
    m_apiServer = new ApiServer(3636);
    m_apiServerThread = new QThread();

    m_apiServer->moveToThread(m_apiServerThread);
    m_apiServerThread->start();
}

void LightpackApiTest::cleanupTestCase()
{
}

//
// Test cases
//
void LightpackApiTest::testCase_ApiVersion()
{
    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);

    // Check the version of the API and Tests for a match
    bool sockReadLineOk = false;
    QString readApiVersion = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(readApiVersion.remove("version:").trimmed() == VERSION_API_TESTS);
}

void LightpackApiTest::testCase_GetStatus()
{       
    QVERIFY(false);
}

void LightpackApiTest::testCase_GetStatusAPI()
{
    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);

    bool sockReadLineOk = false;
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Test idle state:

    sock.write(ApiServer::CmdGetStatusAPI);
    sock.write("\n");

    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultStatusAPI_Idle);

    // Test lock and busy state:

    QTcpSocket sockLock;
    sockLock.connectToHost("127.0.0.1", 3636);
    socketReadLine(&sockLock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    socketWriteCmd(&sockLock, ApiServer::CmdLock);
    QVERIFY(socketReadLine(&sockLock, &sockReadLineOk) == ApiServer::CmdResultLock_Success);
    QVERIFY(sockReadLineOk);

    socketWriteCmd(&sock, ApiServer::CmdGetStatusAPI);
    QVERIFY(socketReadLine(&sock, &sockReadLineOk) == ApiServer::CmdResultStatusAPI_Busy);
    QVERIFY(sockReadLineOk);

    // Test unlock and return to idle state

    socketWriteCmd(&sockLock, ApiServer::CmdUnlock);
    QVERIFY(socketReadLine(&sockLock, &sockReadLineOk) == ApiServer::CmdResultUnlock_Success);
    QVERIFY(sockReadLineOk);

    socketWriteCmd(&sock, ApiServer::CmdGetStatusAPI);
    QVERIFY(socketReadLine(&sock, &sockReadLineOk) == ApiServer::CmdResultStatusAPI_Idle);
    QVERIFY(sockReadLineOk);
}

void LightpackApiTest::testCase_GetProfiles()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_GetProfile()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_Lock()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_Unlock()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_SetColor()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_SetGamma()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_SetSmooth()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_SetProfile()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_SetStatus()
{
    QVERIFY(false);
}

//void LightpackApiTest::testCase_CmdLock()
//{
//    QFETCH(QString, cmd);
//    QVERIFY2(true, "Failure");
//}

//void LightpackApiTest::testCase_CmdLock_data()
//{
//    QTest::addColumn<QString>("data");
//    QTest::newRow("0") << QString();
//}


#define QVERIFY_R(statement, RETURN_VALUE) \
do {\
    if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__))\
        return (RETURN_VALUE);\
} while (0)

QByteArray LightpackApiTest::socketReadLine(QTcpSocket * socket, bool *ok)
{
    *ok = socket->waitForReadyRead(1000) && socket->canReadLine();

    return socket->readLine();
}

void LightpackApiTest::socketWriteCmd(QTcpSocket * socket, const char * cmd)
{
    socket->write(cmd);
    socket->write("\n");
}

unsigned g_debugLevel = Debug::LowLevel;

QTEST_MAIN(LightpackApiTest)

#include "LightpackApiTest.moc"
