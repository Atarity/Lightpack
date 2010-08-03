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

#define TIME_EVAL   0

#if(TIME_EVAL > 0)

#include <sys/time.h>

QStringList messages;
QList<qint64> timevalues;

#define HOW_LONG_IT_START( MESSAGE )    \
{   \
    struct timeval tv; \
    messages.append(QString( MESSAGE )); \
    if(gettimeofday(&tv, 0) < 0){ qWarning() << "Error gettimeofday()"; } \
    else{timevalues.append((tv.tv_sec % 1000)*1000000 + tv.tv_usec);} \
}

#define HOW_LONG_IT_END    \
{   \
    struct timeval tv; \
    if(gettimeofday(&tv, 0) < 0){ qWarning() << "Error gettimeofday()"; } \
    qint64 t = (tv.tv_sec % 1000)*1000000 + tv.tv_usec; \
    qDebug() << "Time:" << messages.last() << "\t=" << \
    ((t - timevalues.last())/1000.0) << "ms";\
    messages.removeLast(); \
    timevalues.removeLast(); \
}
#else
#   define HOW_LONG_IT_START( MESSAGE )
#   define HOW_LONG_IT_END
#endif

ambilightUsb::ambilightUsb()
{
    openDevice();
    openX11Display();

    clearColorSave();

    readSettings();    

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

void ambilightUsb::readSettings()
{
    step_x = settings->value("StepX").toInt();
    step_y = settings->value("StepY").toInt();
    ambilight_width = settings->value("WidthAmbilight").toInt();
    ambilight_height = settings->value("HeightAmbilight").toInt();

    pixels_count_for_each_led = (ambilight_width / step_y) * (ambilight_height / step_x);
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


QString ambilightUsb::hardwareVersion()
{
    if(dev == NULL){
        qWarning() << "AmbilightUSB device didn't open.";
        if(!tryToReopenDevice()){
            return QApplication::tr("open device fail!");
        }
    }
    // TODO: write command CMD_GET_VERSION to device
    bool result = readDataFromDevice();
    if(result){
        int major = read_buffer[1];
        int minor = read_buffer[2];
        return QString::number(major) + "." + QString::number(minor);
    }else{
        qWarning() << "Get hardware version failed!";
        return QString(QApplication::tr("request fail!"));
    }

    // not reachable
}



bool ambilightUsb::updateColorsIfChanges()
{
    HOW_LONG_IT_START("updateColorsIfChanges()");

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

    bool write_colors = false; // false ME!!!

    // 85 ms
    XImage *ximage[4 * pixels_count_for_each_led];
    XColor  xcolors[4 * pixels_count_for_each_led];

    int desktop_width = QApplication::desktop()->width();
    int desktop_height = QApplication::desktop()->height();
    int colors[4][3] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };

    int xcol_indx=0;

    HOW_LONG_IT_START("get and fill colors array");
    for(int led_index=0; led_index < 4; led_index++){
        for(int x=0; x < ambilight_width; x += step_x){
            for(int y=0; y < ambilight_height; y += step_y){

                ximage[xcol_indx]=XGetImage(display,root_window,

                                 ((led_index==LEFT_UP || led_index==LEFT_DOWN)?
                                        x :
                                        (desktop_width-1) - x),

                                 ((led_index==LEFT_UP || led_index==RIGHT_UP)?
                                        (desktop_height/2) - y :
                                        (desktop_height/2) + y
                                      ),
                                 1,1,
                                 AllPlanes,ZPixmap);

                xcolors[xcol_indx].pixel=XGetPixel(ximage[xcol_indx],0,0);

                xcol_indx++;
            }
        }
    }

    XQueryColors(display,cmap,xcolors, 4 * pixels_count_for_each_led);

    for(int led_index=0; led_index<4; led_index++){
        for(int i=0; i < pixels_count_for_each_led; i++){
            colors[led_index][R]+=(xcolors[i + pixels_count_for_each_led * led_index].red >> 8);
            colors[led_index][G]+=(xcolors[i + pixels_count_for_each_led * led_index].green >> 8);
            colors[led_index][B]+=(xcolors[i + pixels_count_for_each_led * led_index].blue >> 8);
        }
    }

    for(int i=0; i < 4 * pixels_count_for_each_led; i++){
        XDestroyImage(ximage[i]);
    }
    HOW_LONG_IT_END;

//    // 120 ms
//    XImage *ximage;
//    XColor  color;
//
//    //  colors[LED_INDEX][COLOR]
//    int colors[4][3] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };
//
//    int desktop_width = QApplication::desktop()->width();
//    int desktop_height = QApplication::desktop()->height();
//
//    for(int x=0; x < ambilight_width; x += step_x){
//        for(int y=0; y < ambilight_height; y += step_y){
//            for(int led_index=0; led_index < 4; led_index++){
//
//                ximage=XGetImage(display,root_window,
//
//                                 ((led_index==LEFT_UP || led_index==LEFT_DOWN)?
//                                        x :
//                                        (desktop_width-1) - x),
//
//                                 ((led_index==LEFT_UP || led_index==RIGHT_UP)?
//                                        (desktop_height/2) - y :
//                                        (desktop_height/2) + y
//                                      ),
//
//                                 1,1,
//                                 AllPlanes,ZPixmap);
//
//                color.pixel=XGetPixel(ximage,0,0);
//
//                XQueryColor(display,cmap,&color);
//
//                colors[led_index][R]+=(color.red >> 8);
//                colors[led_index][G]+=(color.green >> 8);
//                colors[led_index][B]+=(color.blue >> 8);
//
//                XDestroyImage(ximage);
//
//            }
//        }
//    }

    // Find average for each led color
    for(int led_index=0; led_index < 4; led_index++){
        for(int color=0; color < 3; color++){
            colors[led_index][color] /= pixels_count_for_each_led;

            // Color depth 15-bit (5-bit on each color)
            // Each led color must be in 0..31
            colors[led_index][color] /= 8;
        }
    }


    // White balance
    for(int led_index=0; led_index < 4; led_index++){
        // TODO: Add coefs to settings
        colors[led_index][R] *= 0.7;
        colors[led_index][G] *= 1;
        colors[led_index][B] *= 1.2;
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

        HOW_LONG_IT_START("writeBufferToDeviceWithCheck()");
        if(!writeBufferToDeviceWithCheck()){
            return false;
        }
        HOW_LONG_IT_END;

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

    HOW_LONG_IT_END;

    return true;
}


void ambilightUsb::offLeds()
{
    write_buffer[1] = CMD_OFF_ALL;
    writeBufferToDevice();
}
