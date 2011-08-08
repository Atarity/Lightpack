#include"X11Grabber.hpp"

#ifdef X11_GRAB_SUPPORT

#include<cmath>
#include<sys/ipc.h>

X11Grabber::X11Grabber()
{
    this->updateScreenAndAllocateMemory = true;
    this->screen = 0;
    image = NULL;
    display = XOpenDisplay(NULL);
}

X11Grabber::~X11Grabber()
{
    XCloseDisplay(display);
}

const char * X11Grabber::getName()
{
    return "X11Grabber";
}
void X11Grabber::updateGrabScreenFromWidget(QWidget *widget)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;
    updateScreenAndAllocateMemory = true;
    screen = QApplication::desktop()->screenNumber( widget );
}

QList<QRgb> X11Grabber::grabWidgetsColors(QList<MoveMeWidget *> &widgets)
{
    captureScreen();
    QList<QRgb> result;
    for(int i = 0; i < widgets.size(); i++) {
        result.append(getColor(widgets[i]));
    }
    return result;
}

void X11Grabber::captureScreen()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    if( updateScreenAndAllocateMemory ){
        //screenres = QApplication::desktop()->screenGeometry(screen);
        updateScreenAndAllocateMemory = false;

        // todo test and fix dual monitor configuration
        Xscreen = DefaultScreenOfDisplay(display);

        long width=DisplayWidth(display,screen);
        long height=DisplayHeight(display,screen);
	
	DEBUG_HIGH_LEVEL << "dimensions " << width << "x" << height << screen;
        screenres = QRect(0,0,width,height);

	if (image != NULL) {
            XShmDetach(display, &shminfo);
            XDestroyImage(image);
            shmdt (shminfo.shmaddr);
            shmctl(shminfo.shmid, IPC_RMID, 0);
        }
        image = XShmCreateImage(display,   DefaultVisualOfScreen(Xscreen),
                                        DefaultDepthOfScreen(Xscreen),
                                        ZPixmap, NULL, &shminfo,
                                        screenres.width(), screenres.height() );
        uint imagesize;
        imagesize = image->bytes_per_line * image->height;
        shminfo.shmid = shmget(    IPC_PRIVATE,
                                   imagesize,
                                   IPC_CREAT|0777
                                   );

        char* mem = (char*)shmat(shminfo.shmid, 0, 0);
        shminfo.shmaddr = mem;
        image->data = mem;
        shminfo.readOnly = False;

        XShmAttach(display, &shminfo);
    }
    // DEBUG_LOW_LEVEL << "XShmGetImage";
    XShmGetImage(display,
                 RootWindow(display, screen),
                 image,
                 0,
                 0,
                 0x00FFFFFF
                 );
#if 0
    DEBUG_LOW_LEVEL << "QImage";
    QImage *pic = new QImage(1024,768,QImage::Format_RGB32);
    DEBUG_LOW_LEVEL << "format";
    unsigned long pixel;
    for (int y = 0; y < 768; y++)
    {
        for (int x = 0; x < 1024; x++)
        {
            pixel = XGetPixel(image, x, y);
            int r = (pixel >> 16) & 0xff;
            int g = (pixel >>  8) & 0xff;
            int b = (pixel >>  0) & 0xff;
            //QRgb rgb = QRgb() (r,g,b);
            pic->setPixel(x,y,pixel);
        }
    }
    DEBUG_LOW_LEVEL << "save";
    pic->save("/home/atarity/.Lightpack/test.bmp");
#endif
}

QRgb X11Grabber::getColor(const QWidget * grabme)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return getColor(grabme->x(),
                    grabme->y(),
                    grabme->width(),
                    grabme->height());
}

QRgb X11Grabber::getColor(int x, int y, int width, int height)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO
            << "x y w h:" << x << y << width << height;

    // Checking for the 'grabme' widget position inside the monitor that is used to capture color
    if( x + width  < screenres.left()  ||
        x               > screenres.right()  ||
        y + height < screenres.top()    ||
        y               > screenres.bottom() ){

        DEBUG_MID_LEVEL << "Widget 'grabme' is out of screen, x y w h:" << x << y << width << height;

        // Widget 'grabme' is out of screen
        return 0x000000;
    }

    // Convert coordinates from "Main" desktop coord-system to capture-monitor coord-system
    x -= screenres.left() ;
    y -= screenres.top();

    // Ignore part of LED widget which out of screen
    if( x < 0 ) {
        width  += x;  /* reduce width  */
        x = 0;
    }
    if( y < 0 ) {
        height += y;  /* reduce height */
        y = 0;
    }
    if( x + width  > (int)screenres.width()  ) width  -= (x + width ) - screenres.width();
    if( y + height > (int)screenres.height() ) height -= (y + height) - screenres.height();

    //calculate aligned width (align by 4 pixels)
    width = width - (width % 4);

    if(width < 0 || height < 0){
        qWarning() << Q_FUNC_INFO << "width < 0 || height < 0:" << width << height;

        // width and height can't be negative
        return 0x000000;
    }

    register unsigned r=0,g=0,b=0;

    unsigned char *pbPixelsBuff;
    int bytesPerPixel = image->bits_per_pixel / 8;
    pbPixelsBuff = (unsigned char *)image->data;
    int count = 0; // count the amount of pixels taken into account
    for(int j = 0; j < height; j++) {
        int index = image->bytes_per_line * (y+j) + x * bytesPerPixel;
        for(int i = 0; i < width; i+=4) {
            b += pbPixelsBuff[index]   + pbPixelsBuff[index + 4] + pbPixelsBuff[index + 8 ] + pbPixelsBuff[index + 12];
            g += pbPixelsBuff[index+1] + pbPixelsBuff[index + 5] + pbPixelsBuff[index + 9 ] + pbPixelsBuff[index + 13];
            r += pbPixelsBuff[index+2] + pbPixelsBuff[index + 6] + pbPixelsBuff[index + 10] + pbPixelsBuff[index + 14];
            count+=4;
            index += bytesPerPixel * 4;
        }

    }

    if( count != 0 ){
        r = (unsigned)round((double) r / count) & 0xff;
        g = (unsigned)round((double) g / count) & 0xff;
        b = (unsigned)round((double) b / count) & 0xff;
    }

    QRgb result = qRgb(r,g,b);// im.pixel(0,0);

    DEBUG_HIGH_LEVEL << "QRgb result =" << hex << result;

    return result;
}
#endif
