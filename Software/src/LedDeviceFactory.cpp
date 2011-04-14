#include "LedDeviceFactory.h"
#include "lightfx.h"
#include "ambilightusb.h"

ILedDevice * LedDeviceFactory::create(QObject *parent, bool isAlienFx)
{
    return isAlienFx ? (ILedDevice *)new LightFx(parent) : (ILedDevice *)new AmbilightUsb(parent);
}
