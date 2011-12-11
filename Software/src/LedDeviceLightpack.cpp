/*
 * LightpackDevice.cpp
 *
 *  Created on: 26.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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

#include "LedDeviceLightpack.hpp"

#include <unistd.h>

#include <QtDebug>
#include "debug.h"
#include "Settings.hpp"

using namespace SettingsScope;

const int LedDeviceLightpack::MaximumLedsCount = MaximumNumberOfLeds::Lightpack6;

LedDeviceLightpack::LedDeviceLightpack(QObject *parent) :
        ILedDevice(parent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << this->thread()->currentThreadId();

    m_hidDevice = NULL;

    memset(m_writeBuffer, 0, sizeof(m_writeBuffer));
    memset(m_readBuffer, 0, sizeof(m_readBuffer));

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "initialized";
}

LedDeviceLightpack::~LedDeviceLightpack()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "hid_close(...);";
    hid_close(m_hidDevice);
}

void LedDeviceLightpack::setColors(const QList<QRgb> & colors)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;
#if 0
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << this->thread()->currentThreadId();
#endif

    resizeColorsBuffer(colors.count());

    // Save colors for showing changes of the brightness
    m_colorsSaved = colors;

    LightpackMath::gammaCorrection(m_gamma, colors, m_colorsBuffer, 4096 /* 12-bit result */);
    LightpackMath::brightnessCorrection(m_brightness, m_colorsBuffer);

    // First write_buffer[0] == 0x00 - ReportID, i have problems with using it
    // Second byte of usb buffer is command (write_buffer[1] == CMD_UPDATE_LEDS, see below)
    int buffIndex = WRITE_BUFFER_INDEX_DATA_START;

    for (int i = 0; i < m_colorsBuffer.count(); i++)
    {
        StructRgb color = m_colorsBuffer[i];

        // Send main 8 bits for compability with existing devices
        m_writeBuffer[buffIndex++] = (color.r & 0x0FF0) >> 4;
        m_writeBuffer[buffIndex++] = (color.g & 0x0FF0) >> 4;
        m_writeBuffer[buffIndex++] = (color.b & 0x0FF0) >> 4;

        // Send over 4 bits for devices revision >= 6
        // All existing devices ignore it
        m_writeBuffer[buffIndex++] = (color.r & 0x000F);
        m_writeBuffer[buffIndex++] = (color.g & 0x000F);
        m_writeBuffer[buffIndex++] = (color.b & 0x000F);
    }

    bool ok = writeBufferToDeviceWithCheck(CMD_UPDATE_LEDS);

    // WARNING: LedDeviceFactory sends data only when the arrival of this signal
    emit commandCompleted(ok);
}

void LedDeviceLightpack::offLeds()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    bool ok = writeBufferToDeviceWithCheck(CMD_OFF_ALL);
    emit commandCompleted(ok);
}

void LedDeviceLightpack::setRefreshDelay(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    m_writeBuffer[WRITE_BUFFER_INDEX_DATA_START] = value & 0xff;
    m_writeBuffer[WRITE_BUFFER_INDEX_DATA_START+1] = (value >> 8);

    bool ok = writeBufferToDeviceWithCheck(CMD_SET_TIMER_OPTIONS);
    emit commandCompleted(ok);
}

void LedDeviceLightpack::setColorDepth(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    m_writeBuffer[WRITE_BUFFER_INDEX_DATA_START] = (unsigned char)value;

    bool ok = writeBufferToDeviceWithCheck(CMD_SET_PWM_LEVEL_MAX_VALUE);
    emit commandCompleted(ok);
}

void LedDeviceLightpack::setSmoothSlowdown(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    m_writeBuffer[WRITE_BUFFER_INDEX_DATA_START] = (unsigned char)value;

    bool ok = writeBufferToDeviceWithCheck(CMD_SET_SMOOTH_SLOWDOWN);
    emit commandCompleted(ok);
}

void LedDeviceLightpack::setGamma(double value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    m_gamma = value;
    setColors(m_colorsSaved);
    // commandCompleted() signal already emited by setColors() slot
}

void LedDeviceLightpack::setBrightness(int percent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << percent;

    m_brightness = percent;
    setColors(m_colorsSaved);
    // commandCompleted() signal already emited by setColors() slot
}

