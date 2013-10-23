/*
 * LightpacksDiscoveryPage.cpp
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

#include "LightpacksDiscoveryPage.hpp"
#include "ui_LightpacksDiscoveryPage.h"
#include "LedDeviceLightpack.hpp"
#include "Settings.hpp"
#include "Wizard.hpp"

LightpacksDiscoveryPage::LightpacksDiscoveryPage(SettingsScope::Settings *settings, QWidget *parent) :
    QWizardPage(parent),
    SettingsAwareTrait(settings),
    _ui(new Ui::LightpacksDiscoveryPage)
{
    _ui->setupUi(this);
}

LightpacksDiscoveryPage::~LightpacksDiscoveryPage()
{
    delete _ui;
}

void LightpacksDiscoveryPage::initializePage() {
    LedDeviceLightpack lpack;

    lpack.open();

    if (lpack.lightpacksFound() > 0) {
        char buf[10];
        _ui->rbChooseAnotherDevice->setEnabled(true);
        _ui->rbLightpackSelected->setEnabled(true);

        QString caption;
        if (lpack.lightpacksFound() == 1)
            caption = tr("1 Lightpack found");
        else
            caption = tr("%0 Lightpacks found").arg(itoa(lpack.lightpacksFound(), buf, 10));

        QString caption2 = tr("%0 zones are available").arg(itoa(lpack.maxLedsCount(), buf, 10));
        _ui->labelLightpacksCount->setText(caption);
        _ui->labelZonesAvailable->setText(caption2);
    }
}

bool LightpacksDiscoveryPage::validatePage() {
    if (_ui->rbLightpackSelected->isChecked())
        _settings->setConnectedDevice(SupportedDevices::DeviceTypeLightpack);
    return true;
}

int LightpacksDiscoveryPage::nextId() const {
    if (_ui->rbLightpackSelected->isChecked())
        return Page_MonitorsConfiguration;
    else
        return Page_ChooseDevice;
}

