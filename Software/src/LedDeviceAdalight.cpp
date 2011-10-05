/*
 * LedDeviceAdalight.cpp
 *
 *  Created on: 06.09.2011
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

#include "LedDeviceAdalight.hpp"
#include "LightpackMath.hpp"
#include "Settings.hpp"
#include "../../CommonHeaders/LEDS_COUNT.h"
#include "debug.h"
#include "stdio.h"

using namespace SettingsScope;

LedDeviceAdalight::LedDeviceAdalight(QObject * parent) : ILedDevice(parent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    for (int i = 0; i < LEDS_COUNT; i++)
    {
        m_colorsSaved << 0;
        m_colorsBuffer << StructRgb();
    }

    // Initialize buffer header
    int ledsCountHi = ((LEDS_COUNT - 1) >> 8) & 0xff;
    int ledsCountLo = (LEDS_COUNT - 1) & 0xff;

    m_writeBufferHeader.append((char)'A');
    m_writeBufferHeader.append((char)'d');
    m_writeBufferHeader.append((char)'a');
    m_writeBufferHeader.append((char)ledsCountHi);
    m_writeBufferHeader.append((char)ledsCountLo);
    m_writeBufferHeader.append((char)(ledsCountHi ^ ledsCountLo ^ 0x55));

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "initialized";
}

LedDeviceAdalight::~LedDeviceAdalight()
{
    if (m_AdalightDevice != NULL)
        m_AdalightDevice->close();

    delete m_AdalightDevice;
}

void LedDeviceAdalight::setColors(const QList<QRgb> & colors)
{
    // Save colors for showing changes of the brightness
    m_colorsSaved = colors;

    LightpackMath::gammaCorrection(m_gamma, colors, m_colorsBuffer);
    LightpackMath::brightnessCorrection(m_brightness, m_colorsBuffer);

    m_writeBuffer.clear();
    m_writeBuffer.append(m_writeBufferHeader);

    for (int i = 0; i < m_colorsBuffer.count(); i++)
    {
        StructRgb color = m_colorsBuffer[i];

        m_writeBuffer.append((color.r & 0x0FF0) >> 4);
        m_writeBuffer.append((color.g & 0x0FF0) >> 4);
        m_writeBuffer.append((color.b & 0x0FF0) >> 4);
    }

    bool ok = writeBuffer(m_writeBuffer);

    emit commandCompleted(ok);
}

void LedDeviceAdalight::offLeds()
{
    m_colorsSaved.clear();

    for (int i = 0; i < LEDS_COUNT; i++)
        m_colorsSaved << 0;

    setColors(m_colorsSaved);
}

void LedDeviceAdalight::setTimerOptions(int /*prescallerIndex*/, int /*outputCompareRegValue*/)
{
    emit commandCompleted(true);
}

void LedDeviceAdalight::setColorDepth(int /*value*/)
{
    emit commandCompleted(true);
}

void LedDeviceAdalight::setSmoothSlowdown(int /*value*/)
{
    emit commandCompleted(true);
}

void LedDeviceAdalight::setGamma(double value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    m_gamma = value;
    emit commandCompleted(true);
}

void LedDeviceAdalight::setBrightness(int percent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << percent;

    m_brightness = percent;
    setColors(m_colorsSaved);
}

void LedDeviceAdalight::requestFirmwareVersion()
{
    emit firmwareVersion("unknown (adalight device)");
    emit commandCompleted(true);
}

void LedDeviceAdalight::open()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_gamma = Settings::getDeviceGamma();
    m_brightness = Settings::getDeviceBrightness();

    m_AdalightDevice = new AbstractSerial();

    m_AdalightDevice->setDeviceName(Settings::getAdalightPort());

    bool ok = m_AdalightDevice->open(AbstractSerial::WriteOnly | AbstractSerial::Unbuffered);

    // Ubuntu 10.04: on every second attempt to open the device leads to failure
    if (ok == false)
    {
        // Try one more time
        ok = m_AdalightDevice->open(AbstractSerial::WriteOnly | AbstractSerial::Unbuffered);
    }

    if (ok)
    {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Serial device" << m_AdalightDevice->deviceName() << "open";

        int baudRateInt = Settings::getAdalightBaudRate();
        AbstractSerial::BaudRate baudrate = AbstractSerial::BaudRate115200;

        if (baudRateInt == 9600)
            baudrate = AbstractSerial::BaudRate9600;
        else if (baudRateInt == 115200)
            baudrate = AbstractSerial::BaudRate115200;
        else
            baudrate = AbstractSerial::BaudRate9600;

        ok = m_AdalightDevice->setBaudRate(baudrate);
        if (ok)
        {
            DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Baud rate  :" << m_AdalightDevice->baudRate();
            DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Data bits  :" << m_AdalightDevice->dataBits();
            DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Parity     :" << m_AdalightDevice->parity();
            DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Stop bits  :" << m_AdalightDevice->stopBits();
            DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Flow       :" << m_AdalightDevice->flowControl();

        } else {
            qWarning() << Q_FUNC_INFO << "Set baud rate" << Settings::getAdalightBaudRate() << "fail";
        }

    } else {
        qWarning() << Q_FUNC_INFO << "Serial device" << m_AdalightDevice->deviceName() << "open fail";
    }

    emit openDeviceSuccess(ok);
}

bool LedDeviceAdalight::writeBuffer(const QByteArray & buff)
{
    printf("\n");
    for (int i = 0; i < buff.count(); i++)
    {
        printf("0x%02x ", buff.at(i) & 0xff);
    }
    printf("\n");
    fflush(stdout);


    if (m_AdalightDevice->isOpen() == false)
        return false;

    int bytes = m_AdalightDevice->write(buff);

    if (bytes != buff.count())
    {
        qWarning() << Q_FUNC_INFO << "bytes != buff.count()";
        return false;
    }

    return true;
}
