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

#include <QtDebug>
#include "debug.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    ui->setupUi(this);

    this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint );

    versionsUpdate();
}

AboutDialog::~AboutDialog()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    delete ui;
}

void AboutDialog::changeEvent(QEvent *e)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << e->type();

    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        // Retranslate UI and update versions string
        versionsUpdate();
        break;
    default:
        break;
    }
}

void AboutDialog::closeEvent(QCloseEvent *event)
{    
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    this->hide();
    event->ignore();
}

void AboutDialog::setFirmwareVersion(const QString &firmwareVersion)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    this->fimwareVersion = firmwareVersion;
    versionsUpdate();
}


void AboutDialog::versionsUpdate()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    ui->retranslateUi(this);

    // Save templete for construct version string
    QString versionsTemplate = ui->labelVersions->text();

#ifdef HG_REVISION
    versionsTemplate = versionsTemplate.arg(
            QApplication::applicationVersion(),
            HG_REVISION,
            fimwareVersion );
#else
    versionsTemplate = versionsTemplate.arg(
            QApplication::applicationVersion(),
            "unknown",
            fimwareVersion );
    versionsTemplate.remove(QRegExp(" [(].+[)]"));
#endif

    ui->labelVersions->setText( versionsTemplate );

    adjustSize();

    setFixedSize( sizeHint() );
}




