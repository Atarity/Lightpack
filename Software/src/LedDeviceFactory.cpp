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

using namespace SettingsScope;

LedDeviceFactory::LedDeviceFactory(QObject *parent)
    : QObject(parent)
{
    m_isLastCommandCompleted = true;

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
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "m_ledDevice->deleteLater();";
        m_ledDevice->deleteLater();
    }

    initLedDevice();
}

void LedDeviceFactory::setColors(const QList<QRgb> & colors)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << "Is last command completed:" << m_isLastCommandCompleted;

    if (m_isLastCommandCompleted)
    {
        m_isLastCommandCompleted = false;
        emit ledDeviceSetColors(colors);
    } else {
        m_savedColors = colors;
        cmdQueueAppend(LedDeviceCommands::SetColors);
    }
}

void LedDeviceFactory::offLeds()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << "Is last command completed:" << m_isLastCommandCompleted;

    if (m_isLastCommandCompleted)
    {
        m_isLastCommandCompleted = false;
        emit ledDeviceOffLeds();
    } else {
        cmdQueueAppend(LedDeviceCommands::OffLeds);
    }
}

void LedDeviceFactory::setTimerOptions(int prescallerIndex, int outputCompareRegValue)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << prescallerIndex << outputCompareRegValue
                    << "Is last command completed:" << m_isLastCommandCompleted;

    if (m_isLastCommandCompleted)
    {
        m_isLastCommandCompleted = false;
        emit ledDeviceSetTimerOptions(prescallerIndex, outputCompareRegValue);
    } else {
        m_savedTimerPrescallerIndex = prescallerIndex;
        m_savedTimerOCR = outputCompareRegValue;
        cmdQueueAppend(LedDeviceCommands::SetTimerOptions);
    }
}

void LedDeviceFactory::setColorDepth(int value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << value << "Is last command completed:" << m_isLastCommandCompleted;

    if (m_isLastCommandCompleted)
    {
        m_isLastCommandCompleted = false;
        emit ledDeviceSetColorDepth(value);
    } else {
        m_savedColorDepth = value;
        cmdQueueAppend(LedDeviceCommands::SetColorDepth);
    }
}

void LedDeviceFactory::setSmoothSlowdown(int value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << value << "Is last command completed:" << m_isLastCommandCompleted;

    if (m_isLastCommandCompleted)
    {
        m_isLastCommandCompleted = false;
        emit ledDeviceSetSmoothSlowdown(value);
    } else {
        m_savedSmoothSlowdown = value;
        cmdQueueAppend(LedDeviceCommands::SetSmoothSlowdown);
    }
}

void LedDeviceFactory::setGamma(double value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << value << "Is last command completed:" << m_isLastCommandCompleted;

    if (m_isLastCommandCompleted)
    {
        m_isLastCommandCompleted = false;
        emit ledDeviceSetGamma(value);
    } else {
        m_savedGamma = value;
        cmdQueueAppend(LedDeviceCommands::SetGamma);
    }
}

void LedDeviceFactory::requestFirmwareVersion()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << "Is last command completed:" << m_isLastCommandCompleted;

    if (m_isLastCommandCompleted)
    {
        m_isLastCommandCompleted = false;
        emit ledDeviceRequestFirmwareVersion();
    } else {
        cmdQueueAppend(LedDeviceCommands::RequestFirmwareVersion);
    }
}

void LedDeviceFactory::ledDeviceCommandCompleted(bool ok)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << ok;

    if (ok)
    {
        if (m_cmdQueue.isEmpty() == false)
            cmdQueueProcessNext();
        else
            m_isLastCommandCompleted = true;
    }
    else
    {
        m_cmdQueue.clear();
        m_isLastCommandCompleted = true;
    }
}

