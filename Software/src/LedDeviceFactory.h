#ifndef LEDDEVICEFACTORY_H
#define LEDDEVICEFACTORY_H

#include "ileddevice.h"

class LedDeviceFactory
{
public:
    static ILedDevice * create(QObject *parent, bool isAlienFx);
};

#endif // LEDDEVICEFACTORY_H
