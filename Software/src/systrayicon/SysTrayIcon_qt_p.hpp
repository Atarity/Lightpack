/*
 * SysTrayIcon_qt_p.hpp
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

#ifndef SYSTRAYICON_QT_P_HPP
#define SYSTRAYICON_QT_P_HPP
#include "SysTrayIcon.hpp"
#include "QSystemTrayIcon"


class SysTrayIconPrivate
{
    Q_DECLARE_PUBLIC(SysTrayIcon)

public:
    SysTrayIconPrivate(SysTrayIcon * q)
        : _qsystray(new QSystemTrayIcon())
        , q_ptr(q)
    {
        QObject::connect(_qsystray, SIGNAL(activated(QSystemTrayIcon::ActivationReason))
                         , q_ptr, SIGNAL(activated(QSystemTrayIcon::ActivationReason)));
        QObject::connect(_qsystray, SIGNAL(messageClicked())
                         , q_ptr, SIGNAL(messageClicked()));
    }
    virtual ~SysTrayIconPrivate()
    {
        delete _qsystray;
    }

    void setToolTip(QString &tip)
    {
        _qsystray->setToolTip(tip);
    }

    bool isVisible()
    {
        return _qsystray->isVisible();
    }

    void setIcon(QString &filename)
    {
        _qsystray->setIcon(QIcon(filename));
    }

    void setContextMenu(QMenu *menu)
    {
        _qsystray->setContextMenu(menu);
    }

    void showMessage(QString &title, QString &msg)
    {
        _qsystray->showMessage(title, msg);
    }

    void show()
    {
        _qsystray->show();
    }

    void hide()
    {
        _qsystray->hide();
    }

public:
    QSystemTrayIcon * _qsystray;

    SysTrayIcon * const q_ptr;
};

#endif // SYSTRAYICON_QT_P_HPP
