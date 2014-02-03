/*
 * VirtualDesktop.hpp
 *
 *  Created on: 12/18/2013
 *     Project: Prismatik
 *
 *  Copyright (c) 2013 tim.helloworld
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

#ifndef VIRTUALDESKTOP_HPP
#define VIRTUALDESKTOP_HPP

#include <QObject>
#include <QList>

class QImage;
class GrabberBase;
class GrabWidget;

class VirtualDesktop : public QObject
{
    Q_OBJECT
public:
    explicit VirtualDesktop(QObject *parent = 0);
    virtual ~VirtualDesktop();

    const QList<QImage *> & constScreens() const { return _screensWithWidgets; }

signals:

public slots:
    void grab(GrabberBase * grabber, const QList<GrabWidget *> *widgets);

private:
    QList<int> getUsedScreens(const QList<GrabWidget *> *widgets);
private:
    QList<QImage *> _screensWithWidgets;

};

#endif // VIRTUALDESKTOP_HPP
