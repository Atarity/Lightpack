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

namespace {
    const char bytesPerPixel = 4;

    struct ColorValue {
        int r, g, b;
    };

    static int accumulateBufferFormatArgb(
            const unsigned char *buffer,
            unsigned int pitch,
            const QRect &rect,
            ColorValue *resultColor) {
        register unsigned int r=0, g=0, b=0;
        int count = 0; // count the amount of pixels taken into account
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

        resultColor->r = r;
        resultColor->g = g;
        resultColor->b = b;
        return count;
    }

    static int accumulateBufferFormatAbgr(
            const unsigned char *buffer,
            unsigned int pitch,
            const QRect &rect,
            ColorValue *resultColor) {
        register unsigned int r=0, g=0, b=0;
        int count = 0; // count the amount of pixels taken into account
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

        resultColor->r = r;
        resultColor->g = g;
        resultColor->b = b;
        return count;
    }

    static int accumulateBufferFormatRgba(
            const unsigned char *buffer,
            unsigned int pitch,
            const QRect &rect,
            ColorValue *resultColor) {
        register unsigned int r=0, g=0, b=0;
        int count = 0; // count the amount of pixels taken into account
        for(int currentY = 0; currentY < rect.height(); currentY++) {
            int index = pitch * (rect.y()+currentY) + rect.x()*bytesPerPixel;
            for(int currentX = 0; currentX < rect.width(); currentX += 4) {
                b += buffer[index+1] + buffer[index + 5] + buffer[index + 9 ] + buffer[index + 13];
                g += buffer[index+2] + buffer[index + 6] + buffer[index + 10] + buffer[index + 14];
                r += buffer[index+3] + buffer[index + 7] + buffer[index + 11] + buffer[index + 15];
                count += 4;
                index += bytesPerPixel * 4;
            }
        }

        resultColor->r = r;
        resultColor->g = g;
        resultColor->b = b;
        return count;
    }

    static int accumulateBufferFormatBgra(
            const unsigned char *buffer,
            unsigned int pitch,
            const QRect &rect,
            ColorValue *resultColor) {
        register unsigned int r=0, g=0, b=0;
        int count = 0; // count the amount of pixels taken into account
        for(int currentY = 0; currentY < rect.height(); currentY++) {
            int index = pitch * (rect.y()+currentY) + rect.x()*bytesPerPixel;
            for(int currentX = 0; currentX < rect.width(); currentX += 4) {
                r += buffer[index+1] + buffer[index + 5] + buffer[index + 9 ] + buffer[index + 13];
                g += buffer[index+2] + buffer[index + 6] + buffer[index + 10] + buffer[index + 14];
                b += buffer[index+3] + buffer[index + 7] + buffer[index + 11] + buffer[index + 15];
                count += 4;
                index += bytesPerPixel * 4;
            }
        }

        resultColor->r = r;
        resultColor->g = g;
        resultColor->b = b;
        return count;
    }
} // namespace

namespace Grab {
    namespace Calculations {
        QRgb calculateAvgColor(QRgb *result, const unsigned char *buffer, BufferFormat bufferFormat, unsigned int pitch, const QRect &rect ) {

            Q_ASSERT_X(rect.width() % 4 == 0, "average color calculation", "rect width should be aligned by 4 bytes");

            int count = 0; // count the amount of pixels taken into account
            ColorValue color = {0, 0, 0};

            switch(bufferFormat) {
            case BufferFormatArgb:
                count = accumulateBufferFormatArgb(buffer, pitch, rect, &color);
                break;

            case BufferFormatAbgr:
                count = accumulateBufferFormatAbgr(buffer, pitch, rect, &color);
                break;

            case BufferFormatRgba:
                count = accumulateBufferFormatRgba(buffer, pitch, rect, &color);
                break;

            case BufferFormatBgra:
                count = accumulateBufferFormatBgra(buffer, pitch, rect, &color);
                break;
            default:
                return -1;
                break;
            }

            if ( count > 1 ) {
                color.r = ( color.r / count) & 0xff;
                color.g = ( color.g / count) & 0xff;
                color.b = ( color.b / count) & 0xff;
            }

            *result = qRgb(color.r, color.g, color.b);
            return *result;
        }

        QRgb calculateAvgColor(QList<QRgb> *colors) {
            int r=0, g=0, b=0;
            const int size = colors->size();
            for( int i=0; i < size; i++) {
                const QRgb rgb = colors->at(i);
                r += qRed(rgb);
                g += qGreen(rgb);
                b += qBlue(rgb);
            }
            r = r / size;
            g = g / size;
            b = b / size;
            return qRgb(r, g, b);
        }
    }
}
