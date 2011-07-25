
#ifndef WINAPIGRABBER_HPP
#define WINAPIGRABBER_HPP

#include "IGrabber.hpp"
#ifdef WINAPI_GRAB_SUPPORT

#define WINVER 0x0500 /* Windows2000 for MonitorFromWindow(..) func */

#include <windows.h>

class WinAPIGrabber : public IGrabber
{
public:
    WinAPIGrabber();
    ~WinAPIGrabber();
    virtual const char * getName();
    virtual void updateGrabScreenFromWidget( QWidget * widget );
    virtual QList<QRgb> grabWidgetsColors(QList<MoveMeWidget *> &widgets);
private:
    void captureScreen();
    QRgb getColor(const QWidget * grabme);
    QRgb getColor(int x, int y, int width, int height);

private:
    HMONITOR hMonitor;
    bool isBufferNeedsResize;
    MONITORINFO monitorInfo;

    // Size of screen in pixels, initialize in captureScreen() using in getColor()
    unsigned screenWidth;
    unsigned screenHeight;

    // Captured screen buffer, contains actual RGB data in reversed order
    BYTE * pbPixelsBuff;
    unsigned pixelsBuffSize;
    unsigned bytesPerPixel;

    HDC hScreenDC;
    HDC hMemDC;
    HBITMAP hBitmap;

};
#endif
#endif // WINAPIGRABBER_HPP
