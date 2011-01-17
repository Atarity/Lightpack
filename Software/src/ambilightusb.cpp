/*
 * ambilightusb.cpp
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


#include "ambilightusb.h"

#include <unistd.h>

#include <QtDebug>

AmbilightUsb::AmbilightUsb(QObject *parent) :
        QObject(parent)
{
    qDebug() << "ambilightUsb(): openDevice()";
    openDevice();

    usb_send_data_timeout_ms = settings->value("UsbSendDataTimeout").toInt();

    memset(write_buffer, 0, sizeof(write_buffer));
    memset(read_buffer, 0, sizeof(read_buffer));

    qDebug() << "ambilightUsb(): initialized";
}

AmbilightUsb::~AmbilightUsb(){
    hid_close(ambilightDevice);
}

bool AmbilightUsb::deviceOpened()
{
    return !(ambilightDevice == NULL);
}

bool AmbilightUsb::openDevice()
{
    ambilightDevice = NULL;
    struct hid_device_info *devs, *cur_dev;

    qDebug("Start enumeration of all HID devices:");
    devs = hid_enumerate(0, 0);
    qDebug() << "hid_enumerate(0,0) done";
    cur_dev = devs;
    while (cur_dev) {        

        int vid = cur_dev->vendor_id;
        int pid = cur_dev->product_id;
        QString path = QString::fromStdString(cur_dev->path);
        QString serial_number = QString::fromWCharArray(cur_dev->serial_number);
        QString manufacturer_string = QString::fromWCharArray(cur_dev->manufacturer_string);
        QString product_string = QString::fromWCharArray(cur_dev->product_string);

        qDebug() << "Found HID:";
        qDebug() << "  VID:" << hex << vid << "; PID:" << pid;
        qDebug() << "  Path:" << path;
        qDebug() << "  Serial number:" << serial_number;
        qDebug() << "  Manufacturer:" << manufacturer_string;
        qDebug() << "  Product:" << product_string;

        if(vid == USB_VENDOR_ID && pid == USB_PRODUCT_ID && product_string == USB_PRODUCT_STRING){
            qDebug() << "Lightpack found";
            ambilightDevice = hid_open_path(cur_dev->path);
            if(ambilightDevice == NULL){
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

    if(ambilightDevice == NULL){
        qWarning("Lightpack device not found");
        emit openDeviceSuccess(false);
        return false;
    }

    hid_set_nonblocking(ambilightDevice, 1);

    emit openDeviceSuccess(true);
    qDebug("Lightpack opened");
    return true;
}

bool AmbilightUsb::readDataFromDevice()
{    
    int bytes_read = hid_read(ambilightDevice, read_buffer, sizeof(read_buffer));

    if(bytes_read < 0){
        qWarning() << "error reading data:" << bytes_read;
        emit readBufferFromDeviceSuccess(false);
        return false;
    }
    emit readBufferFromDeviceSuccess(true);
    return true;
}

bool AmbilightUsb::writeBufferToDevice(int command)
{
    write_buffer[WRITE_BUFFER_INDEX_REPORT_ID] = 0x00;
    write_buffer[WRITE_BUFFER_INDEX_COMMAND] = command;
    int bytes_write = hid_write(ambilightDevice, write_buffer, sizeof(write_buffer));

    if(bytes_write < 0){
        qWarning() << "error writing data:" << bytes_write;
        emit writeBufferToDeviceSuccess(false);
        return false;
    }
    emit writeBufferToDeviceSuccess(true);
    return true;
}

bool AmbilightUsb::tryToReopenDevice()
{
    hid_close(ambilightDevice);
    qWarning() << "try to reopen device";
    if(openDevice()){
        qWarning() << "reopen success";
        return true;
    }else{
        return false;
    }
}

bool AmbilightUsb::readDataFromDeviceWithCheck()
{
    if(ambilightDevice != NULL){
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

bool AmbilightUsb::writeBufferToDeviceWithCheck(int command)
{
    if(ambilightDevice != NULL){
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

QString AmbilightUsb::hardwareVersion()
{
    if(ambilightDevice == NULL){
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
    int hw_major = read_buffer[INDEX_HW_VER_MAJOR +1];
    int hw_minor = read_buffer[INDEX_HW_VER_MINOR +1];
    return QString::number(hw_major) + "." + QString::number(hw_minor);
}

QString AmbilightUsb::firmwareVersion()
{
    if(ambilightDevice == NULL){
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
    int fw_major = read_buffer[INDEX_FW_VER_MAJOR +1];
    int fw_minor = read_buffer[INDEX_FW_VER_MINOR +1];
    return QString::number(fw_major) + "." + QString::number(fw_minor);
}

void AmbilightUsb::offLeds()
{
    qDebug("offLeds()");
    writeBufferToDeviceWithCheck(CMD_OFF_ALL);
}

void AmbilightUsb::smoothChangeColors(int smoothly_delay)
{
    // TODO: add to settings shoothChangeColors state, send to device and load it to form when start application
    write_buffer[WRITE_BUFFER_INDEX_DATA_START] = (char)smoothly_delay;

    writeBufferToDeviceWithCheck(CMD_SMOOTH_CHANGE_COLORS);
}


void AmbilightUsb::setTimerOptions(int prescallerIndex, int outputCompareRegValue)
{
    qDebug("ambilightUsb::setTimerOptions(%d, %d)", prescallerIndex, outputCompareRegValue);

    write_buffer[WRITE_BUFFER_INDEX_DATA_START] = (unsigned char)prescallerIndex;
    write_buffer[WRITE_BUFFER_INDEX_DATA_START+1] = (unsigned char)outputCompareRegValue;


    writeBufferToDeviceWithCheck(CMD_SET_TIMER_OPTIONS);
}

void AmbilightUsb::setColorDepth(int colorDepth)
{
    qDebug("ambilightUsb::setColorDepth(%d)",colorDepth);

    if(colorDepth <= 0){
        qWarning("ambilightUsb::setColorDepth(%d): This is magic, colorDepth <= 0!", colorDepth);
        return;
    }

    write_buffer[WRITE_BUFFER_INDEX_DATA_START] = (unsigned char)colorDepth;

    writeBufferToDeviceWithCheck(CMD_SET_PWM_LEVEL_MAX_VALUE);
}


void AmbilightUsb::setUsbSendDataTimeoutMs(double usb_send_data_timeout_secs)
{
    this->usb_send_data_timeout_ms = (int)(usb_send_data_timeout_secs * 1000.0);
}



void AmbilightUsb::updateColors(LedColors colors)
{
    // Fill write_buffer with new colors for all LEDs

    // First write_buffer[0] == 0x00 - ReportID, i have problems with using it
    // Second byte of usb buffer is command (write_buffer[1] == CMD_UPDATE_LEDS, see below)
    int i = WRITE_BUFFER_INDEX_DATA_START;
    for(int led=0; led < LEDS_COUNT; led++){
        write_buffer[i++] = colors[led]->r;
        write_buffer[i++] = colors[led]->g;
        write_buffer[i++] = colors[led]->b;
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
