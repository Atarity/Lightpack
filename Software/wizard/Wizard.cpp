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

Wizard::Wizard(SettingsScope::Settings *settings, bool isInitFromSettings, TransientSettings *transSettings, QWidget *parent) :
    QWizard(parent),
    SettingsAwareTrait(settings, isInitFromSettings, transSettings),
    _ui(new Ui::Wizard)
{
    _ui->setupUi(this);
    this->setPage(Page_LightpackDiscovery, new LightpackDiscoveryPage(_settings, _isInitFromSettings, _transSettings) );
    this->setPage(Page_MonitorConfiguration, new MonitorConfigurationPage(_settings, _isInitFromSettings, _transSettings) );
    this->setPage(Page_ZonePlacement, new ZonePlacementPage(_settings, _isInitFromSettings, _transSettings) );
}

Wizard::~Wizard()
{
    delete _ui;
}
