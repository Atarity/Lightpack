/*
 * calculations.cpp
 *
 *  Created on: 23.07.2012
 *     Project: Lightpack
 *
 *  Copyright (c) 2012 Timur Sattarov
 *
 *  Lightpack a USB content-driving ambient lighting system
 *
 *  Lightpack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Lightpack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "calculations.hpp"

namespace Grab {
    namespace Calculations {

        const char bytesPerPixel = 4;

        QRgb calculateAvgColor(QRgb *result, unsigned char *buffer, BufferFormat bufferFormat, unsigned int pitch, const QRect &rect ) {

            Q_ASSERT_X(rect.width() % 4 == 0, "average color calculation", "rect width should be aligned by 4 bytes");

            int count = 0; // count the amount of pixels taken into account
            register unsigned int r=0, g=0, b=0;

            switch(bufferFormat) {
            case BufferFormatArgb:

                for(int currentY = 0; currentY < rect.height(); currentY++) {
                    int index = pitch * (rect.y()+currentY) + rect.x()*bytesPerPixel;
                    for(int currentX = 0; currentX < rect.width(); currentX += 4) {
                        b += buffer[index]   + buffer[index + 4] + buffer[index + 8 ] + buffer[index + 12];
                        g += buffer[index+1] + buffer[index + 5] + buffer[index + 9 ] + buffer[index + 13];
                        r += buffer[index+2] + buffer[index + 6] + buffer[index + 10] + buffer[index + 14];
                        count += 4;
                        index += bytesPerPixel * 4;
                    }

                }
                break;

            case BufferFormatAbgr:
                for(int currentY = 0; currentY < rect.height(); currentY++) {
                    int index = pitch * (rect.y()+currentY) + rect.x()*bytesPerPixel;
                    for(int currentX = 0; currentX < rect.width(); currentX += 4) {
                        r += buffer[index]   + buffer[index + 4] + buffer[index + 8 ] + buffer[index + 12];
                        g += buffer[index+1] + buffer[index + 5] + buffer[index + 9 ] + buffer[index + 13];
                        b += buffer[index+2] + buffer[index + 6] + buffer[index + 10] + buffer[index + 14];
                        count += 4;
                        index += bytesPerPixel * 4;
                    }
                }
                break;

            default:
                return -1;
                break;
            }

            if ( count > 1 ) {
                r = ( r / count) & 0xff;
                g = ( g / count) & 0xff;
                b = ( b / count) & 0xff;
            }

            *result = qRgb(r,g,b);
            return 0;
        }

        QRgb calculateAvgColor(QList<QRgb> *colors) {
            int r=0, g=0, b=0;
            for( int i=0; i<colors->size(); i++) {
               r += qRed(colors->at(i));
               g += qGreen(colors->at(i));
               b += qBlue(colors->at(i));
            }
            r = r / colors->size();
            g = g / colors->size();
            b = b / colors->size();
            return qRgb(r, g, b);
        }
    }
}
