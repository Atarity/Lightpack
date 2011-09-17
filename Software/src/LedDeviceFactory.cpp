/*
 * LedDeviceFactory.cpp
 *
 *  Created on: 17.04.2011
 *      Author: Timur Sattarov && Mike Shatohin
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

#include <qglobal.h>

#include "LedDeviceFactory.hpp"
#include "LedDeviceLightpack.hpp"
#include "LedDeviceAlienFx.hpp"
#include "LedDeviceVirtual.hpp"
#include "Settings.hpp"


LedDeviceFactory::LedDeviceFactory(QObject *parent)
    : QObject(parent)
{
    m_isSetColorsDone = true;

    m_ledDeviceThread = new QThread();

    initLedDevice();
}

void LedDeviceFactory::recreateLedDevice()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    disconnectSignalSlotsLedDevice();
    m_ledDeviceThread->quit();

    if (m_ledDevice != NULL)
    {
        m_ledDevice->deleteLater();
    }

    initLedDevice();
}


void LedDeviceFactory::setColorsDone()
{
    m_isSetColorsDone = true;
}

void LedDeviceFactory::setColorsIfDeviceAvailable(const QList<QRgb> & colors)
{
    if (m_isSetColorsDone)
    {
        emit setLedDeviceColors(colors);
        m_isSetColorsDone = false;
    } else {
        //qWarning() << Q_FUNC_INFO << "Hey! hey! hey! No so fast!";
    }
}

void LedDeviceFactory::initLedDevice()
{
    m_isSetColorsDone = true;

    m_ledDevice = createLedDevice();
    connectSignalSlotsLedDevice();

    m_ledDevice->moveToThread(m_ledDeviceThread);
    m_ledDeviceThread->start();

    emit offLeds();    
}

ILedDevice * LedDeviceFactory::createLedDevice()
{
    SupportedDevices::DeviceType connectedDevice = Settings::getConnectedDevice();

    if (connectedDevice == SupportedDevices::AlienFxDevice){
#       if !defined(Q_WS_WIN)
        qWarning() << Q_FUNC_INFO << "AlienFx not supported on current platform";

        Settings::setConnectedDevice(SupportedDevices::DefaultDevice);
        connectedDevice = Settings::getConnectedDevice();
#       endif /* Q_WS_WIN */
    }

    switch (connectedDevice){

    case SupportedDevices::LightpackDevice:
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "SupportedDevices::LightpackDevice";
        return (ILedDevice *)new LedDeviceLightpack();

    case SupportedDevices::AlienFxDevice:
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "SupportedDevices::AlienFxDevice";

#       ifdef Q_WS_WIN
        return (ILedDevice *)new LedDeviceAlienFx();
#       else
        break;
#       endif /* Q_WS_WIN */

    case SupportedDevices::VirtualDevice:
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "SupportedDevices::VirtualDevice";
        return (ILedDevice *)new LedDeviceVirtual();
    }

    qFatal("%s %s%d%s", Q_FUNC_INFO,
           "Create LedDevice fail. connectedDevice = '", connectedDevice,
           "'. Application exit.");

    return NULL; // Avoid compiler warning
}

void LedDeviceFactory::connectSignalSlotsLedDevice()
{
    if (m_ledDevice == NULL)
    {
        qWarning() << Q_FUNC_INFO << "m_ledDevice == NULL";
        return;
    }

    connect(m_ledDevice, SIGNAL(firmwareVersion(QString)), this, SIGNAL(firmwareVersion(QString)), Qt::QueuedConnection);
    connect(m_ledDevice, SIGNAL(ioDeviceSuccess(bool)), this, SIGNAL(ioDeviceSuccess(bool)), Qt::QueuedConnection);
    connect(m_ledDevice, SIGNAL(setColorsDone()), this, SLOT(setColorsDone()), Qt::QueuedConnection);
    connect(m_ledDevice, SIGNAL(openDeviceSuccess(bool)), this, SIGNAL(openDeviceSuccess(bool)), Qt::QueuedConnection);

    connect(this, SIGNAL(setLedDeviceColors(QList<QRgb>)), m_ledDevice, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
    connect(this, SIGNAL(offLeds()), m_ledDevice, SLOT(offLeds()), Qt::QueuedConnection);
    connect(this, SIGNAL(setTimerOptions(int,int)), m_ledDevice, SLOT(setTimerOptions(int,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setColorDepth(int)), m_ledDevice, SLOT(setColorDepth(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setSmoothSlowdown(int)), m_ledDevice, SLOT(setSmoothSlowdown(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(requestFirmwareVersion()), m_ledDevice, SLOT(requestFirmwareVersion()), Qt::QueuedConnection);
}

void LedDeviceFactory::disconnectSignalSlotsLedDevice()
{
    if (m_ledDevice == NULL)
    {
        qWarning() << Q_FUNC_INFO << "m_ledDevice == NULL";
        return;
    }

    disconnect(m_ledDevice, SIGNAL(firmwareVersion(QString)), this, SIGNAL(firmwareVersion(QString)));
    disconnect(m_ledDevice, SIGNAL(ioDeviceSuccess(bool)), this, SIGNAL(ioDeviceSuccess(bool)));
    disconnect(m_ledDevice, SIGNAL(setColorsSuccess(bool)), this, SLOT(ioDeviceSuccess(bool)));
    disconnect(m_ledDevice, SIGNAL(openDeviceSuccess(bool)), this, SIGNAL(openDeviceSuccess(bool)));

    disconnect(this, SIGNAL(setLedDeviceColors(QList<QRgb>)), m_ledDevice, SLOT(setColors(QList<QRgb>)));
    disconnect(this, SIGNAL(offLeds()), m_ledDevice, SLOT(offLeds()));
    disconnect(this, SIGNAL(setTimerOptions(int,int)), m_ledDevice, SLOT(setTimerOptions(int,int)));
    disconnect(this, SIGNAL(setColorDepth(int)), m_ledDevice, SLOT(setColorDepth(int)));
    disconnect(this, SIGNAL(setSmoothSlowdown(int)), m_ledDevice, SLOT(setSmoothSlowdown(int)));
    disconnect(this, SIGNAL(requestFirmwareVersion()), m_ledDevice, SLOT(requestFirmwareVersion()));
}
