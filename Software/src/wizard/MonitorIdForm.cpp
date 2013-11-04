/*
 * MonitorIdForm.cpp
 *
 *  Created on: 10/23/2013
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

#include "MonitorIdForm.hpp"
#include "ui_MonitorIdForm.h"

MonitorIdForm::MonitorIdForm(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::MonitorIdForm)
{
    _ui->setupUi(this);
}

MonitorIdForm::~MonitorIdForm()
{
    delete _ui;
}

void MonitorIdForm::setId(int id)
{
    _ui->label->setText(QString::number(id));
}
