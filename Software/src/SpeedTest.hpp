/*
 * SpeedTest.hpp
 *
 *  Created on: 4.04.2011
 *      Author: Mike Shatohin (brunql)
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

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QTime>

class SpeedTest : public QObject
{
    Q_OBJECT

public:
    SpeedTest();

    void start();

private:
    void printHeader();
    void startTests();
    void testFullScreenGrabSpeed();
    void testDefaultLedWidgetsGrabSpeed();

private:
    QFile resultFile;
    QTextStream resultStream;

    QTime time;

    static const int TestTimes;
    static const int LedsCount;
    static const int LedWidth;
    static const int LedHeight;
};

