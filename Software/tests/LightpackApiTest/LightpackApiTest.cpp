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
#include "Settings.hpp"
#include "enums.hpp"

#include <stdlib.h>

#include <iostream>
using namespace std;

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

class SettingsWindowLittleVersion;

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
    void testCase_SetColorValid();
    void testCase_SetColorValid_data();
    void testCase_SetColorValid2();
    void testCase_SetColorValid2_data();
    void testCase_SetColorInvalid();
    void testCase_SetColorInvalid_data();

    void testCase_SetGamma();
    void testCase_SetGammaValid();
    void testCase_SetGammaValid_data();
    void testCase_SetGammaInvalid();
    void testCase_SetGammaInvalid_data();

    void testCase_SetSmooth();
    void testCase_SetProfile();
    void testCase_SetStatus();

//    void testCase_CmdLock_data();

private:
    QByteArray socketReadLine(QTcpSocket * socket, bool *ok);
    void socketWriteCmd(QTcpSocket * socket, const char * cmd);
    QString getProfilesResultString();
    void processEventsFromLittle(SettingsWindowLittleVersion * little);
    QRgb getGammaCorrectedValue(QRgb rgb, double gamma);

private:
    ApiServer *m_apiServer;
    QThread *m_apiServerThread;
    SettingsWindowLittleVersion *little;
};

// SettingsWindowLittleVersion is just for testCase_GetStatus()
class SettingsWindowLittleVersion : public QObject
{
    Q_OBJECT
public:
    SettingsWindowLittleVersion(QObject *parent) : QObject(parent)
    { m_status = Backlight::StatusOff; }
signals:
    void resultBacklightStatus(Backlight::Status status);
public slots:
    void requestBacklightStatus();
    void setLedColors(QList<QRgb> colors);
public:
    Backlight::Status m_status;
    bool m_isDone;
    QList<QRgb> m_colors;
};

void SettingsWindowLittleVersion::requestBacklightStatus()
{
    emit resultBacklightStatus(m_status);
    m_isDone = true;
}

void SettingsWindowLittleVersion::setLedColors(QList<QRgb> colors)
{
    m_colors = colors;
    m_isDone = true;
}

LightpackApiTest::LightpackApiTest()
{
    // Register QMetaType for Qt::QueuedConnection
    qRegisterMetaType< QList<QRgb> >("QList<QRgb>");
    qRegisterMetaType<Backlight::Status>("Backlight::Status");
}

void LightpackApiTest::initTestCase()
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    // Start Api Server in separate thread for access by QTcpSocket-s
    m_apiServer = new ApiServer(3636);
    m_apiServerThread = new QThread();

    m_apiServer->moveToThread(m_apiServerThread);
    m_apiServerThread->start();

    little = new SettingsWindowLittleVersion(this);

    connect(m_apiServer, SIGNAL(requestBacklightStatus()), little, SLOT(requestBacklightStatus()), Qt::QueuedConnection);
    connect(little, SIGNAL(resultBacklightStatus(Backlight::Status)), m_apiServer, SLOT(resultBacklightStatus(Backlight::Status)), Qt::QueuedConnection);

    connect(m_apiServer, SIGNAL(updateLedsColors(QList<QRgb>)), little, SLOT(setLedColors(QList<QRgb>)), Qt::QueuedConnection);

    Settings::Initialize(QDir::currentPath(), true);
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
    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);

    bool sockReadLineOk = false;
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Test Backlight Off state:
    little->m_status = Backlight::StatusOff;
    socketWriteCmd(&sock, ApiServer::CmdGetStatus);

    processEventsFromLittle(little);

    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultStatus_Off);

    // Test Backlight On state:
    little->m_status = Backlight::StatusOn;
    socketWriteCmd(&sock, ApiServer::CmdGetStatus);

    processEventsFromLittle(little);

    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultStatus_On);

    // Test Backlight DeviceError state:
    little->m_status = Backlight::StatusDeviceError;
    socketWriteCmd(&sock, ApiServer::CmdGetStatus);

    processEventsFromLittle(little);

    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultStatus_DeviceError);
}

