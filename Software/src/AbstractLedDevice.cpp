/*
 * AbstractLedDevice.cpp
 *
 *  Created on: 05.02.2013
 *     Project: Prismatik
 *
 *  Copyright (c) 2013 Timur Sattarov, tim.helloworld [at] gmail.com
 *
 *  Lightpack is an open-source, USB content-driving ambient lighting
 *  hardware.
 *
 *  Prismatik is a free, open-source software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as published 
 *  by the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Prismatik and Lightpack files is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "AbstractLedDevice.hpp"
#include "colorspace_types.h"
#include "LightpackMath.hpp"
#include "Settings.hpp"

void AbstractLedDevice::setGamma(double value) {
    m_gamma = value;
    setColors(m_colorsSaved);
}
void AbstractLedDevice::setBrightness(int value) {
    m_brightness = value;
    setColors(m_colorsSaved);
}
void AbstractLedDevice::setLuminosityThreshold(int value) {
    m_luminosityThreshold = value;
    setColors(m_colorsSaved);
}
void AbstractLedDevice::setMinimumLuminosityThresholdEnabled(bool value) {
    m_isMinimumLuminosityEnabled = value;
    setColors(m_colorsSaved);
}

void AbstractLedDevice::updateWBAdjustments() {
    using namespace SettingsScope;
    size_t ledsCount = Settings::getNumberOfLeds(Settings::getConnectedDevice());
    m_wbAdjustments.clear();
    for (int i=0; i < ledsCount; i++) {
        WBAdjustment wbAdjustment;
        wbAdjustment.red   = Settings::getLedCoefRed(i);
        wbAdjustment.green = Settings::getLedCoefGreen(i);
        wbAdjustment.blue  = Settings::getLedCoefBlue(i);
        m_wbAdjustments.append(wbAdjustment);
    }
    setColors(m_colorsSaved);
}

void AbstractLedDevice::updateDeviceSettings()
{
    using namespace SettingsScope;
    setGamma(Settings::getDeviceGamma());
    setBrightness(Settings::getDeviceBrightness());
    setLuminosityThreshold(Settings::getLuminosityThreshold());
    setMinimumLuminosityThresholdEnabled(Settings::isMinimumLuminosityEnabled());
    updateWBAdjustments();
}

/*!
  Modifies colors according to gamma, luminosity threshold, white balance and brightness settings
  All modifications are made over extended 12bit RGB, so \code outColors \endcode will contain 12bit
  RGB instead of 8bit.
*/
void AbstractLedDevice::applyColorModifications(const QList<QRgb> &inColors, QList<StructRgb> &outColors) {

    bool isApplyWBAdjustments = m_wbAdjustments.count() == inColors.count();

    StructLab avgColor;
    for(int i = 0; i < inColors.count(); i++) {

        //renormalize to 12bit
        double k = 4095/255.0;
        outColors[i].r = qRed(inColors[i])   * k;
        outColors[i].g = qGreen(inColors[i]) * k;
        outColors[i].b = qBlue(inColors[i])  * k;

        if (isApplyWBAdjustments) {
            outColors[i].r *= m_wbAdjustments[i].red;
            outColors[i].g *= m_wbAdjustments[i].green;
            outColors[i].b *= m_wbAdjustments[i].blue;
        }

        LightpackMath::gammaCorrection(m_gamma, outColors[i]);

        //calculate final avgColor only once
        if (i==0)
            avgColor = LightpackMath::toLab(LightpackMath::avgColor(outColors));

        StructLab lab = LightpackMath::toLab(outColors[i]);
        int dl = m_luminosityThreshold - lab.l;
        if (dl > 0) {
            if (m_isMinimumLuminosityEnabled) { // apply minimum luminosity or dead-zone
                double fadingCoeff = 1+dl*0.7 + (dl > 5 ? (dl - 5)*(dl - 5): 0);
                char da = lab.a - avgColor.a;
                char db = lab.b - avgColor.b;
                lab.l = m_luminosityThreshold;
                lab.a = avgColor.a + round(da/fadingCoeff);
                lab.b = avgColor.b + round(db/fadingCoeff);
                StructRgb rgb = LightpackMath::toRgb(lab);
                outColors[i] = rgb;
            } else {
                outColors[i].r = 0;
                outColors[i].g = 0;
                outColors[i].b = 0;
            }
        }

        LightpackMath::brightnessCorrection(m_brightness, outColors[i]);
    }

}
