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

enum GrabResult {
    GrabResultOk,
    GrabResultFrameNotReady,
    GrabResultError
};

class GrabberContext;

class QImage;

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
    virtual void init() = 0;
    virtual void startGrabbing() = 0;
    virtual void stopGrabbing() = 0;
    virtual bool isGrabbingStarted() const = 0;
    virtual void setGrabInterval(int msec) = 0;
    virtual void grab(QList<QRgb> &grabResult, const QList<GrabWidget*> &grabWidgets);

protected slots:
    /*!
      Grab implementation, called from @a GrabberBase#grab() slot, needs to be overriden
     \return GrabResult
    */
    virtual GrabResult _grab(QList<QRgb> &grabResult, const QList<GrabWidget*> &grabWidgets) = 0;

signals:
    void frameGrabAttempted(GrabResult grabResult);

    /*!
      Signals \a GrabManager that the grabber wants to be started or stopped
    */
    void grabberStateChangeRequested(bool isStartRequested);

protected:
    GrabberContext *_context;
    GrabResult m_lastGrabResult;
    QList<QImage *> m_screens;

};
