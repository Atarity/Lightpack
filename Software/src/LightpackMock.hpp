#ifndef LIGHTPACKMOCK_H
#define LIGHTPACKMOCK_H

#include "ILedDevice.hpp"

class LightpackMock : public ILedDevice
{
public:
    LightpackMock(QObject * parent) : ILedDevice(parent){}

public:
    bool openDevice(){return true;}
    bool deviceOpened(){return true;}
    QString firmwareVersion(){return "unknown";}
    void offLeds(){}

public slots:
    void updateColors(const QList<StructRGB> & /*colors*/){}
    void setTimerOptions(int /*prescallerIndex*/, int /*outputCompareRegValue*/){}
    void setColorDepth(int /*value*/){}
    void setSmoothSlowdown(int /*value*/){}
    void setBrightness(int /*value*/){}

};

#endif // LIGHTPACKMOCK_H
