/*
 * GrabberBase.hpp
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

#pragma once

#include <QSharedPointer>
#include <QColor>
#include <QTimer>
#include "../common/defs.h"
#include "../src/GrabWidget.hpp"
#include "calculations.hpp"
#include "GrabberContext.hpp"

enum GrabResult {
    GrabResultOk,
    GrabResultFrameNotReady,
    GrabResultError
};

struct ScreenInfo {
    ScreenInfo()
        : handle(NULL)
    {}
    QRect rect;
    void * handle;
    bool operator== (const ScreenInfo &other) const {
        return other.rect == this->rect;
    }
};

struct GrabbedScreen {
    GrabbedScreen()
        : imgFormat(BufferFormatUnknown)
        , associatedData(NULL)
    {}
    unsigned char * imgData;
    size_t imgDataSize;
    BufferFormat imgFormat;
    ScreenInfo screenInfo;
    void * associatedData;
};

/*!
  Base class which represents each particular grabber. If you want to add a new grabber just add implementation of \code GrabberBase \endcode
  and modify \a GrabManager
*/
class GrabberBase : public QObject
{
    Q_OBJECT
public:

    /*!
     \param parent standart Qt-specific owner
     \param grabResult \code QList \endcode to write results of grabbing to
     \param grabWidgets List of GrabWidgets
    */
    GrabberBase(QObject * parent, GrabberContext * grabberContext);
    virtual ~GrabberBase() {}

    virtual const char * name() const = 0;

public slots:
//    virtual void init() = 0;
    virtual void startGrabbing() = 0;
    virtual void stopGrabbing() = 0;
    virtual bool isGrabbingStarted() const = 0;
    virtual void setGrabInterval(int msec) = 0;

    virtual void grab();

protected slots:
    /*!
      Grabs screens and saves them to \a GrabberBase#_screens field. Called by:wa
      \a GrabberBase#grab() slot. Needs to be implemented in derived classes.
      \return GrabResult
    */
    virtual GrabResult grabScreens() = 0;
    virtual bool reallocate(const QList< ScreenInfo > &grabScreens) = 0;

    virtual QList< ScreenInfo > * screensToGrab(QList< ScreenInfo > * result, const QList<GrabWidget *> &grabWidgets) = 0;
    virtual bool isReallocationNeeded(const QList< ScreenInfo > &grabScreens) const;

protected:
    const GrabbedScreen * screenOfRect(const QRect &rect) const;

signals:
    void frameGrabAttempted(GrabResult grabResult);

    /*!
      Signals \a GrabManager that the grabber wants to be started or stopped
    */
    void grabberStateChangeRequested(bool isStartRequested);

protected:
    GrabberContext *_context;
    GrabResult _lastGrabResult;
    QList<GrabbedScreen> _screens;

};
