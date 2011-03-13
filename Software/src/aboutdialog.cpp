/* 
 * aboutdialog.cpp
 *
 *  Created on: 26.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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
 
#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "version.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint );

    QString softwareVersion = tr("Software version ") + QApplication::applicationVersion();
#ifdef HG_REVISION
    softwareVersion += QString(" <a href=\"http://code.google.com/p/lightpack/source/detail?r=%1\">%1</a>").arg( HG_REVISION );
#endif

    ui->labelVersionSoftware->setText( softwareVersion );
    this->adjustSize();

    setFixedSize(width(), height());
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::changeEvent(QEvent *e)
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

void AboutDialog::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore();
}

void AboutDialog::setFirmwareVersion(const QString &firmwareVersion)
{
    ui->labelVersionFirmware->setText(tr("Firmware version ") + firmwareVersion);
}

