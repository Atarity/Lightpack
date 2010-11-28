/* 
 * aboutdialog.cpp
 *
 *  Created on: 26.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: AmbilightUSB
 *
 *  AmbilightUSB is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  AmbilightUSB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  AmbilightUSB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "version.h"

aboutDialog::aboutDialog(QString hardwareVersion, QString firmwareVersion, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
    ui->labelVersionSoftware->setText(tr("<b>Software version:</b> ") + QApplication::applicationVersion());
    ui->labelVersionHardware->setText(tr("<b>Hardware version:</b> ") + hardwareVersion);
    ui->labelVersionFirmware->setText(tr("<b>Firmware version:</b> ") + firmwareVersion);
}

aboutDialog::~aboutDialog()
{
    delete ui;
}

void aboutDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
