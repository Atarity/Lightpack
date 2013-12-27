/*
 * ColorProvider.hpp
 *
 *  Created on: 12/23/2013
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

#ifndef COLORPROVIDER_HPP
#define COLORPROVIDER_HPP

#include <QObject>
#include "GrabberBase.hpp"

struct Screen;

class ColorProvider : public QObject
{
    Q_OBJECT
public:
    explicit ColorProvider(QObject *parent = 0);

signals:

public slots:

private:
    GrabberBase * _grabber;


};

#endif // COLORPROVIDER_HPP
