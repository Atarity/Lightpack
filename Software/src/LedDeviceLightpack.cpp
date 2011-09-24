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

    // First write_buffer[0] == 0x00 - ReportID, i have problems with using it
    // Second byte of usb buffer is command (write_buffer[1] == CMD_UPDATE_LEDS, see below)
    int index = WRITE_BUFFER_INDEX_DATA_START;

    for (int led = 0; led < LEDS_COUNT; led++)
    {
        // Send colors values
        m_writeBuffer[index++] = qRed  ( colors[led] );
        m_writeBuffer[index++] = qGreen( colors[led] );
        m_writeBuffer[index++] = qBlue ( colors[led] );

        // Send change colors steps
        m_writeBuffer[index++] = 0;
        m_writeBuffer[index++] = 0;
        m_writeBuffer[index++] = 0;
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

void LedDeviceLightpack::setTimerOptions(int prescallerIndex, int outputCompareRegValue)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << prescallerIndex << outputCompareRegValue;

    m_writeBuffer[WRITE_BUFFER_INDEX_DATA_START] = outputCompareRegValue & 0xff;
    m_writeBuffer[WRITE_BUFFER_INDEX_DATA_START+1] = (outputCompareRegValue >> 8);

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

//void LedDeviceLightpack::setBrightness(int value)
//{
//    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;
//    write_buffer[WRITE_BUFFER_INDEX_DATA_START] = (unsigned char)value;
//    bool ok = writeBufferToDeviceWithCheck(CMD_SET_BRIGHTNESS);
//    emit commandCompleted(ok);
//}

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
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    bool ok = openDevice();

    if (ok)
    {
        updateDeviceSettings();
    }

    emit openDeviceSuccess(ok);
}

bool LedDeviceLightpack::openDevice()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_hidDevice = NULL;

    struct hid_device_info *devs, *cur_dev;

    DEBUG_LOW_LEVEL << "Start enumeration of all HID devices";

    devs = hid_enumerate(0, 0);
    cur_dev = devs;

    while (cur_dev)
    {
        int vid = cur_dev->vendor_id;
        int pid = cur_dev->product_id;

        QString manufacturer_string = QString::fromWCharArray(cur_dev->manufacturer_string);
        QString product_string = QString::fromWCharArray(cur_dev->product_string);

        DEBUG_LOW_LEVEL << QString("Found HID: 0x%1 0x%2 %3 %4")
                .arg(pid, 4, 16, QChar('0'))
                .arg(vid, 4, 16, QChar('0'))
                .arg(product_string)
                .arg(manufacturer_string).trimmed();

        if (vid == USB_VENDOR_ID && pid == USB_PRODUCT_ID && product_string == USB_PRODUCT_STRING)
        {
            DEBUG_LOW_LEVEL << "Lightpack found";
            m_hidDevice = hid_open_path(cur_dev->path);

            if (m_hidDevice == NULL)
            {
                qWarning("Lightpack open fail");
                hid_free_enumeration(devs);
                return false;
            }
            break; // device founded break search and go to free enumeration and success signal
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);

    if (m_hidDevice == NULL)
    {
        qWarning("Lightpack device not found");
        return false;
    }
    hid_set_nonblocking(m_hidDevice, 1);

    DEBUG_LOW_LEVEL << "Lightpack opened";
    return true;
}

bool LedDeviceLightpack::readDataFromDevice()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    int bytes_read = hid_read(m_hidDevice, m_readBuffer, sizeof(m_readBuffer));

    if(bytes_read < 0){
        qWarning() << "error reading data:" << bytes_read;
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
            qWarning() << "error writing data:" << error;
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

    hid_close(m_hidDevice);
    qWarning() << "try to reopen device";
    if(openDevice()){
        qWarning() << "reopen success";
        return true;
    }else{
        return false;
    }
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

    setTimerOptions(Settings::getFwTimerPrescallerIndex(), Settings::getFwTimerOCR());
    setColorDepth(Settings::getFwColorDepth());
    setSmoothSlowdown(Settings::getFwSmoothSlowdown());
    //setBrightness
}




