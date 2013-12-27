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

GrabberBase::GrabberBase(QObject *parent, GrabberContext *grabberContext) : QObject(parent) {
    _context = grabberContext;
}

void GrabberBase::grab(QList<QRgb> &grabResult, const QList<GrabWidget*> &grabWidgets) {
    DEBUG_MID_LEVEL << Q_FUNC_INFO << this->metaObject()->className();
    m_lastGrabResult = _grab(grabResult, grabWidgets);
    emit frameGrabAttempted(m_lastGrabResult);
}
