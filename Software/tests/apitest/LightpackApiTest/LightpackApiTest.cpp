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

#include "debug.h"
#include "ApiServer.hpp"

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

    void testCase_getstatus();
    void testCase_getstatusapi();
    void testCase_getprofiles();
    void testCase_getprofile();

    void testCase_lock();
    void testCase_unlock();

    void testCase_setcolor();
    void testCase_setgamma();
    void testCase_setsmooth();
    void testCase_setprofile();
    void testCase_setstatus();

//    void testCase_CmdLock_data();

private:
    ApiServer *m_apiServer;
};

LightpackApiTest::LightpackApiTest()
{
    // Register QMetaType for Qt::QueuedConnection
    qRegisterMetaType< QList<QRgb> >("QList<QRgb>");
}

void LightpackApiTest::initTestCase()
{
    m_apiServer = new ApiServer();

    int port = 3636;

    bool ok = m_apiServer->listen(QHostAddress::Any, port);

    if (!ok)
    {
        QFAIL("API Server start listen fail");
    }
}

void LightpackApiTest::cleanupTestCase()
{
    delete m_apiServer;
}

//
// Test cases
//

void LightpackApiTest::testCase_getstatus()
{       
    QVERIFY(false);
}

void LightpackApiTest::testCase_getstatusapi()
{
    //QSignalSpy spy(m_apiServer, SIGNAL(updateLedsColors(API::Status)));
    QVERIFY(false);
}
void LightpackApiTest::testCase_getprofiles()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_getprofile()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_lock()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_unlock()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_setcolor()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_setgamma()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_setsmooth()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_setprofile()
{
    QVERIFY(false);
}

void LightpackApiTest::testCase_setstatus()
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


unsigned g_debugLevel = Debug::LowLevel;

QTEST_MAIN(LightpackApiTest)

#include "LightpackApiTest.moc"
