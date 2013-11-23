/*
 * SysTrayIcon.hpp
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

#ifndef SYSTRAYICON_HPP
#define SYSTRAYICON_HPP

#include <QObject>
#ifndef UNITY_DESKTOP
#include <QSystemTrayIcon>
#endif

class QMenu;
class SysTrayIconPrivate;


class SysTrayIcon : public QObject
{
    Q_OBJECT
public:
    explicit SysTrayIcon(QObject *parent = 0);
    virtual ~SysTrayIcon();
    
    void setToolTip(QString &tip);
    bool isVisible();
    void setIcon(QString &filename);
    void setContextMenu(QMenu *menu);
    void showMessage(QString &title, QString &msg);
    void show();
    void hide();

signals:
#ifndef UNITY_DESKTOP
    void activated(QSystemTrayIcon::ActivationReason);
    void messageClicked();
#endif

public slots:

private:
    SysTrayIconPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(SysTrayIcon)
    Q_DISABLE_COPY(SysTrayIcon)

};

#endif // SYSTRAYICON_HPP
