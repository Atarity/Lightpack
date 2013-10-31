/*
 * ZoneWidget.hpp
 *
 *  Created on: 10/26/2013
 *     Project: Prismatik
 *
 *  Copyright (c) 2013 Tim
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

#ifndef ZONEWIDGET_HPP
#define ZONEWIDGET_HPP

#include <QWidget>

namespace Ui {
class ZoneWidget;
}

class ZoneWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ZoneWidget(QWidget *parent = 0);
    ~ZoneWidget();

private:
    Ui::ZoneWidget *_ui;
};

#endif // ZONEWIDGET_HPP
