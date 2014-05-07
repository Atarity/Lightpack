/*
 * SelectDevicePage.cpp
 *
 *  Created on: 11/1/2013
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

#include "SelectDevicePage.hpp"
#include "ui_SelectDevicePage.h"
#include "Wizard.hpp"
#include "LedDeviceVirtual.hpp"
#include "QDesktopWidget"

SelectDevicePage::SelectDevicePage(bool isInitFromSettings, TransientSettings *ts, QWidget *parent):
    QWizardPage(parent),
    SettingsAwareTrait(isInitFromSettings, ts),
    ui(new Ui::SelectDevicePage)
{
    ui->setupUi(this);
}

SelectDevicePage::~SelectDevicePage()
{
    delete ui;
}

void SelectDevicePage::initializePage()
{
    registerField("isAdalight", ui->rbAdalight);
    registerField("isArdulight", ui->rbArdulight);
    registerField("isVirtual", ui->rbVirtual);

}

void SelectDevicePage::cleanupPage()
{
    setField("isAdalight", false);
    setField("isArdulight", false);
    setField("isVirtual", false);
}

bool SelectDevicePage::validatePage()
{
    if (field("isVirtual").toBool()) {
        _transSettings->ledDevice.reset(new LedDeviceVirtual());
    }
    return true;
}

int SelectDevicePage::nextId() const
{
    if (ui->rbVirtual->isChecked())
        if (QApplication::desktop()->screenCount() == 1) {
            return reinterpret_cast<Wizard *>(wizard())->skipMonitorConfigurationPage();
        } else {
            return Page_MonitorConfiguration;
        }

    else
        return Page_ConfigureDevice;
}
