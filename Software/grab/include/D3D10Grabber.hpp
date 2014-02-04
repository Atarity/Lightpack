/*
 * D3D10Grabber.hpp
 *
 *  Created on: 29.05.2012
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

#pragma once

#include<GrabberBase.hpp>
#ifdef D3D10_GRAB_SUPPORT

#include<QList>
#include<QScopedPointer>

typedef void* (*GetHwndCallback_t)();

class D3D10GrabberImpl;

class D3D10Grabber : public GrabberBase
{
    Q_OBJECT

public:
    D3D10Grabber(QObject * parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabWidgets, GetHwndCallback_t getHwndCb);
    ~D3D10Grabber();

    void init(void);

protected:
    virtual GrabResult _grab();

public slots:
    virtual void updateGrabMonitor(QWidget * firstWidget);
    virtual void startGrabbing();
    virtual void stopGrabbing();
    virtual bool isGrabbingStarted() const;
    virtual void setGrabInterval(int msec);
    virtual void grab();

private:
    QScopedPointer<D3D10GrabberImpl> m_impl;
};

#endif
