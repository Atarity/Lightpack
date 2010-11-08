/*
 * ledcolors.h
 *
 *  Created on: 26.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 */
 
#ifndef LEDCOLORS_H
#define LEDCOLORS_H

#include "RGB.h"
#include "struct_rgb.h"
#include "QtDebug"

struct LedColors
{    
    sRGB *LeftUp;
    sRGB *LeftDown;
    sRGB *RightUp;
    sRGB *RightDown;

    LedColors(){
        LeftUp = new sRGB();
        LeftDown = new sRGB();
        RightDown = new sRGB();
        RightUp = new sRGB();
    }

    sRGB * operator[] (int ledIndex){
        switch(ledIndex){
        case LEFT_UP: return LeftUp;
        case LEFT_DOWN: return LeftDown;
        case RIGHT_DOWN: return RightDown;
        case RIGHT_UP: return RightUp;
        }
        qWarning("sRGB * operator[] (int ledIndex): magic error: ledIndex=%d",ledIndex);
        return new sRGB();
    }
};

#endif // LEDCOLORS_H
