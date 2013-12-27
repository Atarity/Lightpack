/*
 * QtGrabberEachWidget.cpp
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

#include "QtGrabberEachWidget.hpp"

#ifdef QT_GRAB_SUPPORT
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include "../src/debug.h"
#include "GrabberContext.hpp"

QtGrabberEachWidget::QtGrabberEachWidget(QObject *parent, GrabberContext *context)
    : TimeredGrabber(parent, context)
{
}

QtGrabberEachWidget::~QtGrabberEachWidget()
{
}

GrabResult QtGrabberEachWidget::_grab(QList<QRgb> &grabResult, const QList<GrabWidget*> &grabWidgets)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    grabResult.clear();
    foreach(GrabWidget * widget, grabWidgets)
	{
        grabResult.append( widget->isAreaEnabled() ? getColor(widget) : qRgb(0,0,0) );
    }
    return GrabResultOk;
}

QRgb QtGrabberEachWidget::getColor(const QWidget * w)
{
    QPixmap pix = QPixmap::grabWindow(QApplication::desktop()->winId(), w->x(), w->y(), w->width(), w->height());
    QPixmap scaledPix = pix.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage im = scaledPix.toImage();
    QRgb result = im.pixel(0,0);
    return result;
}

#endif // QT_GRAB_SUPPORT
