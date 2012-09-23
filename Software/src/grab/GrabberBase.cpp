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
#include "debug.h"

GrabberBase::GrabberBase(QObject *parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabWidgets) : QObject(parent) {
    m_grabResult = grabResult;
    m_grabWidgets = grabWidgets;
}

void GrabberBase::grab() {
    DEBUG_MID_LEVEL << Q_FUNC_INFO;
    m_lastGrabResult = _grab();
    emit frameGrabAttempted(m_lastGrabResult);
}