void LightpackApiTest::testCase_GetStatusAPI()
{
    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);

    bool sockReadLineOk = false;
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Test idle state:
    socketWriteCmd(&sock, ApiServer::CmdGetStatusAPI);

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
    QString cmdProfilesCheckResult = getProfilesResultString();

    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);

    bool sockReadLineOk = false;
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Test GetProfiles command:

    socketWriteCmd(&sock, ApiServer::CmdGetProfiles);

    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);

    qDebug() << "res=" << result;
    qDebug() << "check=" << cmdProfilesCheckResult;

    QVERIFY(result == cmdProfilesCheckResult);

    // Test UTF-8 in profile name

    QString utf8Check = "\u041F\u0440\u043E\u0432\u0435\u0440\u043A\u0430"; // Russian word "Proverka"

    Settings::loadOrCreateProfile("ApiTestProfile");
    Settings::loadOrCreateProfile("ApiTestProfile-UTF-8-" + utf8Check);

    QString cmdProfilesCheckResultWithUtf8 = getProfilesResultString();

    socketWriteCmd(&sock, ApiServer::CmdGetProfiles);

    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);

    QVERIFY(result == cmdProfilesCheckResultWithUtf8);
}

void LightpackApiTest::testCase_GetProfile()
{
    QString cmdProfileCheckResult = ApiServer::CmdResultProfile +
            Settings::getCurrentProfileName() + "\n";

    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);

    bool sockReadLineOk = false;
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Test GetProfile command:

    socketWriteCmd(&sock, ApiServer::CmdGetProfile);

    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);

    QVERIFY(result == cmdProfileCheckResult);
}

void LightpackApiTest::testCase_Lock()
{
    bool sockReadLineOk = false;

    QTcpSocket sockLock, sockTryLock;
    sockLock.connectToHost("127.0.0.1", 3636);
    sockTryLock.connectToHost("127.0.0.1", 3636);

    socketReadLine(&sockLock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);
    socketReadLine(&sockTryLock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Test lock success
    socketWriteCmd(&sockLock, ApiServer::CmdLock);

    QByteArray result = socketReadLine(&sockLock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);

    QVERIFY(result == ApiServer::CmdResultLock_Success);

    // Test lock busy
    socketWriteCmd(&sockTryLock, ApiServer::CmdLock);

    result = socketReadLine(&sockTryLock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);

    QVERIFY(result == ApiServer::CmdResultLock_Busy);

    // Test lock success after unlock
    socketWriteCmd(&sockLock, ApiServer::CmdUnlock);

    result = socketReadLine(&sockLock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);

    QVERIFY(result == ApiServer::CmdResultUnlock_Success);

    socketWriteCmd(&sockTryLock, ApiServer::CmdLock);

    result = socketReadLine(&sockTryLock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);

    QVERIFY(result == ApiServer::CmdResultLock_Success);
}

void LightpackApiTest::testCase_Unlock()
{
    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);

    bool sockReadLineOk = false;
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Test Unlock command:

    socketWriteCmd(&sock, ApiServer::CmdUnlock);

    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);

    QVERIFY(result == ApiServer::CmdResultUnlock_NotLocked);
}

void LightpackApiTest::testCase_SetColor()
{
    bool sockReadLineOk = false;

    QTcpSocket sock, sockLock;
    sock.connectToHost("127.0.0.1", 3636);
    sockLock.connectToHost("127.0.0.1", 3636);

    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);
    socketReadLine(&sockLock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Test in SetColor command is lock state:
    QByteArray setColorCmd = ApiServer::CmdSetColor;

    setColorCmd += "1-23,2,65;";
    int led = 0;
    QRgb rgb = getGammaCorrectedValue(qRgb(23, 2, 65), GAMMA_CORRECTION_DEFAULT_VALUE);

    socketWriteCmd(&sock, setColorCmd);

    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);

    QVERIFY(result == QByteArray(ApiServer::CmdSetColor).append(ApiServer::CmdSetResult_NotLocked));

    // Test in SetColor command is busy state:
    socketWriteCmd(&sockLock, ApiServer::CmdLock);
    result = socketReadLine(&sockLock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultLock_Success);

    socketWriteCmd(&sock, setColorCmd);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == QByteArray(ApiServer::CmdSetColor).append(ApiServer::CmdSetResult_Busy));

    // Test in SetColor change to unlock state:
    socketWriteCmd(&sockLock, ApiServer::CmdUnlock);
    result = socketReadLine(&sockLock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultUnlock_Success);

    socketWriteCmd(&sock, ApiServer::CmdLock);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultLock_Success);

    // Test SetColor different valid strings
    socketWriteCmd(&sock, setColorCmd);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == QByteArray(ApiServer::CmdSetColor).append(ApiServer::CmdSetResult_Ok));

    processEventsFromLittle(little);

    QVERIFY(little->m_colors[led] == rgb);
}

