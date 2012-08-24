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
#include <QtGui>
#include "debug.h"

QtGrabberEachWidget::QtGrabberEachWidget()
{
}

QtGrabberEachWidget::~QtGrabberEachWidget()
{
}

const char * QtGrabberEachWidget::getName()
{
    return "QtGrabberEachWidget";
}

void QtGrabberEachWidget::updateGrabScreenFromWidget(QWidget * /*widget*/)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;
}

GrabResult QtGrabberEachWidget::grabWidgetsColors(QList<GrabWidget *> &widgets, QList<QRgb> * widgetsColors)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    for (int i = 0; i < widgets.size(); i++)
	{
        widgetsColors->append(getColor(widgets[i]));
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
