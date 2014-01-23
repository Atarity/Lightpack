/*
 * GrabberBase.cpp
 *
 *  Created on: 18.07.2012
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

#include "GrabberBase.hpp"
#include "../src/debug.h"

int validCoord(int a) {
    const unsigned int neg = (1 << 15);
    if (a & neg)
        a = ((~(a-1)) & 0x0000ffff) * -1;
    return a;
}

inline QRect & getValidRect(QRect & rect) {
    int x1,x2,y1,y2;
    rect.getCoords(&x1, &y1, &x2, &y2);
    x1 = validCoord(x1);
    y1 = validCoord(y1);
    rect.setCoords(x1, y1, x1 + rect.width() - 1, y1 + rect.height() - 1);
    return rect;
}

GrabberBase::GrabberBase(QObject *parent, GrabberContext *grabberContext) : QObject(parent) {
    _context = grabberContext;
}

const GrabbedScreen * GrabberBase::screenOfRect(const QRect &rect) const {
    QPoint center = rect.center();
    for (int i = 0; i < _screens.size(); ++i) {
        if (_screens[i].screenInfo.rect.contains(center))
            return &_screens[i];
    }
    for (int i = 0; i < _screens.size(); ++i) {
        if (_screens[i].screenInfo.rect.intersects(rect))
            return &_screens[i];
    }
    return NULL;
}

bool GrabberBase::isReallocationNeeded(const QList< ScreenInfo > &grabScreens) const  {
    if (_screens.size() == 0 || grabScreens.size() != _screens.size())
        return true;

    for (int i = 0; i < grabScreens.size(); ++i) {
        if (grabScreens[i].rect.size() != _screens[i].screenInfo.rect.size())
            return true;
    }
    return false;
}

void GrabberBase::grab() {
    DEBUG_MID_LEVEL << Q_FUNC_INFO << this->metaObject()->className();
    QList< ScreenInfo > grabbedScreens;
    screensToGrab(&grabbedScreens, *_context->grabWidgets);
    if (isReallocationNeeded(grabbedScreens)) {
        if (!reallocate(grabbedScreens)) {
            qCritical() << Q_FUNC_INFO << " couldn't reallocate grabbing buffer";
            emit frameGrabAttempted(GrabResultError);
            return;
        }
    }
    _lastGrabResult = grabScreens();
    if (_lastGrabResult == GrabResultOk) {
        _context->grabResult->clear();

        for (int i = 0; i < _context->grabWidgets->size(); ++i) {
            QRect widgetRect = _context->grabWidgets->at(i)->frameGeometry();
            getValidRect(widgetRect);

            const GrabbedScreen *grabbedScreen = screenOfRect(widgetRect);
            if (grabbedScreen == NULL) {
                DEBUG_HIGH_LEVEL << Q_FUNC_INFO << " widget is out of screen " << Debug::toString(widgetRect);
                _context->grabResult->append(0);
                continue;
            }
            DEBUG_HIGH_LEVEL << Q_FUNC_INFO << Debug::toString(widgetRect);
            QRect monitorRect = grabbedScreen->screenInfo.rect;

            QRect clippedRect = monitorRect.intersected(widgetRect);

            // Checking for the 'grabme' widget position inside the monitor that is used to capture color
            if( !clippedRect.isValid() ){

                DEBUG_MID_LEVEL << "Widget 'grabme' is out of screen:" << Debug::toString(clippedRect);

                _context->grabResult->append(qRgb(0,0,0));
                continue;
            }

            // Convert coordinates from "Main" desktop coord-system to capture-monitor coord-system
            QRect preparedRect = clippedRect.translated(-monitorRect.x(), -monitorRect.y());

            // Align width by 4 for accelerated calculations
            preparedRect.setWidth(preparedRect.width() - (preparedRect.width() % 4));

            if( !preparedRect.isValid() ){
                qWarning() << Q_FUNC_INFO << " preparedRect is not valid:" << Debug::toString(preparedRect);
                // width and height can't be negative

                _context->grabResult->append(qRgb(0,0,0));
                continue;
            }

            using namespace Grab;
            const int bytesPerPixel = 4;
            QRgb avgColor;
            if (_context->grabWidgets->at(i)->isAreaEnabled()) {
                Calculations::calculateAvgColor(&avgColor, grabbedScreen->imgData, grabbedScreen->imgFormat, grabbedScreen->screenInfo.rect.width() * bytesPerPixel, preparedRect );
                _context->grabResult->append(avgColor);
            } else {
                _context->grabResult->append(qRgb(0,0,0));
            }
        }

    }
    emit frameGrabAttempted(_lastGrabResult);
}
