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
#include "MonitorsConfigurationPage.hpp"
#include "ui_MonitorsConfigurationPage.h"
#include "MonitorIdForm.hpp"

MonitorsConfigurationPage::MonitorsConfigurationPage(SettingsScope::Settings *settings, QWidget *parent) :
    QWizardPage(parent),
    SettingsAwareTrait(settings),
    _ui(new Ui::MonitorsConfigurationPage)
{
    _ui->setupUi(this);
}

MonitorsConfigurationPage::~MonitorsConfigurationPage()
{
    delete _ui;
}

void MonitorsConfigurationPage::initializePage()
{
    size_t screenCount = QApplication::desktop()->screenCount();
    if(screenCount == 1) {
        wizard()->next();
        return;
    }
    for(int i = 0; i < screenCount; i++) {
        QRect geom = QApplication::desktop()->screenGeometry(i);
        MonitorIdForm *monitorIdForm = new MonitorIdForm();

        monitorIdForm->setWindowFlags(Qt::FramelessWindowHint);

        monitorIdForm->move(geom.topLeft());
        monitorIdForm->resize(geom.width(), geom.height());

        monitorIdForm->show();

        _monitorForms.append(monitorIdForm);
    }

}

bool MonitorsConfigurationPage::validatePage()
{
    foreach ( MonitorIdForm *monitorIdForm, _monitorForms ) {
        delete monitorIdForm;
    }
    return true;
}
