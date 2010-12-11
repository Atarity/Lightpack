/*
 * ledcolors.h
 *
 *  Created on: 26.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: AmbilightUSB
 *
 *  AmbilightUSB is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  AmbilightUSB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  AmbilightUSB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

 
#ifndef LEDCOLORS_H
#define LEDCOLORS_H

#include "RGB.h"
#include "struct_rgb.h"
#include "QtDebug"

struct LedColors
{    
    sRGB *leds[LEDS_COUNT];

    LedColors(){
        for(int i=0; i<LEDS_COUNT; i++){
            leds[i] = new sRGB();
        }
    }

    sRGB * operator[] (int ledIndex){
        if(ledIndex<LEDS_COUNT){
            return leds[ledIndex];
        }else{
            qWarning("sRGB * operator[] (int ledIndex): magic error: ledIndex=%d",ledIndex);
            return new sRGB();
        }
    }
};

#endif // LEDCOLORS_H
