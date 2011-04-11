#ifndef ILEDDEVICE_H
#define ILEDDEVICE_H

#include <QtGui>
#include "struct_rgb.h"


class ILedDevice : public QObject
{
    Q_OBJECT
public:
    ILedDevice(QObject * parent) : QObject(parent){}
    virtual bool openDevice()=0;
    virtual bool deviceOpened()=0;
    virtual QString firmwareVersion()=0;
    virtual void offLeds()=0;

public slots:
    virtual void updateColors(const QList<StructRGB> & colors)=0;
    virtual void setTimerOptions(int prescallerIndex, int outputCompareRegValue)=0;
    virtual void setColorDepth(int colorDepth)=0;
    virtual void smoothChangeColors(bool isSmooth)=0;

signals:
    void openDeviceSuccess(bool isSuccess);
    void ioDeviceSuccess(bool isSuccess);

};

#endif // ILEDDEVICE_H
