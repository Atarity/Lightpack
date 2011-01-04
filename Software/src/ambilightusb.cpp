/*
 * ambilightusb.cpp
 *
 *  Created on: 26.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: AmbilightUSB
 *
 *  AmbilightUSB is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  AmbilightUSB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  AmbilightUSB is distributed in the hope that it will be useful,
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
    unsigned char   rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
    char            vendorName[] = {USB_CFG_VENDOR_NAME, 0}, productName[] = {USB_CFG_DEVICE_NAME, 0};
    int             vid = rawVid[0] + 256 * rawVid[1];
    int             pid = rawPid[0] + 256 * rawPid[1];
    int             err;

    if((err = usbhidOpenDevice(&ambilightDevice, vid, vendorName, pid, productName, 0)) != 0){
        qWarning() << "error finding " << productName << ": " << usbErrorMessage(err);
        emit openDeviceSuccess(false);
        return false;
    }
    qDebug("%s %s (PID: 0x%04x; VID: 0x%04x) opened.", productName, vendorName, pid, vid);
    emit openDeviceSuccess(true);
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

bool AmbilightUsb::writeBufferToDevice()
{
    int err;

    if((err = usbhidSetReport(ambilightDevice, write_buffer, sizeof(write_buffer), usb_send_data_timeout_ms)) != 0){   /* add a dummy report ID */
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

bool AmbilightUsb::writeBufferToDeviceWithCheck()
{
    if(ambilightDevice != NULL){
        if(!writeBufferToDevice()){
            if(tryToReopenDevice()){
                return writeBufferToDevice();
            }else{
                return false;
            }
        }
        return true;
    }else{
        if(tryToReopenDevice()){
            return writeBufferToDevice();
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
    write_buffer[1] = CMD_OFF_ALL;

    write_buffer[9]  = CMD_NOP;
    write_buffer[17] = CMD_NOP;
    write_buffer[25] = CMD_NOP;

    writeBufferToDeviceWithCheck();
}

void AmbilightUsb::smoothChangeColors(int smoothly_delay)
{
    // TODO: add to settings shoothChangeColors state, send to device and load it to form when start application
    write_buffer[1] = CMD_SMOOTH_CHANGE_COLORS;
    write_buffer[2] = (char)smoothly_delay;

    write_buffer[9]  = CMD_NOP;
    write_buffer[17] = CMD_NOP;
    write_buffer[25] = CMD_NOP;

    writeBufferToDeviceWithCheck();
}


void AmbilightUsb::setTimerOptions(int prescallerIndex, int outputCompareRegValue)
{
    qDebug("ambilightUsb::setTimerOptions(%d, %d)", prescallerIndex, outputCompareRegValue);

    // TODO: set names for each index
    write_buffer[1] = CMD_SET_TIMER_OPTIONS;
    write_buffer[2] = (unsigned char)prescallerIndex;
    write_buffer[3] = (unsigned char)outputCompareRegValue;

    write_buffer[9]  = CMD_NOP;
    write_buffer[17] = CMD_NOP;
    write_buffer[25] = CMD_NOP;

    writeBufferToDeviceWithCheck();
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

    write_buffer[9]  = CMD_NOP;
    write_buffer[17] = CMD_NOP;
    write_buffer[25] = CMD_NOP;

    writeBufferToDeviceWithCheck();
}


void AmbilightUsb::setUsbSendDataTimeoutMs(double usb_send_data_timeout_secs)
{
    this->usb_send_data_timeout_ms = (int)(usb_send_data_timeout_secs * 1000.0);
}



void AmbilightUsb::updateColors(LedColors colors)
{
    // Fill write_buffer with new colors for all LEDs

    write_buffer[1] = CMD_LEDS_1_2;
    write_buffer[2] = (unsigned char)colors[LED1]->r;
    write_buffer[3] = (unsigned char)colors[LED1]->g;
    write_buffer[4] = (unsigned char)colors[LED1]->b;

    write_buffer[5] = (unsigned char)colors[LED2]->r;
    write_buffer[6] = (unsigned char)colors[LED2]->g;
    write_buffer[7] = (unsigned char)colors[LED2]->b;
    write_buffer[8] = 0x00;

    write_buffer[9] = CMD_LEDS_3_4;
    write_buffer[10] = (unsigned char)colors[LED3]->r;
    write_buffer[11] = (unsigned char)colors[LED3]->g;
    write_buffer[12] = (unsigned char)colors[LED3]->b;

    write_buffer[13] = (unsigned char)colors[LED4]->r;
    write_buffer[14] = (unsigned char)colors[LED4]->g;
    write_buffer[15] = (unsigned char)colors[LED4]->b;
    write_buffer[16] = 0x00;

    write_buffer[17] = CMD_LEDS_5_6;
    write_buffer[18] = (unsigned char)colors[LED5]->r;
    write_buffer[19] = (unsigned char)colors[LED5]->g;
    write_buffer[20] = (unsigned char)colors[LED5]->b;

    write_buffer[21] = (unsigned char)colors[LED6]->r;
    write_buffer[22] = (unsigned char)colors[LED6]->g;
    write_buffer[23] = (unsigned char)colors[LED6]->b;
    write_buffer[24] = 0x00;

    write_buffer[25] = CMD_LEDS_7_8;
    write_buffer[26] = (unsigned char)colors[LED7]->r;
    write_buffer[27] = (unsigned char)colors[LED7]->g;
    write_buffer[28] = (unsigned char)colors[LED7]->b;

    write_buffer[29] = (unsigned char)colors[LED8]->r;
    write_buffer[30] = (unsigned char)colors[LED8]->g;
    write_buffer[31] = (unsigned char)colors[LED8]->b;
    write_buffer[32] = 0x00;

    writeBufferToDeviceWithCheck();
}
