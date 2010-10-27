/*
 * ambilightusb.cpp
 *
 *  Created on: 26.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
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
        emit openDeviceError();
        return false;
    }
    qDebug("%s %s (PID: 0x%04x; VID: 0x%04x) opened.", productName, vendorName, pid, vid);
    emit openDeviceSuccess();
    return true;
}

bool AmbilightUsb::readDataFromDevice()
{
    int err;

    int len = sizeof(read_buffer);
    if((err = usbhidGetReport(ambilightDevice, 0, read_buffer, &len)) != 0){
        qWarning() << "error reading data:" << usbErrorMessage(err);
        emit readBufferFromDeviceError();
        return false;
    }
    emit readBufferFromDeviceSuccess();
    return true;
}

bool AmbilightUsb::writeBufferToDevice()
{
    int err;

    if((err = usbhidSetReport(ambilightDevice, write_buffer, sizeof(write_buffer), usb_send_data_timeout_ms)) != 0){   /* add a dummy report ID */
        qWarning() << "error writing data:" << usbErrorMessage(err);
        emit writeBufferToDeviceError();
        return false;
    }
    emit writeBufferToDeviceSuccess();
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
        return readDataFromDevice();
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
        return writeBufferToDevice();
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

    int major = read_buffer[1];
    int minor = read_buffer[2];
    return QString::number(major) + "." + QString::number(minor);
}

void AmbilightUsb::offLeds()
{
    write_buffer[1] = CMD_OFF_ALL;

    writeBufferToDeviceWithCheck();
}


void AmbilightUsb::setTimerOptions(int prescallerIndex, int outputCompareRegValue)
{
    qDebug("ambilightUsb::setTimerOptions(%d, %d)", prescallerIndex, outputCompareRegValue);

    // TODO: set names for each index
    write_buffer[1] = CMD_SET_TIMER_OPTIONS;
    write_buffer[2] = (unsigned char)prescallerIndex;
    write_buffer[3] = (unsigned char)outputCompareRegValue;

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

    writeBufferToDeviceWithCheck();
}


void AmbilightUsb::setUsbSendDataTimeoutMs(double usb_send_data_timeout_secs)
{
    this->usb_send_data_timeout_ms = (int)(usb_send_data_timeout_secs * 1000.0);
}



void AmbilightUsb::updateColors(int colors[LEDS_COUNT][3])
{
    write_buffer[1] = CMD_RIGHT_SIDE;
    write_buffer[2] = (unsigned char)colors[RIGHT_UP][R];
    write_buffer[3] = (unsigned char)colors[RIGHT_UP][G];
    write_buffer[4] = (unsigned char)colors[RIGHT_UP][B];

    write_buffer[5] = (unsigned char)colors[RIGHT_DOWN][R];
    write_buffer[6] = (unsigned char)colors[RIGHT_DOWN][G];
    write_buffer[7] = (unsigned char)colors[RIGHT_DOWN][B];

    writeBufferToDeviceWithCheck();


    write_buffer[1] = CMD_LEFT_SIDE;
    write_buffer[2] = (unsigned char)colors[LEFT_UP][R];
    write_buffer[3] = (unsigned char)colors[LEFT_UP][G];
    write_buffer[4] = (unsigned char)colors[LEFT_UP][B];

    write_buffer[5] = (unsigned char)colors[LEFT_DOWN][R];
    write_buffer[6] = (unsigned char)colors[LEFT_DOWN][G];
    write_buffer[7] = (unsigned char)colors[LEFT_DOWN][B];

    writeBufferToDeviceWithCheck();
}