void LightpackApiTest::testCase_SetColorValid()
{
    bool sockReadLineOk = false;

    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Lock
    socketWriteCmd(&sock, ApiServer::CmdLock);
    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultLock_Success);

    QByteArray setColorCmd = ApiServer::CmdSetColor;

    QFETCH(QString, cmd);
    QFETCH(int, led);
    QFETCH(int, r);
    QFETCH(int, g);
    QFETCH(int, b);

    QRgb rgb = getGammaCorrectedValue(qRgb(r, g, b), GAMMA_CORRECTION_DEFAULT_VALUE);

    setColorCmd += cmd;

    // Test SetColor different valid strings
    socketWriteCmd(&sock, setColorCmd);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == QByteArray(ApiServer::CmdSetColor).append(ApiServer::CmdSetResult_Ok));

    processEventsFromLittle(little);

    QVERIFY(little->m_colors[led-1] == rgb);

    // Unlock
    socketWriteCmd(&sock, ApiServer::CmdUnlock);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultUnlock_Success);
}

void LightpackApiTest::testCase_SetColorValid_data()
{
    QTest::addColumn<QString>("cmd");
    QTest::addColumn<int>("led");
    QTest::addColumn<int>("r");
    QTest::addColumn<int>("g");
    QTest::addColumn<int>("b");

    QTest::newRow("0") << "1-1,1,1" << 1 << 1 << 1 << 1; // setcolor without semicolon is valid!
    QTest::newRow("1") << "1-1,1,1;" << 1 << 1 << 1 << 1;
    QTest::newRow("2") << "2-1,1,1;" << 2 << 1 << 1 << 1;
    QTest::newRow("3") << "10-1,1,1;" << 10 << 1 << 1 << 1;
    QTest::newRow("4") << "1-192,1,1;" << 1 << 192 << 1 << 1;
    QTest::newRow("5") << "1-1,185,1;" << 1 << 1 << 185 << 1;
    QTest::newRow("6") << "1-14,15,19;" << 1 << 14 << 15 << 19;
    QTest::newRow("7") << "1-255,255,255;" << 1 << 255 << 255 << 255;
    QTest::newRow("8") << "10-255,255,255;" << 10 << 255 << 255 << 255;
    QTest::newRow("9") << "10-0,0,1;" << 10 << 0 << 0 << 1;
    QTest::newRow("10") << "10-1,0,0;" << 10 << 1 << 0 << 0;
}

void LightpackApiTest::testCase_SetColorValid2()
{
    bool sockReadLineOk = false;

    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Lock
    socketWriteCmd(&sock, ApiServer::CmdLock);
    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultLock_Success);

    QByteArray setColorCmd = ApiServer::CmdSetColor;

    QFETCH(QString, cmd);

    setColorCmd += cmd;

    // Test SetColor different valid strings
    socketWriteCmd(&sock, setColorCmd);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == QByteArray(ApiServer::CmdSetColor).append(ApiServer::CmdSetResult_Ok));

    // Just process all pending events from m_apiServer
    processEventsFromLittle(little);

    // Unlock
    socketWriteCmd(&sock, ApiServer::CmdUnlock);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultUnlock_Success);
}

