/*
 * desktop.cpp
 *
 *  Created on: 30.01.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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

#include <QtGui>
#include <QtDebug>
#include "desktop.h"

int Desktop::Width = 0;
int Desktop::Height = 0;

// Using this for scale LED widgets
int Desktop::WidthSaved = 0;
int Desktop::HeightSaved = 0;

Desktop *Desktop::self = NULL; /* private */


void Desktop::Initialize(QObject *parent)
{
    qDebug() << "Desktop::Initialize()";
    Desktop::self = new Desktop(parent);
}

Desktop::Desktop(QObject *parent) : QObject(parent)
{
    Desktop::Width = QApplication::desktop()->screenGeometry().width();
    Desktop::Height = QApplication::desktop()->screenGeometry().height();

    Desktop::WidthSaved = Desktop::Width;
    Desktop::HeightSaved = Desktop::Height;

    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(sizeChanged(int)));
}

void Desktop::sizeChanged(int /*screen*/)
{    
    Desktop::WidthSaved = Desktop::Width;
    Desktop::HeightSaved = Desktop::Height;

    Desktop::Width = QApplication::desktop()->screenGeometry().width();
    Desktop::Height = QApplication::desktop()->screenGeometry().height();
    qDebug() << "Desktop::sizeChanged():" << Width << "x" << Height;
}
