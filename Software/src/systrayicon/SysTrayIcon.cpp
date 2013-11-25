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

const QString SysTrayIcon::LightpackDownloadsPageUrl = "http://code.google.com/p/lightpack/downloads/list";

SysTrayIcon::SysTrayIcon(QObject *parent) :
    QObject(parent),
    d_ptr(new SysTrayIconPrivate(this))
{
}

SysTrayIcon::~SysTrayIcon()
{
    delete d_ptr;
}


bool SysTrayIcon::isVisible() const
{
    const Q_D(SysTrayIcon);
    return d->isVisible();
}

void SysTrayIcon::showMessage(const QString &title, const QString &text)
{
    Q_D(SysTrayIcon);
    d->showMessage(title, text);
}

void SysTrayIcon::showMessage(const Message msg)
{
    Q_D(SysTrayIcon);
    d->showMessage(msg);
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

void SysTrayIcon::retranslateUi()
{
    Q_D(SysTrayIcon);
    d->retranslateUi();
}

void SysTrayIcon::setStatus(const Status status, const QString *arg)
{
    Q_D(SysTrayIcon);
    d->setStatus(status, arg);
}

QString SysTrayIcon::toolTip() const
{
    const Q_D(SysTrayIcon);
    return d->toolTip();
}

void SysTrayIcon::updateProfiles()
{
    Q_D(SysTrayIcon);
    d->updateProfiles();
}