void LightpackApiTest::testCase_SetColorValid2_data()
{
    QTest::addColumn<QString>("cmd");

    QTest::newRow("1") << "1-1,1,1;2-2,2,2;3-3,3,3;4-4,4,4;5-5,5,5;6-6,6,6;7-7,7,7;8-8,8,8;9-9,9,9;10-10,10,10;";
    QTest::newRow("2") << "1-1,1,1;2-2,2,2;3-3,3,3;4-4,4,4;5-5,5,5;6-6,6,6;7-7,7,7;8-8,8,8;9-9,9,9;";
    QTest::newRow("3") << "1-1,1,1;2-2,2,2;3-3,3,3;4-4,4,4;5-5,5,5;6-6,6,6;7-7,7,7;8-8,8,8;";
    QTest::newRow("4") << "1-1,1,1;2-2,2,2;3-3,3,3;4-4,4,4;5-5,5,5;6-6,6,6;7-7,7,7;";
    QTest::newRow("5") << "1-1,1,1;2-2,2,2;3-3,3,3;4-4,4,4;5-5,5,5;6-6,6,6;";
    QTest::newRow("6") << "1-1,1,1;2-2,2,2;3-3,3,3;4-4,4,4;5-5,5,5;";
    QTest::newRow("7") << "1-1,1,1;2-2,2,2;3-3,3,3;4-4,4,4;";
    QTest::newRow("8") << "1-1,1,1;2-2,2,2;3-3,3,3;";
    QTest::newRow("9") << "1-1,1,1;2-2,2,2;";
    QTest::newRow("10") << "1-1,1,1;";

    // Set color to one led several times in one cmd is VALID!
    QTest::newRow("11") << "1-1,1,1;1-1,1,1;1-2,2,2;1-3,3,3;";
}


void LightpackApiTest::testCase_SetColorInvalid()
{
    bool sockReadLineOk = false;

    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Lock
    socketWriteCmd(&sock, ApiServer::CmdLock);
    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultLock_Success);

    QByteArray setColorCmd = ApiServer::CmdSetColor;

    QFETCH(QString, cmd);

    setColorCmd += cmd;

    // Test SetColor different valid strings
    socketWriteCmd(&sock, setColorCmd);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == QByteArray(ApiServer::CmdSetColor).append(ApiServer::CmdSetResult_Error));

    // Unlock
    socketWriteCmd(&sock, ApiServer::CmdUnlock);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultUnlock_Success);
}

void LightpackApiTest::testCase_SetColorInvalid_data()
{
     QTest::addColumn<QString>("cmd");

     QTest::newRow("1") << "1--1,1,1;";
     QTest::newRow("2") << "1-1,,1,1;";
     QTest::newRow("3") << "1-1,1,,1;";
     QTest::newRow("4") << "1-1.1.1";
     QTest::newRow("5") << "1-1,1,1;2-";
     QTest::newRow("6") << "11-1,1,1;";
     QTest::newRow("7") << "0-1,1,1;";
     QTest::newRow("8") << "1-1,-1,1;";
     QTest::newRow("9") << "1-1,1111,1;";
     QTest::newRow("10") << "1-1,1,256;";
     QTest::newRow("11") << "!-1,1,1;";
     QTest::newRow("12") << "-1,1,1;";
     QTest::newRow("13") << "1-1,1;";
     QTest::newRow("14") << "1-1,100000000000000000000000;";
     QTest::newRow("15") << "1-1,1,1;2-4,5,,;";
     QTest::newRow("16") << "1-1,1,1;2-2,2,2;3-3,3,3;4-4,4,4;5-5,5,5;;6-6,6,6;7-7,7,7;8-8,8,8;9-9,9,9;";
     QTest::newRow("17") << "1-1,1,1;;";
}

void LightpackApiTest::testCase_SetGamma()
{
    bool sockReadLineOk = false;

    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Lock
    socketWriteCmd(&sock, ApiServer::CmdLock);
    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultLock_Success);

    // Set gamma to 5.0 value
    QByteArray setGammaCmd = ApiServer::CmdSetGamma;
    setGammaCmd += "5.0";

    socketWriteCmd(&sock, setGammaCmd);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == QByteArray(ApiServer::CmdSetGamma).append(ApiServer::CmdSetResult_Ok));

    // Test set color works with new gamma
    QByteArray setColorCmd = ApiServer::CmdSetColor;
    setColorCmd += "1-23,2,65;";
    int led = 0;
    QRgb rgb = getGammaCorrectedValue(qRgb(23, 2, 65), 5.0);

    socketWriteCmd(&sock, setColorCmd);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == QByteArray(ApiServer::CmdSetColor).append(ApiServer::CmdSetResult_Ok));

    processEventsFromLittle(little);

    QVERIFY(little->m_colors[led] == rgb);

    // Unlock
    socketWriteCmd(&sock, ApiServer::CmdUnlock);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultUnlock_Success);
}

