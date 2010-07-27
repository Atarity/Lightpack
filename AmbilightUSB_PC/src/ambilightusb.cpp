/*
 * ambilightusb.cpp
 *
 *  Created on: 26.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 */

#include <QtDebug>
#include "ambilightusb.h"

#include "RGB.h"        /* Leds defines, CMD defines */
#include "usbconfig.h"  /* For device VID, PID, vendor name and product name */
#include "hiddata.h"    /* USB HID */


ambilightUsb::ambilightUsb()
{
    openDevice();
    openX11Display();

    clearColorSave();

    for(uint i=0; i<sizeof(write_buffer); i++){
        write_buffer[i] = 0;
    }

    for(uint i=0; i<sizeof(read_buffer); i++){
        read_buffer[i] = 0;
    }
}

ambilightUsb::~ambilightUsb(){
    usbhidCloseDevice(dev);
}

void ambilightUsb::clearColorSave()
{
    for(int i=0; i < 4; i++){
        for(int d=0; d < 3; d++){
            colors_save[i][d] = 0;
        }
    }
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

    if((err = usbhidSetReport(dev, write_buffer, sizeof(write_buffer))) != 0){   /* add a dummy report ID */
        qWarning() << "error writing data:" << usbErrorMessage(err);
        return false;
    }
    return true;
}

bool ambilightUsb::tryToReopenDevice()
{
    qWarning() << "try to reopen device";
    usbhidCloseDevice(dev);
    if(openDevice()){
        qWarning() << "reopen success";
        return true;
    }else{
        qWarning() << "reopen failed";
        return false;
    }
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

bool ambilightUsb::openX11Display()
{
    display = XOpenDisplay( NULL );
    if (display == NULL)
    {
        qFatal("XOpenDisplay returns NULL");
        return false;
    }

    cmap = DefaultColormap(display, DefaultScreen(display));
    root_window = XRootWindow(display, XDefaultScreen(display));

    return true;
}




bool ambilightUsb::updateColorsIfChanges()
{
    if(display == (Display *) NULL){
        qFatal("X11 display didn't open.");
        return false;
    }
    if(dev == NULL){
        qWarning() << "AmbilightUSB device didn't open.";
        if(!tryToReopenDevice()){
            return false;
        }
    }

    XImage *ximage;
    XColor  color;

    //  colors[LED_INDEX][COLOR]
    int colors[4][3] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };
    bool write_colors = false;

    for(int x=0; x < PIXELS_X; x += X_STEP){
        for(int y=0; y < PIXELS_Y; y += Y_STEP){
            for(int led_index=0; led_index < 4; led_index++){

                ximage=XGetImage(display,root_window,
                                 ((led_index==LEFT_UP || led_index==LEFT_DOWN)?x:1279-x),
                                 ((led_index==LEFT_UP || led_index==RIGHT_UP)?y:799-y),
                                 1,1,AllPlanes,ZPixmap);

                color.pixel=XGetPixel(ximage,0,0);

                XQueryColor(display,cmap,&color);

                colors[led_index][R]+=(color.red >> 8);
                colors[led_index][G]+=(color.green >> 8);
                colors[led_index][B]+=(color.blue >> 8);

                XDestroyImage(ximage);

            }
        }
    }

    // Find average for each led color
    for(int led_index=0; led_index < 4; led_index++){
        for(int color=0; color < 3; color++){
            colors[led_index][color] /= PIXELS_COUNT_FOR_EACH_LED;

            // Color depth 15-bit (5-bit on each color)
            // Each led color must be in 0..31
            colors[led_index][color] /= 8;
        }
    }

    for(int led_index=0; led_index < 4; led_index++){
        for(int color=0; color < 3; color++){
            if(colors_save[led_index][color] != colors[led_index][color]){
                write_colors = true;
                colors_save[led_index][color] = colors[led_index][color];
                break;
            }
        }
    }

    if(write_colors){
        write_buffer[1] = CMD_RIGHT_SIDE;
        write_buffer[2] = (unsigned char)colors[RIGHT_UP][R];
        write_buffer[3] = (unsigned char)colors[RIGHT_UP][G];
        write_buffer[4] = (unsigned char)colors[RIGHT_UP][B];

        write_buffer[5] = (unsigned char)colors[RIGHT_DOWN][R];
        write_buffer[6] = (unsigned char)colors[RIGHT_DOWN][G];
        write_buffer[7] = (unsigned char)colors[RIGHT_DOWN][B];

        if(!writeBufferToDeviceWithCheck()){
            return false;
        }

        write_buffer[1] = CMD_LEFT_SIDE;
        write_buffer[2] = (unsigned char)colors[LEFT_UP][R];
        write_buffer[3] = (unsigned char)colors[LEFT_UP][G];
        write_buffer[4] = (unsigned char)colors[LEFT_UP][B];

        write_buffer[5] = (unsigned char)colors[LEFT_DOWN][R];
        write_buffer[6] = (unsigned char)colors[LEFT_DOWN][G];
        write_buffer[7] = (unsigned char)colors[LEFT_DOWN][B];

        if(!writeBufferToDeviceWithCheck()){
            return false;
        }

        write_colors = false;
    }
    return true;
}


void ambilightUsb::offLeds()
{
    write_buffer[1] = CMD_OFF_ALL;
    writeBufferToDevice();
}
