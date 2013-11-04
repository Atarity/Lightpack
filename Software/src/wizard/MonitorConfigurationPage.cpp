/*
 * MonitorsConfigurationPage.cpp
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

#include <QDesktopWidget>
#include <QRadioButton>
#include "MonitorConfigurationPage.hpp"
#include "ui_MonitorConfigurationPage.h"
#include "MonitorIdForm.hpp"
#include "debug.h"

MonitorConfigurationPage::MonitorConfigurationPage(bool isInitFromSettings, TransientSettings *ts, QWidget *parent) :
    QWizardPage(parent),
    SettingsAwareTrait(isInitFromSettings, ts),
    _ui(new Ui::MonitorsConfigurationPage)
{
    _ui->setupUi(this);
    registerField("screenId", _ui->cbMonitor);
}

MonitorConfigurationPage::~MonitorConfigurationPage()
{
    delete _ui;
}

void MonitorConfigurationPage::initializePage()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    size_t screenCount = QApplication::desktop()->screenCount();
//    if (!_isInitFromSettings && screenCount > 1) {
        for(size_t i = 0; i < screenCount; i++) {
            QRect geom = QApplication::desktop()->screenGeometry(i);
            MonitorIdForm *monitorIdForm = new MonitorIdForm();

            monitorIdForm->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

            monitorIdForm->move(geom.topLeft());
            monitorIdForm->resize(geom.width(), geom.height());

            monitorIdForm->setId(i+1);

            QString text = QString("Monitor %0, %1x%2").arg(QString::number(i+1), QString::number(geom.width()), QString::number(geom.height()));
            _ui->cbMonitor->addItem(text, (int)i);

            monitorIdForm->show();

            _monitorForms.append(monitorIdForm);
        }
//    } else {
//        QMetaObject::invokeMethod(wizard(), "next", Qt::QueuedConnection);
//    }
}

bool MonitorConfigurationPage::validatePage()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    foreach ( MonitorIdForm *monitorIdForm, _monitorForms ) {
        delete monitorIdForm;
    }
    return true;
}

void MonitorConfigurationPage::addMonitor(int id)\
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    QRadioButton *rbMon = new QRadioButton(this);

    char buf[10];
    QString name = QString("rb%0").arg(QString::number(id));

    rbMon->setObjectName(name);
    rbMon->setText(buf);

    _ui->gridLayout->addWidget(rbMon, id + 1, 1, 1, 1);

}