void LightpackApiTest::testCase_SetGammaValid()
{
    bool sockReadLineOk = false;

    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Lock
    socketWriteCmd(&sock, ApiServer::CmdLock);
    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultLock_Success);

    QFETCH(QString, gammaStr);

    QByteArray setGammaCmd = ApiServer::CmdSetGamma;
    setGammaCmd += gammaStr;

    socketWriteCmd(&sock, setGammaCmd);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == QByteArray(ApiServer::CmdSetGamma).append(ApiServer::CmdSetResult_Ok));

    // Unlock
    socketWriteCmd(&sock, ApiServer::CmdUnlock);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultUnlock_Success);
}

void LightpackApiTest::testCase_SetGammaValid_data()
{
    QTest::addColumn<QString>("gammaStr");

    QTest::newRow("0") << ".01"; // this is valid
    QTest::newRow("1") << "1.0";
    QTest::newRow("2") << "2.0";
    QTest::newRow("3") << "3";
    QTest::newRow("4") << "10.00";
    QTest::newRow("5") << "0.00";
}

void LightpackApiTest::testCase_SetGammaInvalid()
{
    bool sockReadLineOk = false;

    QTcpSocket sock;
    sock.connectToHost("127.0.0.1", 3636);
    socketReadLine(&sock, &sockReadLineOk); // skip version line
    QVERIFY(sockReadLineOk);

    // Lock
    socketWriteCmd(&sock, ApiServer::CmdLock);
    QByteArray result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultLock_Success);

    QFETCH(QString, gammaStr);

    QByteArray setGammaCmd = ApiServer::CmdSetGamma;
    setGammaCmd += gammaStr;

    socketWriteCmd(&sock, setGammaCmd);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == QByteArray(ApiServer::CmdSetGamma).append(ApiServer::CmdSetResult_Error));

    // Unlock
    socketWriteCmd(&sock, ApiServer::CmdUnlock);
    result = socketReadLine(&sock, &sockReadLineOk);
    QVERIFY(sockReadLineOk);
    QVERIFY(result == ApiServer::CmdResultUnlock_Success);
}

void LightpackApiTest::testCase_SetGammaInvalid_data()
{
    QTest::addColumn<QString>("gammaStr");

    QTest::newRow("1") << "0.0001";
    QTest::newRow("2") << "12.0";
    QTest::newRow("3") << ":12.0";
    QTest::newRow("4") << "2.0;";
    QTest::newRow("5") << "1.2.0";
    QTest::newRow("6") << "-1.2";
    QTest::newRow("7") << "10.01";
    QTest::newRow("8") << "4.56857";
    QTest::newRow("9") << "4.5685787384739473827432423489237985739487593745987349857938475";
    QTest::newRow("10") << "+100500";
    QTest::newRow("11") << "Galaxy in Danger!";
    QTest::newRow("12") << "reinterpret_cast<CodeMonkey*>(m_pSelf);";
    QTest::newRow("13") << "BSOD are coming soon!";
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

QString LightpackApiTest::getProfilesResultString()
{
    QStringList profiles = Settings::findAllProfiles();

    QString cmdProfilesCheckResult = ApiServer::CmdResultProfiles;
    for (int i = 0; i < profiles.count(); i++)
        cmdProfilesCheckResult += profiles[i] + ";";
    cmdProfilesCheckResult += "\n";

    return cmdProfilesCheckResult;
}

void LightpackApiTest::processEventsFromLittle(SettingsWindowLittleVersion * little)
{
    QTime time;
    time.restart();
    little->m_isDone = false;

    while (little->m_isDone == false && time.elapsed() < ApiServer::SignalWaitTimeoutMs)
    {
        QApplication::processEvents(QEventLoop::WaitForMoreEvents, ApiServer::SignalWaitTimeoutMs);
    }
}

QRgb LightpackApiTest::getGammaCorrectedValue(QRgb rgb, double gamma)
{
    unsigned r = 256.0 * pow( qRed(rgb)   / 256.0, gamma );
    unsigned g = 256.0 * pow( qGreen(rgb) / 256.0, gamma );
    unsigned b = 256.0 * pow( qBlue(rgb)  / 256.0, gamma );

    return qRgb(r, g, b);
}

unsigned g_debugLevel = Debug::LowLevel;

QTEST_MAIN(LightpackApiTest)

#include "LightpackApiTest.moc"
