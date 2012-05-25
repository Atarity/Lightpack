/*
 * LedDeviceManager.cpp
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

#include "LedDeviceManager.hpp"
#include "LedDeviceLightpack.hpp"
#include "LedDeviceAlienFx.hpp"
#include "LedDeviceAdalight.hpp"
#include "LedDeviceArdulight.hpp"
#include "LedDeviceVirtual.hpp"
#include "Settings.hpp"

using namespace SettingsScope;

LedDeviceManager::LedDeviceManager(QObject *parent)
    : QObject(parent)
{
    m_isLastCommandCompleted = true;

    m_ledDeviceThread = new QThread();

    for (int i = 0; i < SupportedDevices::DevicesCount; i++)
        m_ledDevices.append(NULL);

    initLedDevice();
}

void LedDeviceManager::recreateLedDevice()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    disconnectSignalSlotsLedDevice();

    initLedDevice();
}

void LedDeviceManager::setColors(const QList<QRgb> & colors)
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

void LedDeviceManager::switchOffLeds()
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

void LedDeviceManager::setRefreshDelay(int value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << value
                    << "Is last command completed:" << m_isLastCommandCompleted;

    if (m_isLastCommandCompleted)
    {
        m_isLastCommandCompleted = false;
        emit ledDeviceSetRefreshDelay(value);
    } else {
        m_savedRefreshDelay = value;
        cmdQueueAppend(LedDeviceCommands::SetRefreshDelay);
    }
}

void LedDeviceManager::setColorDepth(int value)
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

void LedDeviceManager::setSmoothSlowdown(int value)
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

void LedDeviceManager::setGamma(double value)
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

void LedDeviceManager::setBrightness(int value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << value << "Is last command completed:" << m_isLastCommandCompleted;

    if (m_isLastCommandCompleted)
    {
        m_isLastCommandCompleted = false;
        emit ledDeviceSetBrightness(value);
    } else {
        m_savedBrightness = value;
        cmdQueueAppend(LedDeviceCommands::SetBrightness);
    }
}

void LedDeviceManager::requestFirmwareVersion()
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

void LedDeviceManager::updateDeviceSettings()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << "Is last command completed:" << m_isLastCommandCompleted;

    if (m_isLastCommandCompleted)
    {
        m_isLastCommandCompleted = false;
        emit ledDeviceUpdateDeviceSettings();
    } else {
        cmdQueueAppend(LedDeviceCommands::UpdateDeviceSettings);
    }
}

void LedDeviceManager::ledDeviceCommandCompleted(bool ok)
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

    emit ioDeviceSuccess(ok);
}

void LedDeviceManager::initLedDevice()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_isLastCommandCompleted = true;

    SupportedDevices::DeviceType connectedDevice = Settings::getConnectedDevice();

    if (m_ledDevices[connectedDevice] == NULL)
    {
        m_ledDevice = m_ledDevices[connectedDevice] = createLedDevice(connectedDevice);

        connectSignalSlotsLedDevice();

        m_ledDevice->moveToThread(m_ledDeviceThread);
        m_ledDeviceThread->start();
    } else {
        disconnectSignalSlotsLedDevice();

        m_ledDevice = m_ledDevices[connectedDevice];

        connectSignalSlotsLedDevice();
    }

    emit ledDeviceOpen();
}

ILedDevice * LedDeviceManager::createLedDevice(SupportedDevices::DeviceType deviceType)
{    

    if (deviceType == SupportedDevices::AlienFxDevice){
#       if !defined(Q_WS_WIN)
        qWarning() << Q_FUNC_INFO << "AlienFx not supported on current platform";

        Settings::setConnectedDevice(SupportedDevices::DefaultDevice);
        deviceType = Settings::getConnectedDevice();
#       endif /* Q_WS_WIN */
    }

    switch (deviceType){

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

    case SupportedDevices::AdalightDevice:
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "SupportedDevices::AdalightDevice";
        return (ILedDevice *)new LedDeviceAdalight();

    case SupportedDevices::ArdulightDevice:
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "SupportedDevices::ArdulightDevice";
        return (ILedDevice *)new LedDeviceArdulight();

    case SupportedDevices::VirtualDevice:
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "SupportedDevices::VirtualDevice";
        return (ILedDevice *)new LedDeviceVirtual();

    default:
        break;
    }

    qFatal("%s %s%d%s", Q_FUNC_INFO,
           "Create LedDevice fail. deviceType = '", deviceType,
           "'. Application exit.");

    return NULL; // Avoid compiler warning
}

