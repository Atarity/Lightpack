#ifndef LFXAPI_H
#define LFXAPI_H

#include "../alienfx/LFX2.h"

extern LFX2INITIALIZE lfxInitFunction;
extern LFX2RELEASE lfxReleaseFunction;
extern LFX2RESET lfxResetFunction;
extern LFX2UPDATE lfxUpdateFunction;
extern LFX2GETNUMDEVICES lfxGetNumDevicesFunction;
extern LFX2GETDEVDESC lfxGetDeviceDescriptionFunction;
extern LFX2GETNUMLIGHTS lfxGetNumLightsFunction;
extern LFX2SETLIGHTCOL lfxSetLightColorFunction;
extern LFX2GETLIGHTCOL lfxGetLightColorFunction;
extern LFX2GETLIGHTDESC lfxGetLightDescriptionFunction;
extern LFX2LIGHT lfxLightFunction;

#endif //LFXAPI_H
