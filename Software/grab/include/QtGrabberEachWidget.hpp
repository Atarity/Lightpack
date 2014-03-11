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
#ifdef QT_GRAB_SUPPORT

#include "../src/enums.hpp"


using namespace Grab;

class QtGrabberEachWidget : public TimeredGrabber
{
public:
    QtGrabberEachWidget(QObject *parent, GrabberContext * context);
    virtual ~QtGrabberEachWidget();

    DECLARE_GRABBER_NAME("QtGrabberEachWidget")

protected:
    virtual GrabResult _grab(QList<QRgb> &grabResult, const QList<GrabWidget*> &grabWidgets);

private:
    QRgb getColor(const QWidget * w);
};

#endif // QT_GRAB_SUPPORT
