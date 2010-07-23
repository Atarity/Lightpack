/*
 * main.c
 *
 *  Created on: 21.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 *          MCU: ATtiny44
 *        Clock: 12MHz
 */

#include <QtCore>
#include <X11/Xutil.h>

#include "main.h"       /* Pixel counts defines */
#include "RGB.h"        /* Leds defines, CMD defines */
#include "usbconfig.h"  /* For device VID, PID, vendor name and product name */
#include "hiddata.h"

//
// Global variables
//

usbDevice_t *dev;
char read_buffer[8 + 1];    /* room for dummy report ID */
char write_buffer[1 + 7];


static void showHelpMessage(char *myName)
{
    fprintf(stderr, "Project : AmbilightUSB \n");
    fprintf(stderr, "Author  : brunql \n");
    fprintf(stderr, "Version : 2.0 \n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s --help  - show this help \n", myName);
    fprintf(stderr, "  %s         - start application \n", myName);
    fprintf(stderr, "  %s off     - turn off all leds \n", myName);
    fprintf(stderr, "  sudo %s    - if can't open device, try this \n", myName);
}

static char *usbErrorMessage(int errCode)
{
    static char buffer[80];

    switch(errCode){
    case USBOPEN_ERR_ACCESS:      return (char *)"Access to device denied";
    case USBOPEN_ERR_NOTFOUND:    return (char *)"The specified device was not found";
    case USBOPEN_ERR_IO:          return (char *)"Communication error with device";
    default:
        sprintf(buffer, "Unknown USB error %d", errCode);
        return buffer;
    }
    return NULL;    /* not reached */
}

static usbDevice_t  *openDevice(void)
{
    usbDevice_t     *dev = NULL;
    unsigned char   rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
    char            vendorName[] = {USB_CFG_VENDOR_NAME, 0}, productName[] = {USB_CFG_DEVICE_NAME, 0};
    int             vid = rawVid[0] + 256 * rawVid[1];
    int             pid = rawPid[0] + 256 * rawPid[1];
    int             err;

    if((err = usbhidOpenDevice(&dev, vid, vendorName, pid, productName, 0)) != 0){
        fprintf(stderr, "error finding %s: %s\n", productName, usbErrorMessage(err));
        return NULL;
    }
    fprintf(stdout, "Device %s %s (PID: 0x%04x; VID: 0x%04x): opened successful!\n", productName, vendorName, pid, vid);
    return dev;
}

void readData(void)
{
    int err;

    int len = sizeof(read_buffer);
    if((err = usbhidGetReport(dev, 0, read_buffer, &len)) != 0){
        fprintf(stderr, "error reading data: %s\n", usbErrorMessage(err));
    }
}

void writeBuffer(void)
{
    int err;

    while((err = usbhidSetReport(dev, write_buffer, sizeof(write_buffer))) != 0)   /* add a dummy report ID */
    {
        fprintf(stderr, "error writing data: %s\n", usbErrorMessage(err));
        while((dev = openDevice()) == NULL){
            printf("Retry openDevice()...\n");
            sleep(1);
        }
    }
}

void tryOpenDevice(void)
{
    while((dev = openDevice()) == NULL){
        printf("Retry...\n");
        sleep(1);
    }
}

int main(int argc, char **argv)
{
    if(argc == 1){
        tryOpenDevice();

        memset(write_buffer, 0, sizeof(write_buffer));

        Display *display = XOpenDisplay((char *) NULL);
        if (display == (Display *) NULL)
        {
            fprintf(stderr, "XOpenDisplay failed!'n");
            exit(1);
        }

        Colormap cmap = DefaultColormap(display, DefaultScreen(display));
        Window root_window = XRootWindow(display, XDefaultScreen(display));

        //  colors[LED_INDEX][COLOR]
        int colors[4][3]        = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };
        int colors_save[4][3]   = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };

        bool write_colors = true;

        while(1){

            XImage *ximage;
            XColor  color;


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
                    //colors[led_index][i] *= 60;
                    colors[led_index][color] /= PIXELS_COUNT_FOR_EACH_LED;
//                    if(colors[led_index][i] > 0xff){
//                        printf("%d ", colors[led_index][i]);
//                    }
                    //colors[led_index][i] /= 256;
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

                writeBuffer();

                write_buffer[1] = CMD_LEFT_SIDE;
                write_buffer[2] = (unsigned char)colors[LEFT_UP][R];
                write_buffer[3] = (unsigned char)colors[LEFT_UP][G];
                write_buffer[4] = (unsigned char)colors[LEFT_UP][B];

                write_buffer[5] = (unsigned char)colors[LEFT_DOWN][R];
                write_buffer[6] = (unsigned char)colors[LEFT_DOWN][G];
                write_buffer[7] = (unsigned char)colors[LEFT_DOWN][B];

                writeBuffer();

                write_colors = false;
            }


            ////////////////////////////////////
            // // QPixmap::grabWindow - SLOW!!!
            // QPixmap originalPixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
            // QImage im = originalPixmap.toImage();
            // QRgb rgb = im.pixel(x, y); // #AARRGGBB
            // r += ((rgb & 0x00ff0000) >> 16);
            // g += ((rgb & 0x0000ff00) >> 8);
            // b += (rgb & 0x000000ff);
            ////////////////////////////////////


        }

    }else if(strcasecmp(argv[1], "off") == 0){
        tryOpenDevice();
        memset(write_buffer, 0, sizeof(write_buffer));
        write_buffer[1] = CMD_OFF_ALL;
        writeBuffer();
    }else{
        showHelpMessage(argv[0]);
        exit(1);
    }

    usbhidCloseDevice(dev);
    return 0;
}

/* ------------------------------------------------------------------------- */
