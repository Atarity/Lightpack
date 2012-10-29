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

#include <QColor>
#include <QTimer>
#include "../common/defs.h"
#include "GrabWidget.hpp"

enum GrabResult {
    GrabResultOk,
    GrabResultFrameNotReady,
    GrabResultError
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
    GrabberBase(QObject * parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabWidgets);

public slots:
    virtual void init() = 0;
    virtual void startGrabbing() = 0;
    virtual void stopGrabbing() = 0;
    virtual bool isGrabbingStarted() const = 0;
    virtual void setGrabInterval(int msec) = 0;
    virtual void grab();

    /*!
      Updates monitor to grab picture from. We are grabbing picture from monitor first grabWidget belongs to.
     \param widget
    */
    virtual void updateGrabMonitor(QWidget * widget) = 0;

    /*!
      Grab implementation, called from @a GrabberBase#grab() slot, needs to be overriden
     \return GrabResult
    */
    virtual GrabResult _grab() = 0;

signals:
    void frameGrabAttempted(GrabResult grabResult);

    /*!
      Signals \a GrabManager that the grabber wants to be started or stopped
    */
    void grabberStateChangeRequested(bool isStartRequested);

protected:
    QList<QRgb> *m_grabResult; /*!< \code QList \endcode which stores grab result and could be accessed by \code GrabManager \endcode */
    QList<GrabWidget *> *m_grabWidgets;
    GrabResult m_lastGrabResult;

};
