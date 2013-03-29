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
#include "LightpackApplication.hpp"

#include <unistd.h>

#include <QtDebug>
#include "debug.h"
#include "Settings.hpp"

using namespace SettingsScope;

const int LedDeviceLightpack::PingDeviceInterval = 1000;
const int LedDeviceLightpack::MaximumLedsCount = MaximumNumberOfLeds::Lightpack6;

LedDeviceLightpack::LedDeviceLightpack(QObject *parent) :
    AbstractLedDevice(parent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << this->thread()->currentThreadId();

    m_hidDevice = NULL;

    memset(m_writeBuffer, 0, sizeof(m_writeBuffer));
    memset(m_readBuffer, 0, sizeof(m_readBuffer));

    m_timerPingDevice = new QTimer(this);

    connect(m_timerPingDevice, SIGNAL(timeout()), this, SLOT(timerPingDeviceTimeout()));
    connect(this, SIGNAL(ioDeviceSuccess(bool)), this, SLOT(restartPingDevice(bool)));
    connect(this, SIGNAL(openDeviceSuccess(bool)), this, SLOT(restartPingDevice(bool)));

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "initialized";
}

LedDeviceLightpack::~LedDeviceLightpack()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "hid_close(...);";
    closeDevice();
}

void LedDeviceLightpack::setColors(const QList<QRgb> & colors)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << hex << (colors.isEmpty() ? -1 : colors.first());
#if 0
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << this->thread()->currentThreadId();
#endif
    if (colors.count() > MaximumLedsCount) {
        qWarning() << Q_FUNC_INFO << "data size is greater than max leds count";

        // skip command with wrong data size
        return;
    }

    QMutexLocker locker(&getLightpackApp()->m_mutex);

    resizeColorsBuffer(colors.count());

    // Save colors for showing changes of the brightness
    m_colorsSaved = colors;

    applyColorModifications(colors, m_colorsBuffer);

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

    locker.unlock();

    bool ok = writeBufferToDeviceWithCheck(CMD_UPDATE_LEDS);

    // WARNING: LedDeviceManager sends data only when the arrival of this signal
    emit commandCompleted(ok);
}

void LedDeviceLightpack::switchOffLeds()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if (m_colorsSaved.count() == 0)
    {
        for (int i = 0; i < MaximumLedsCount; i++)
            m_colorsSaved << 0;
    } else {
        for (int i = 0; i < m_colorsSaved.count(); i++)
            m_colorsSaved[i] = 0;
    }

    setColors(m_colorsSaved);

    // Stop ping device if switchOffLeds() signal comes
    m_timerPingDevice->stop();
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
    emit commandCompleted(true);
}

void LedDeviceLightpack::setSmoothSlowdown(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    m_writeBuffer[WRITE_BUFFER_INDEX_DATA_START] = (unsigned char)value;

    bool ok = writeBufferToDeviceWithCheck(CMD_SET_SMOOTH_SLOWDOWN);
    emit commandCompleted(ok);
}

void LedDeviceLightpack::setColorSequence(QString /*value*/)
{
    emit commandCompleted(true);
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

void LedDeviceLightpack::updateDeviceSettings()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    AbstractLedDevice::updateDeviceSettings();
    setRefreshDelay(Settings::getDeviceRefreshDelay());
    setColorDepth(Settings::getDeviceColorDepth());
    setSmoothSlowdown(Settings::getDeviceSmooth());

    requestFirmwareVersion();
}


void LedDeviceLightpack::open()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if (m_hidDevice != NULL)
    {
        closeDevice();
    }

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << QString("hid_open(0x%1, 0x%2)")
                       .arg(USB_VENDOR_ID, 4, 16, QChar('0'))
                       .arg(USB_PRODUCT_ID, 4, 16, QChar('0'));

    m_hidDevice = hid_open(USB_VENDOR_ID, USB_PRODUCT_ID, NULL);

    if (m_hidDevice == NULL)
    {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Lightpack device not found";
        emit openDeviceSuccess(false);
        return;
    }

    // Immediately return from hid_read() if no data available
    hid_set_nonblocking(m_hidDevice, 1);

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Lightpack opened";

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
    open();

    if (m_hidDevice == NULL)
    {
        return false;
    }

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Reopen success";
    return true;
}

bool LedDeviceLightpack::readDataFromDeviceWithCheck()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if (m_hidDevice != NULL)
    {
        if (!readDataFromDevice())
        {
            if (tryToReopenDevice())
                return readDataFromDevice();
            else
                return false;
        }
        return true;
    } else {
        if (tryToReopenDevice())
            return readDataFromDevice();
        else
            return false;
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
                    return writeBufferToDevice(command);
                else
                    return false;
            }
        }
        return true;
    } else {
        if (tryToReopenDevice())
            return writeBufferToDevice(command);
        else
            return false;
    }
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

void LedDeviceLightpack::closeDevice()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    hid_close(m_hidDevice);
    m_hidDevice = NULL;
}

void LedDeviceLightpack::restartPingDevice(bool isSuccess)
{
    Q_UNUSED(isSuccess);

    if (Settings::isBacklightEnabled() && Settings::isPingDeviceEverySecond())
    {
        // Start ping device with PingDeviceInterval ms after last data transfer complete
        m_timerPingDevice->start(PingDeviceInterval);
    } else {
        m_timerPingDevice->stop();
    }
}

void LedDeviceLightpack::timerPingDeviceTimeout()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if (m_hidDevice == NULL)
    {
        DEBUG_MID_LEVEL << Q_FUNC_INFO << "hid_open";
        m_hidDevice = hid_open(USB_VENDOR_ID, USB_PRODUCT_ID, NULL);

        if (m_hidDevice == NULL)
        {
            DEBUG_MID_LEVEL << Q_FUNC_INFO << "hid_open fail";
            emit openDeviceSuccess(false);
            return;
        }
        DEBUG_MID_LEVEL << Q_FUNC_INFO << "hid_open ok";

        emit openDeviceSuccess(true);
        closeDevice(); // device should be opened by open() function
        return;
    }

    DEBUG_MID_LEVEL << Q_FUNC_INFO << "hid_write";

    m_writeBuffer[WRITE_BUFFER_INDEX_REPORT_ID] = 0x00;
    m_writeBuffer[WRITE_BUFFER_INDEX_COMMAND] = CMD_NOP;
    int bytes = hid_write(m_hidDevice, m_writeBuffer, sizeof(m_writeBuffer));

    if (bytes < 0)
    {
        DEBUG_MID_LEVEL << Q_FUNC_INFO << "hid_write fail";
        closeDevice();
        emit ioDeviceSuccess(false);
        return;
    }

    DEBUG_MID_LEVEL << Q_FUNC_INFO << "hid_write ok";

    emit ioDeviceSuccess(true);
}