void LedDeviceManager::connectSignalSlotsLedDevice()
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
    connect(m_ledDevice, SIGNAL(setColors_VirtualDeviceCallback(QList<QRgb>)), this, SIGNAL(setColors_VirtualDeviceCallback(QList<QRgb>)), Qt::QueuedConnection);    

    connect(this, SIGNAL(ledDeviceOpen()),                      m_ledDevice, SLOT(open()), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceSetColors(QList<QRgb>)),      m_ledDevice, SLOT(setColors(QList<QRgb>)), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceOffLeds()),                   m_ledDevice, SLOT(switchOffLeds()), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceSetRefreshDelay(int)),        m_ledDevice, SLOT(setRefreshDelay(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceSetColorDepth(int)),          m_ledDevice, SLOT(setColorDepth(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceSetSmoothSlowdown(int)),      m_ledDevice, SLOT(setSmoothSlowdown(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceSetGamma(double)),            m_ledDevice, SLOT(setGamma(double)), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceSetBrightness(int)),          m_ledDevice, SLOT(setBrightness(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceRequestFirmwareVersion()),    m_ledDevice, SLOT(requestFirmwareVersion()), Qt::QueuedConnection);
    connect(this, SIGNAL(ledDeviceUpdateDeviceSettings()),      m_ledDevice, SLOT(updateDeviceSettings()), Qt::QueuedConnection);
}

void LedDeviceManager::disconnectSignalSlotsLedDevice()
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

    disconnect(this, SIGNAL(ledDeviceOpen()),                   m_ledDevice, SLOT(open()));
    disconnect(this, SIGNAL(ledDeviceSetColors(QList<QRgb>)),   m_ledDevice, SLOT(setColors(QList<QRgb>)));
    disconnect(this, SIGNAL(ledDeviceOffLeds()),                m_ledDevice, SLOT(switchOffLeds()));
    disconnect(this, SIGNAL(ledDeviceSetRefreshDelay(int)),     m_ledDevice, SLOT(setRefreshDelay(int)));
    disconnect(this, SIGNAL(ledDeviceSetColorDepth(int)),       m_ledDevice, SLOT(setColorDepth(int)));
    disconnect(this, SIGNAL(ledDeviceSetSmoothSlowdown(int)),   m_ledDevice, SLOT(setSmoothSlowdown(int)));
    disconnect(this, SIGNAL(ledDeviceSetGamma(double)),         m_ledDevice, SLOT(setGamma(double)));
    disconnect(this, SIGNAL(ledDeviceSetBrightness(int)),       m_ledDevice, SLOT(setBrightness(int)));
    disconnect(this, SIGNAL(ledDeviceRequestFirmwareVersion()), m_ledDevice, SLOT(requestFirmwareVersion()));
    disconnect(this, SIGNAL(ledDeviceUpdateDeviceSettings()),   m_ledDevice, SLOT(updateDeviceSettings()));
}

void LedDeviceManager::cmdQueueAppend(LedDeviceCommands::Cmd cmd)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << cmd;

    if (m_cmdQueue.contains(cmd) == false)
    {
        m_cmdQueue.append(cmd);
    }
}

void LedDeviceManager::cmdQueueProcessNext()
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

        case LedDeviceCommands::SetRefreshDelay:
            emit ledDeviceSetRefreshDelay(m_savedRefreshDelay);
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

        case LedDeviceCommands::SetBrightness:
            emit ledDeviceSetBrightness(m_savedBrightness);
            break;

        case LedDeviceCommands::RequestFirmwareVersion:
            emit ledDeviceRequestFirmwareVersion();
            break;

        case LedDeviceCommands::UpdateDeviceSettings:
            emit ledDeviceUpdateDeviceSettings();
            break;

        default:
            qCritical() << Q_FUNC_INFO << "fail process cmd =" << cmd;
            break;
        }
    }
}


