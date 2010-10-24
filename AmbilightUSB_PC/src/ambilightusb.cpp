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

ambilightUsb::ambilightUsb()
{
    openDevice();

    timeEval = new TimeEvaluations();

    clearColorSave();

    readSettings();    

    memset(write_buffer, 0, sizeof(write_buffer));
    memset(read_buffer, 0, sizeof(read_buffer));
}

ambilightUsb::~ambilightUsb(){
    usbhidCloseDevice(dev);
}

void ambilightUsb::clearColorSave()
{
    for(int i=0; i < LEDS_COUNT; i++){
        for(int d=0; d < 3; d++){
            colors_save[i][d] = 0;
        }
    }
}

void ambilightUsb::readSettings()
{
    ambilight_width = settings->value("WidthAmbilight").toInt();
    ambilight_height = settings->value("HeightAmbilight").toInt();

    usb_send_data_timeout = settings->value("UsbSendDataTimeout").toInt();

    white_balance_r = settings->value("WhiteBalanceCoefRed").toDouble();
    white_balance_g = settings->value("WhiteBalanceCoefGreen").toDouble();
    white_balance_b = settings->value("WhiteBalanceCoefBlue").toDouble();
}

bool ambilightUsb::deviceOpened()
{
    return !(dev == NULL);
}

QString ambilightUsb::usbErrorMessage(int errCode)
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

bool ambilightUsb::openDevice()
{
    dev = NULL;
    unsigned char   rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
    char            vendorName[] = {USB_CFG_VENDOR_NAME, 0}, productName[] = {USB_CFG_DEVICE_NAME, 0};
    int             vid = rawVid[0] + 256 * rawVid[1];
    int             pid = rawPid[0] + 256 * rawPid[1];
    int             err;

    if((err = usbhidOpenDevice(&dev, vid, vendorName, pid, productName, 0)) != 0){
        qWarning() << "error finding " << productName << ": " << usbErrorMessage(err);
        return false;
    }
    qDebug("%s %s (PID: 0x%04x; VID: 0x%04x) opened.", productName, vendorName, pid, vid);
    return true;
}

bool ambilightUsb::readDataFromDevice()
{
    int err;

    int len = sizeof(read_buffer);
    if((err = usbhidGetReport(dev, 0, read_buffer, &len)) != 0){
        qWarning() << "error reading data:" << usbErrorMessage(err);
        return false;
    }
    return true;
}

bool ambilightUsb::writeBufferToDevice()
{
    int err;

    if((err = usbhidSetReport(dev, write_buffer, sizeof(write_buffer), usb_send_data_timeout)) != 0){   /* add a dummy report ID */
        qWarning() << "error writing data:" << usbErrorMessage(err);
        return false;
    }
    return true;
}

bool ambilightUsb::tryToReopenDevice()
{
    qWarning() << "AmbilightUSB device didn't open. Try to reopen device...";    
    usbhidCloseDevice(dev); // TODO: need this?
    if(openDevice()){
        qWarning() << "reopen success";
        return true;
    }else{
        return false;
    }
}

bool ambilightUsb::readDataFromDeviceWithCheck()
{
    if(!readDataFromDevice()){
        if(tryToReopenDevice()){
            // Repeat send buffer:
            return readDataFromDevice();
        }else{
            return false;
        }
    }
    return true;
}

bool ambilightUsb::writeBufferToDeviceWithCheck()
{
    if(!writeBufferToDevice()){
        if(tryToReopenDevice()){
            // Repeat send buffer:
            return writeBufferToDevice();
        }else{
            return false;
        }
    }
    return true;
}

QString ambilightUsb::hardwareVersion()
{
    if(dev == NULL){
        if(!tryToReopenDevice()){
            return QApplication::tr("device unavailable");
        }
    }
    // TODO: write command CMD_GET_VERSION to device
    bool result = readDataFromDevice();
    if(!result){
        return QApplication::tr("read device fail");
    }    

    int major = read_buffer[1];
    int minor = read_buffer[2];
    return QString::number(major) + "." + QString::number(minor);
}

bool ambilightUsb::offLeds()
{
    write_buffer[1] = CMD_OFF_ALL;

    return writeBufferToDeviceWithCheck();
}


