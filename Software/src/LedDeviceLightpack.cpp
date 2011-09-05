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

LedDeviceLightpack::LedDeviceLightpack(QObject *parent) :
        ILedDevice(parent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << this->thread()->currentThreadId();

    openDevice();


    memset(write_buffer, 0, sizeof(write_buffer));
    memset(read_buffer, 0, sizeof(read_buffer));

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "initialized";
}

LedDeviceLightpack::~LedDeviceLightpack()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "hid_close(ambilightDevice);";
    hid_close(hidDevice);
}

bool LedDeviceLightpack::deviceOpened()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    return !(hidDevice == NULL);
}

bool LedDeviceLightpack::openDevice()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    hidDevice = NULL;

    struct hid_device_info *devs, *cur_dev;

    DEBUG_LOW_LEVEL << "Start enumeration of all HID devices";
    devs = hid_enumerate(0, 0);
    cur_dev = devs;
    while (cur_dev) {
        int vid = cur_dev->vendor_id;
        int pid = cur_dev->product_id;

        QString manufacturer_string = QString::fromWCharArray(cur_dev->manufacturer_string);
        QString product_string = QString::fromWCharArray(cur_dev->product_string);

        DEBUG_LOW_LEVEL << QString("Found HID: 0x%1 0x%2 %3 %4")
                .arg(pid, 4, 16, QChar('0'))
                .arg(vid, 4, 16, QChar('0'))
                .arg(product_string)
                .arg(manufacturer_string).trimmed();

        if(vid == USB_VENDOR_ID && pid == USB_PRODUCT_ID && product_string == USB_PRODUCT_STRING){
            DEBUG_LOW_LEVEL << "Lightpack found";
            hidDevice = hid_open_path(cur_dev->path);
            if(hidDevice == NULL){
                qWarning("Lightpack open fail");

                hid_free_enumeration(devs);

                emit openDeviceSuccess(false);
                return false;
            }
            break; // device founded break search and go to free enumeration and success signal
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);

    if(hidDevice == NULL){
        qWarning("Lightpack device not found");
        emit openDeviceSuccess(false);
        return false;
    }

    hid_set_nonblocking(hidDevice, 1);

    emit openDeviceSuccess(true);
    DEBUG_LOW_LEVEL << "Lightpack opened";
    return true;
}

bool LedDeviceLightpack::readDataFromDevice()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    int bytes_read = hid_read(hidDevice, read_buffer, sizeof(read_buffer));

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

    write_buffer[WRITE_BUFFER_INDEX_REPORT_ID] = 0x00;
    write_buffer[WRITE_BUFFER_INDEX_COMMAND] = command;
    int bytes_write = hid_write(hidDevice, write_buffer, sizeof(write_buffer));

    if(bytes_write < 0){
        qWarning() << "error writing data:" << bytes_write;
        emit ioDeviceSuccess(false);
        return false;
    }
    emit ioDeviceSuccess(true);
    return true;
}

bool LedDeviceLightpack::tryToReopenDevice()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    hid_close(hidDevice);
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
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if(hidDevice != NULL){
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

    if(hidDevice != NULL){
        if(!writeBufferToDevice(command)){
            if(!writeBufferToDevice(command)){
                if(tryToReopenDevice()){
                    return writeBufferToDevice(command);
                }else{
                    return false;
                }
            }
        }
        return true;
    }else{
        if(tryToReopenDevice()){
            return writeBufferToDevice(command);
        }else{
            return false;
        }
    }
}

QString LedDeviceLightpack::firmwareVersion()
{
    DEBUG_OUT << Q_FUNC_INFO;

    if(hidDevice == NULL){
        if(!tryToReopenDevice()){
            return QApplication::tr("device unavailable");
        }
    }
    // TODO: write command CMD_GET_VERSION to device
    bool result = readDataFromDeviceWithCheck();
    if(!result){
        return QApplication::tr("read device fail");
    }

    // read_buffer[0] - report ID, skip it by +1
    int fw_major = read_buffer[INDEX_FW_VER_MAJOR];
    int fw_minor = read_buffer[INDEX_FW_VER_MINOR];
    QString firmwareVer = QString::number(fw_major) + "." + QString::number(fw_minor);

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << firmwareVer;

    return firmwareVer;
}

void LedDeviceLightpack::offLeds()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    writeBufferToDeviceWithCheck(CMD_OFF_ALL);
}

void LedDeviceLightpack::setTimerOptions(int prescallerIndex, int outputCompareRegValue)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << prescallerIndex << outputCompareRegValue;

    write_buffer[WRITE_BUFFER_INDEX_DATA_START] = outputCompareRegValue & 0xff;
    write_buffer[WRITE_BUFFER_INDEX_DATA_START+1] = (outputCompareRegValue >> 8);

    writeBufferToDeviceWithCheck(CMD_SET_TIMER_OPTIONS);
}

void LedDeviceLightpack::setColorDepth(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    write_buffer[WRITE_BUFFER_INDEX_DATA_START] = (unsigned char)value;

    writeBufferToDeviceWithCheck(CMD_SET_PWM_LEVEL_MAX_VALUE);
}

void LedDeviceLightpack::setSmoothSlowdown(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    write_buffer[WRITE_BUFFER_INDEX_DATA_START] = (unsigned char)value;

    writeBufferToDeviceWithCheck(CMD_SET_SMOOTH_SLOWDOWN);
}

//void LedDeviceLightpack::setBrightness(int value)
//{
//    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;
//    write_buffer[WRITE_BUFFER_INDEX_DATA_START] = (unsigned char)value;
//    writeBufferToDeviceWithCheck(CMD_SET_BRIGHTNESS);
//}



void LedDeviceLightpack::updateColors(const QList<QRgb> & colors)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;
#if 0
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "thread id: " << this->thread()->currentThreadId();
#endif

    // Fill write_buffer with new colors for all LEDs

    // First write_buffer[0] == 0x00 - ReportID, i have problems with using it
    // Second byte of usb buffer is command (write_buffer[1] == CMD_UPDATE_LEDS, see below)
    int i = WRITE_BUFFER_INDEX_DATA_START;
    for(int led=0; led < LEDS_COUNT; led++){
        // Send colors values
        write_buffer[i++] = qRed  ( colors[led] );
        write_buffer[i++] = qGreen( colors[led] );
        write_buffer[i++] = qBlue ( colors[led] );

        // Send change colors steps
        write_buffer[i++] = 0;
        write_buffer[i++] = 0;
        write_buffer[i++] = 0;
    }

#if 0
    QString res = "";
    for(unsigned i=0; i<sizeof(write_buffer); i++){
        res += QString().sprintf("%02x", write_buffer[i]);
    }
    qDebug() << "write:" << res;
#endif

    writeBufferToDeviceWithCheck(CMD_UPDATE_LEDS);

#if 0
    usleep(100*1000);

    memset(read_buffer,0x00,sizeof(read_buffer));

    readDataFromDevice();
    res = "";
    for(unsigned i=0; i<sizeof(read_buffer); i++){
        res += QString().sprintf("%02x", read_buffer[i]);
    }
    qDebug() << "read :" << res;
#endif
}
