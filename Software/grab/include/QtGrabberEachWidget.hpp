/*
 * QtGrabberEachWidget.hpp
 *
 *  Created on: 22.11.11
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Timur Sattarov, Mike Shatohin
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

#pragma once

#include "TimeredGrabber.hpp"
#include "../src/enums.hpp"

#ifdef QT_GRAB_SUPPORT

using namespace Grab;

class QtGrabberEachWidget : public TimeredGrabber
{
public:
    QtGrabberEachWidget(QObject *parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabAreasGeometry);
    virtual ~QtGrabberEachWidget();
    virtual const char * name() const {
        static char * name = "QtGrabberEachWidget";
        return name;
    }
    virtual void updateGrabMonitor( QWidget * widget );

protected:
    virtual GrabResult _grab();

private:
    QRgb getColor(const QWidget * w);
};

#endif // QT_GRAB_SUPPORT