void LedDeviceLightpack::requestFirmwareVersion()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString fwVersion;

    bool ok = readDataFromDeviceWithCheck();

    // TODO: write command CMD_GET_VERSION to device
    if (ok)
    {
        int fw_major = m_readBuffer[INDEX_FW_VER_MAJOR];
        int fw_minor = m_readBuffer[INDEX_FW_VER_MINOR];
        fwVersion = QString::number(fw_major) + "." + QString::number(fw_minor);
    } else {
        fwVersion = QApplication::tr("read device fail");
    }

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Version:" << fwVersion;

    emit firmwareVersion(fwVersion);
    emit commandCompleted(ok);
}

void LedDeviceLightpack::open()
{
    m_hidDevice = NULL;

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << QString("hid_open(0x%1, 0x%2)")
                       .arg(USB_VENDOR_ID, 4, 16, QChar('0'))
                       .arg(USB_PRODUCT_ID, 4, 16, QChar('0'));

    m_hidDevice = hid_open(USB_VENDOR_ID, USB_PRODUCT_ID, NULL);

    if (m_hidDevice == NULL)
    {
        qWarning("Lightpack device not found");
        emit openDeviceSuccess(false);
        return;
    }

    // Immediately return from hid_read() if no data available
    hid_set_nonblocking(m_hidDevice, 1);

    DEBUG_LOW_LEVEL << "Lightpack opened";

    updateDeviceSettings();

    emit openDeviceSuccess(true);
}

bool LedDeviceLightpack::readDataFromDevice()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    int bytes_read = hid_read(m_hidDevice, m_readBuffer, sizeof(m_readBuffer));

    if(bytes_read < 0){
        qWarning() << "Error reading data:" << bytes_read;
        emit ioDeviceSuccess(false);
        return false;
    }
    emit ioDeviceSuccess(true);
    return true;
}

bool LedDeviceLightpack::writeBufferToDevice(int command)
{    
    DEBUG_MID_LEVEL << Q_FUNC_INFO << command;
#if 0
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << this->thread()->currentThreadId();
#endif

    m_writeBuffer[WRITE_BUFFER_INDEX_REPORT_ID] = 0x00;
    m_writeBuffer[WRITE_BUFFER_INDEX_COMMAND] = command;

    int error = hid_write(m_hidDevice, m_writeBuffer, sizeof(m_writeBuffer));
    if (error < 0)
    {
        // Trying to repeat sending data:
        error = hid_write(m_hidDevice, m_writeBuffer, sizeof(m_writeBuffer));
        if(error < 0){
            qWarning() << "Error writing data:" << error;
            emit ioDeviceSuccess(false);
            return false;
        }
    }
    emit ioDeviceSuccess(true);
    return true;
}

bool LedDeviceLightpack::tryToReopenDevice()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    qWarning() << "Try to reopen device";

    hid_close(m_hidDevice);

    open();

    if (m_hidDevice == NULL)
        return false;

    qWarning() << "Reopen success";
    return true;
}

bool LedDeviceLightpack::readDataFromDeviceWithCheck()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if(m_hidDevice != NULL){
        if(!readDataFromDevice()){
            if(tryToReopenDevice()){
                return readDataFromDevice();
            }else{
                return false;
            }
        }
        return true;        
    }else{
        if(tryToReopenDevice()){
            return readDataFromDevice();
        }else{
            return false;
        }
    }
}

bool LedDeviceLightpack::writeBufferToDeviceWithCheck(int command)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if (m_hidDevice != NULL)
    {
        if (!writeBufferToDevice(command))
        {
            if (!writeBufferToDevice(command))
            {
                if (tryToReopenDevice())
                {
                    return writeBufferToDevice(command);
                } else {
                    return false;
                }
            }
        }
        return true;
    } else {
        if (tryToReopenDevice())
        {
            return writeBufferToDevice(command);
        } else {
            return false;
        }
    }
}

void LedDeviceLightpack::updateDeviceSettings()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    setRefreshDelay(Settings::getDeviceRefreshDelay());
    setSmoothSlowdown(Settings::getDeviceSmooth());
    setGamma(Settings::getDeviceGamma());
    setBrightness(Settings::getDeviceBrightness());

    requestFirmwareVersion();
}

void LedDeviceLightpack::resizeColorsBuffer(int buffSize)
{
    if (m_colorsBuffer.count() == buffSize)
        return;

    m_colorsBuffer.clear();

    if (buffSize > MaximumLedsCount)
    {
        qCritical() << Q_FUNC_INFO << "buffSize > MaximumLedsCount" << buffSize << ">" << MaximumLedsCount;

        buffSize = MaximumLedsCount;
    }

    for (int i = 0; i < buffSize; i++)
    {
        m_colorsBuffer << StructRgb();
    }
}
