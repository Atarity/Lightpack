#ifndef LEDDEVICEFACTORY_H
#define LEDDEVICEFACTORY_H

#include "ILedDevice.hpp"

class LedDeviceFactory
{
public:
    static ILedDevice * create(QObject *parent, bool isAlienFx);
};

#endif // LEDDEVICEFACTORY_H
