/*
 * TimeredGrabber.hpp
 *
 *  Created on: 27.08.2012
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


#ifndef TIMEREDGRABBER_HPP
#define TIMEREDGRABBER_HPP

#include <QTimer>
#include "GrabberBase.hpp"

class TimeredGrabber : public GrabberBase
{
    Q_OBJECT
public:
    TimeredGrabber(QObject * parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabAreasGeometry);
    virtual ~TimeredGrabber();

    virtual const char * name() const = 0;
public slots:
    virtual void init();
    virtual GrabResult _grab() = 0;
    virtual void startGrabbing();
    virtual void stopGrabbing();
    virtual bool isGrabbingStarted() const;
    virtual void setGrabInterval(int msec);

protected:
    QTimer *m_timer;
};

#endif // TIMEREDGRABBER_HPP
