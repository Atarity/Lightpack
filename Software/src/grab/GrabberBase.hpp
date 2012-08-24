/*
 * GrabberBase.hpp
 *
 *  Created on: 25.07.11
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

#include <QColor>
#include <QTimer>
#include "defs.h"
#include "GrabWidget.hpp"

enum GrabResult {
    GrabResultOk,
    GrabResultFrameNotReady
};

/*!
  Interface which represents each particular grabber. If you want to add a new grabber just add implementation of @code GrabberBase
  and modify \a GrabManager
*/
class GrabberBase : public QObject
{
    Q_OBJECT
public:
/*!
 \param parent standart Qt-specific owner
 \param grabResult \code QList to write results of grabbing to
 \param grabWidgets List of GrabWidgets
*/
    GrabberBase(QObject * parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabWidgets);
    virtual void startGrabbing() = 0;
    virtual void stopGrabbing() = 0;
    virtual void isGrabbingStarted() = 0;
    virtual void setGrabInterval(int msec) = 0;

    /*!
      Grab implementation, called from @a GrabberBase#grab() slot, needs to be overriden
     \return GrabResult
    */
    virtual GrabResult _grab() = 0;

protected:
    QList<QRgb> *m_grabResult; /*!< @code QList which stores grab result and could be accessed by @code GrabManager */
    QList<GrabWidget *> *m_grabWidgets;



public slots:

    virtual void grab();

    virtual void firstWidgetPositionChanged(QWidget * widget) = 0;

signals:
    void frameGrabAttempted(GrabResult grabResult);
};
