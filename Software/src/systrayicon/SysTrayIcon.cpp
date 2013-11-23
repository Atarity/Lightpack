/*
 * SysTrayIcon.cpp
 *
 *  Created on: 11/23/2013
 *     Project: Prismatik 
 *
 *  Copyright (c) 2013 tim
 *
 *  Lightpack is an open-source, USB content-driving ambient lighting
 *  hardware.
 *
 *  Prismatik is a free, open-source software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as published 
 *  by the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Prismatik and Lightpack files is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "SysTrayIcon.hpp"

#ifdef UNITY_DESKTOP
#include "SysTrayIcon_unity_p.hpp"
#else
#include "SysTrayIcon_qt_p.hpp"
#endif

SysTrayIcon::SysTrayIcon(QObject *parent) :
    QObject(parent),
    d_ptr(new SysTrayIconPrivate(this))
{
}

SysTrayIcon::~SysTrayIcon()
{
    delete d_ptr;
}


void SysTrayIcon::setToolTip(QString &tip)
{
    Q_D(SysTrayIcon);
    d->setToolTip(tip);
}

bool SysTrayIcon::isVisible()
{
    Q_D(SysTrayIcon);
    return d->isVisible();
}

void SysTrayIcon::setIcon(QString &filename)
{
    Q_D(SysTrayIcon);
    d->setIcon(filename);
}

void SysTrayIcon::setContextMenu(QMenu *menu)
{
    Q_D(SysTrayIcon);
    d->setContextMenu(menu);
}

void SysTrayIcon::showMessage(QString &title, QString &msg)
{
    Q_D(SysTrayIcon);
    d->showMessage(title, msg);
}

void SysTrayIcon::show()
{
    Q_D(SysTrayIcon);
    d->show();
}

void SysTrayIcon::hide()
{
    Q_D(SysTrayIcon);
    d->hide();
}