void LedDeviceFactory::initLedDevice()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_isLastCommandCompleted = true;

    m_ledDevice = createLedDevice();
    connectSignalSlotsLedDevice();

    m_ledDevice->open();

    m_ledDevice->moveToThread(m_ledDeviceThread);
    m_ledDeviceThread->start();
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

    connect(m_ledDevice, SIGNAL(commandCompleted(bool)),        this, SLOT(ledDeviceCommandCompleted(bool)), Qt::QueuedConnection);

    connect(m_ledDevice, SIGNAL(firmwareVersion(QString)),      this, SIGNAL(firmwareVersion(QString)), Qt::QueuedConnection);
    connect(m_ledDevice, SIGNAL(ioDeviceSuccess(bool)),         this, SIGNAL(ioDeviceSuccess(bool)), Qt::QueuedConnection);
    connect(m_ledDevice, SIGNAL(openDeviceSuccess(bool)),       this, SIGNAL(openDeviceSuccess(bool)), Qt::QueuedConnection);

    connect(this, SIGNAL(ledDeviceSetColors(QList<QRgb>)),      m_ledDevice, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceOffLeds()),                   m_ledDevice, SLOT(offLeds()), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceSetTimerOptions(int,int)),    m_ledDevice, SLOT(setTimerOptions(int,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceSetColorDepth(int)),          m_ledDevice, SLOT(setColorDepth(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceSetSmoothSlowdown(int)),      m_ledDevice, SLOT(setSmoothSlowdown(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceSetGamma(double)),            m_ledDevice, SLOT(setGamma(double)), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceRequestFirmwareVersion()),    m_ledDevice, SLOT(requestFirmwareVersion()), Qt::QueuedConnection);
}

void LedDeviceFactory::disconnectSignalSlotsLedDevice()
{
    if (m_ledDevice == NULL)
    {
        qWarning() << Q_FUNC_INFO << "m_ledDevice == NULL";
        return;
    }

    disconnect(m_ledDevice, SIGNAL(commandCompleted(bool)),     this, SLOT(ledDeviceCommandCompleted(bool)));

    disconnect(m_ledDevice, SIGNAL(firmwareVersion(QString)),   this, SIGNAL(firmwareVersion(QString)));
    disconnect(m_ledDevice, SIGNAL(ioDeviceSuccess(bool)),      this, SIGNAL(ioDeviceSuccess(bool)));
    disconnect(m_ledDevice, SIGNAL(openDeviceSuccess(bool)),    this, SIGNAL(openDeviceSuccess(bool)));

    disconnect(this, SIGNAL(ledDeviceSetColors(QList<QRgb>)),   m_ledDevice, SLOT(setColors(QList<QRgb>)));
    disconnect(this, SIGNAL(ledDeviceOffLeds()),                m_ledDevice, SLOT(offLeds()));
    disconnect(this, SIGNAL(ledDeviceSetTimerOptions(int,int)), m_ledDevice, SLOT(setTimerOptions(int,int)));
    disconnect(this, SIGNAL(ledDeviceSetColorDepth(int)),       m_ledDevice, SLOT(setColorDepth(int)));
    disconnect(this, SIGNAL(ledDeviceSetSmoothSlowdown(int)),   m_ledDevice, SLOT(setSmoothSlowdown(int)));
    disconnect(this, SIGNAL(ledDeviceRequestFirmwareVersion()), m_ledDevice, SLOT(requestFirmwareVersion()));
}

void LedDeviceFactory::cmdQueueAppend(LedDeviceCommands::Cmd cmd)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << cmd;

    if (m_cmdQueue.contains(cmd) == false)
    {
        m_cmdQueue.append(cmd);
    }
}

void LedDeviceFactory::cmdQueueProcessNext()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << m_cmdQueue;

    if (m_cmdQueue.isEmpty() == false)
    {
        LedDeviceCommands::Cmd cmd = m_cmdQueue.takeFirst();

        switch(cmd)
        {
        case LedDeviceCommands::OffLeds:
            emit ledDeviceOffLeds();
            break;

        case LedDeviceCommands::SetColors:
            emit ledDeviceSetColors(m_savedColors);
            break;

        case LedDeviceCommands::SetTimerOptions:
            emit ledDeviceSetTimerOptions(m_savedTimerPrescallerIndex, m_savedTimerOCR);
            break;

        case LedDeviceCommands::SetColorDepth:
            emit ledDeviceSetColorDepth(m_savedColorDepth);
            break;

        case LedDeviceCommands::SetSmoothSlowdown:
            emit ledDeviceSetSmoothSlowdown(m_savedSmoothSlowdown);
            break;

        case LedDeviceCommands::SetGamma:
            emit ledDeviceSetGamma(m_savedGamma);
            break;

        case LedDeviceCommands::RequestFirmwareVersion:
            emit ledDeviceRequestFirmwareVersion();
            break;

        default:
            qCritical() << Q_FUNC_INFO << "fail process cmd =" << cmd;
            break;
        }
    }
}