bool ambilightUsb::setTimerOptions(enum PRESCALLERS prescaller, int outputCompareRegValue)
{
    // TODO: get names for each index
    write_buffer[1] = CMD_SET_TIMER_OPTIONS;
    write_buffer[2] = (char)prescaller;
    write_buffer[3] = (char)outputCompareRegValue;

    return writeBufferToDeviceWithCheck();
}



//
// Main AmbilightUSB function: update colors if it changes
//
// if error, return number lower than zero;
// else return how long it in ms;
//
double ambilightUsb::updateColorsIfChanges()
{
    timeEval->howLongItStart();
    bool write_colors = false;

    if(dev == NULL){
        if(!tryToReopenDevice()){
            return -2;
        }
    }

    int desktop_width = QApplication::desktop()->width();
    int desktop_height = QApplication::desktop()->height();
    int colors[LEDS_COUNT][3] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };

    for(int led_index=0; led_index<LEDS_COUNT; led_index++){
        QPixmap pix = QPixmap::grabWindow(QApplication::desktop()->winId(),
                                          ((led_index==LEFT_UP || led_index==LEFT_DOWN)?
                                           0 :
                                           (desktop_width-1) - ambilight_width),

                                          ((led_index==LEFT_UP || led_index==RIGHT_UP)?
                                           (desktop_height/2) - ambilight_height :
                                           (desktop_height/2)
                                           ),
                                          ambilight_width, ambilight_height);
        QPixmap scaledPix = pix.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QImage im = scaledPix.toImage();

        colors[led_index][R] = (im.pixel(0,0) >> 0x10) & 0xff;
        colors[led_index][G] = (im.pixel(0,0) >> 0x08) & 0xff;
        colors[led_index][B] = (im.pixel(0,0) >> 0x00) & 0xff;
    }


    // Find average for each led color
    for(int led_index=0; led_index < LEDS_COUNT; led_index++){
        for(int color=0; color < 3; color++){
            // Color depth 15-bit (5-bit on each color)
            // Each led color must be in 0..31
            colors[led_index][color] /= 8;

            //  9.6 mA - all off
            // 90.0 mA - all on
            //colors[led_index][color] = 32;
        }
    }

    // White balance
    for(int led_index=0; led_index < LEDS_COUNT; led_index++){
        colors[led_index][R] *= white_balance_r;
        colors[led_index][G] *= white_balance_g;
        colors[led_index][B] *= white_balance_b;
    }

    for(int led_index=0; led_index < LEDS_COUNT; led_index++){
        for(int color=0; color < 3; color++){
            if(colors_save[led_index][color] != colors[led_index][color]){
                write_colors = true;
                colors_save[led_index][color] = colors[led_index][color];
                break;
            }
        }
    }    


//    double timeLong = timeEval->howLongItEnd();
//    qDebug() << timeLong;
//    usleep(100000);
//
//    return timeLong;

    if(write_colors){
        write_buffer[1] = CMD_RIGHT_SIDE;
        write_buffer[2] = (unsigned char)colors[RIGHT_UP][R];
        write_buffer[3] = (unsigned char)colors[RIGHT_UP][G];
        write_buffer[4] = (unsigned char)colors[RIGHT_UP][B];

        write_buffer[5] = (unsigned char)colors[RIGHT_DOWN][R];
        write_buffer[6] = (unsigned char)colors[RIGHT_DOWN][G];
        write_buffer[7] = (unsigned char)colors[RIGHT_DOWN][B];

        if(!writeBufferToDeviceWithCheck()){
            return -3;
        }

        write_buffer[1] = CMD_LEFT_SIDE;
        write_buffer[2] = (unsigned char)colors[LEFT_UP][R];
        write_buffer[3] = (unsigned char)colors[LEFT_UP][G];
        write_buffer[4] = (unsigned char)colors[LEFT_UP][B];

        write_buffer[5] = (unsigned char)colors[LEFT_DOWN][R];
        write_buffer[6] = (unsigned char)colors[LEFT_DOWN][G];
        write_buffer[7] = (unsigned char)colors[LEFT_DOWN][B];

        if(!writeBufferToDeviceWithCheck()){
            return -3;
        }

        write_colors = false;
    }else{
        // if device disconnected return error code -4
        if(!readDataFromDeviceWithCheck()){
            return -4;
        }
    }

    return timeEval->howLongItEnd();
}
