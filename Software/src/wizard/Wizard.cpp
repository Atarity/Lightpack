/*
 * Wizard.cpp
 *
 *  Created on: 10/22/2013
 *     Project: %PROJECT% (Use "Lightpack" for hardware/firmware, or "Prismatik" for software)
 *
 *  Copyright (c) 2013 %NICKNAME%
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

#include "Wizard.hpp"
#include "ui_Wizard.h"
#include "LightpackDiscoveryPage.hpp"
#include "MonitorConfigurationPage.hpp"
#include "ZonePlacementPage.hpp"
#include "SelectProfilePage.hpp"
#include "SelectDevicePage.hpp"
#include "ConfigureDevicePage.hpp"
#include "AbstractLedDevice.hpp"

Wizard::Wizard(bool isInitFromSettings, QWidget *parent) :
    QWizard(parent),
    SettingsAwareTrait(isInitFromSettings, NULL),
    _ui(new Ui::Wizard)
{
    _transSettings = new TransientSettings;
    memset(_transSettings, 0, sizeof(TransientSettings));

    _ui->setupUi(this);
    this->setPage(Page_LightpackDiscovery, new LightpackDiscoveryPage(_isInitFromSettings, _transSettings) );
    this->setPage(Page_ChooseDevice, new SelectDevicePage(_isInitFromSettings, _transSettings) );
    this->setPage(Page_ConfigureDevice, new ConfigureDevicePage(_isInitFromSettings, _transSettings) );
    this->setPage(Page_MonitorConfiguration, new MonitorConfigurationPage(_isInitFromSettings, _transSettings) );
    this->setPage(Page_ChooseProfile, new SelectProfilePage(_isInitFromSettings, _transSettings) );
    this->setPage(Page_ZonePlacement, new ZonePlacementPage(_isInitFromSettings, _transSettings) );
}

Wizard::~Wizard()
{
//    if (_transSettings->ledDevice != NULL)
//        delete _transSettings->ledDevice;
    delete _transSettings;
    delete _ui;
}
