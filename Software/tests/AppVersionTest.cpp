/*
 * AppVersionTest.cpp
 *
 *  Created on: 4/15/2014
 *     Project: Prismatik
 *
 *  Copyright (c) 2014 tim
 *
 *  Lightpack is an open-source, USB content-driving ambient lighting
 *  hardware.
 *
 *  Prismatik is a free, open-source software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Prismatik and Lightpack files is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "AppVersionTest.hpp"
#include <QtTest/QtTest>
#include "../src/UpdatesProcessor.hpp"

AppVersionTest::AppVersionTest()
{
}

void AppVersionTest::testCaseExplicitEquality()
{
    AppVersion a("5.10.7");
    AppVersion b("5.10.7");
    QVERIFY2(a == b, "Version explicit equality test failed");
    QVERIFY2(!(a != b), "Version explicit equality test failed");
}

void AppVersionTest::testCaseImplicitEquality()
{
    AppVersion a("5");
    AppVersion b("5.0.0");
    QVERIFY2(a == b, "Version implicit equality test failed");
    QVERIFY2(!(a != b), "Version implicit equality test failed");
}

void AppVersionTest::testCaseFalseEquality()
{
    AppVersion a("5.1");
    AppVersion b("5.0.0");
    QVERIFY2(a != b, "Version false equality test failed");
    QVERIFY2(!(a == b), "Version false equality test failed");
}

void AppVersionTest::testCaseLT()
{
    AppVersion a("5");
    AppVersion b("5.1.0");
    QVERIFY2(a < b, "Less than test failed");
    QVERIFY2(!(a > b), "Less than test failed");
}

void AppVersionTest::testCaseGT()
{
    AppVersion a("6.0.0.1");
    AppVersion b("5.1.0.2");
    QVERIFY2(a > b, "Greater than test failed");
    QVERIFY2(!(a < b), "Greater than test failed");
}
