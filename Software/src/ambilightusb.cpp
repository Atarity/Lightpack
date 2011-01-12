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
    usbhidCloseDevice(ambilightDevice);
}

bool AmbilightUsb::deviceOpened()
{
    return !(ambilightDevice == NULL);
}

QString AmbilightUsb::usbErrorMessage(int errCode)
{
    QString result = "";
    switch(errCode){
    case USBOPEN_ERR_ACCESS:      result = "Access to device denied"; break;
    case USBOPEN_ERR_NOTFOUND:    result = "The specified device was not found"; break;
    case USBOPEN_ERR_IO:          result = "Communication error with device"; break;
    default:
        result = result.sprintf("Unknown USB error %d", errCode);
        break;
    }
    return result;
}

bool AmbilightUsb::openDevice()
{
    ambilightDevice = NULL;
    int vid = USB_VENDOR_ID;
    int pid = USB_PRODUCT_ID;
    int err;


    // TODO: add strings with this:
    //    iManufacturer           1 brunql.github.com
    //    iProduct                2 Lightpack
    // And check it in usbhidOpenDevice(...)
    if((err = usbhidOpenDevice(&ambilightDevice, vid, NULL, pid, NULL, 0)) != 0){
        qWarning() << "error finding " << ": " << usbErrorMessage(err);
        emit openDeviceSuccess(false);
        return false;
    }
    emit openDeviceSuccess(true);
    qDebug("Lightpack (PID: 0x%04x; VID: 0x%04x) opened.", pid, vid);
    return true;
}

bool AmbilightUsb::readDataFromDevice()
{
    int err;

    int len = sizeof(read_buffer);
    if((err = usbhidGetReport(ambilightDevice, 0, read_buffer, &len)) != 0){
        qWarning() << "error reading data:" << usbErrorMessage(err);
        emit readBufferFromDeviceSuccess(false);
        return false;
    }
    emit readBufferFromDeviceSuccess(true);
    return true;
}

bool AmbilightUsb::writeBufferToDevice(int reportId)
{
    int err = usbhidSetReport(ambilightDevice, reportId, write_buffer, sizeof(write_buffer));

    if(err != 0){
        qWarning() << "error writing data:" << usbErrorMessage(err);
        emit writeBufferToDeviceSuccess(false);
        return false;
    }
    emit writeBufferToDeviceSuccess(true);
    return true;
}

bool AmbilightUsb::tryToReopenDevice()
{
    qWarning() << "AmbilightUSB device didn't open. Try to reopen device...";
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

bool AmbilightUsb::writeBufferToDeviceWithCheck(int reportId)
{
    if(ambilightDevice != NULL){
        if(!writeBufferToDevice(reportId)){
            if(tryToReopenDevice()){
                return writeBufferToDevice(reportId);
            }else{
                return false;
            }
        }
        return true;
    }else{
        if(tryToReopenDevice()){
            return writeBufferToDevice(reportId);
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
    writeBufferToDeviceWithCheck(CMD_OFF_ALL);
}

void AmbilightUsb::smoothChangeColors(int smoothly_delay)
{
    // TODO: add to settings shoothChangeColors state, send to device and load it to form when start application
    write_buffer[1] = CMD_SMOOTH_CHANGE_COLORS;
    write_buffer[2] = (char)smoothly_delay;

    writeBufferToDeviceWithCheck(CMD_NOP);
}


void AmbilightUsb::setTimerOptions(int prescallerIndex, int outputCompareRegValue)
{
    qDebug("ambilightUsb::setTimerOptions(%d, %d)", prescallerIndex, outputCompareRegValue);

    // TODO: set names for each index
    write_buffer[1] = CMD_SET_TIMER_OPTIONS;
    write_buffer[2] = (unsigned char)prescallerIndex;
    write_buffer[3] = (unsigned char)outputCompareRegValue;


    writeBufferToDeviceWithCheck(CMD_NOP);
}

void AmbilightUsb::setColorDepth(int colorDepth)
{
    qDebug("ambilightUsb::setColorDepth(%d)",colorDepth);

    if(colorDepth <= 0){
        qWarning("ambilightUsb::setColorDepth(%d): This is magic, colorDepth <= 0!", colorDepth);
        return;
    }

    // TODO: set names for each index
    write_buffer[1] = CMD_SET_PWM_LEVEL_MAX_VALUE;
    write_buffer[2] = (unsigned char)colorDepth;

    writeBufferToDeviceWithCheck(CMD_NOP);
}


void AmbilightUsb::setUsbSendDataTimeoutMs(double usb_send_data_timeout_secs)
{
    this->usb_send_data_timeout_ms = (int)(usb_send_data_timeout_secs * 1000.0);
}



void AmbilightUsb::updateColors(LedColors colors)
{
    // Fill write_buffer with new colors for all LEDs
    int i=0;
    for(int led=0; led < LEDS_COUNT; led++){
        write_buffer[i++] = colors[led]->r;
        write_buffer[i++] = colors[led]->g;
        write_buffer[i++] = colors[led]->b;
    }

    writeBufferToDeviceWithCheck(CMD_UPDATE_LEDS);
}
