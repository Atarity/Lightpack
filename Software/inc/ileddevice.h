#ifndef ILEDDEVICE_H
#define ILEDDEVICE_H

#include <QtGui>
#include "struct_rgb.h"

class ILedDevice
{
public:
    virtual bool openDevice();
    virtual bool deviceOpened();
    virtual QString firmwareVersion();
    virtual void offLeds();

    virtual void updateColors(const QList<StructRGB> & colors);
    virtual void setTimerOptions(int prescallerIndex, int outputCompareRegValue);
    virtual void setColorDepth(int colorDepth);
    virtual void smoothChangeColors(bool isSmooth);

    virtual void openDeviceSuccess(bool isSuccess);
    virtual void ioDeviceSuccess(bool isSuccess);

//    virtual void a();
};

#endif // ILEDDEVICE_H
