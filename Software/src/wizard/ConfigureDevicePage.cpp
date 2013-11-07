/*
 * ConfigureDevicePage.cpp
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

#include "ConfigureDevicePage.hpp"
#include "ui_ConfigureDevicePage.h"
#include "Settings.hpp"
#include "LedDeviceArdulight.hpp"
#include "LedDeviceAdalight.hpp"
#include "LedDeviceVirtual.hpp"
#include "QMessageBox"

using namespace SettingsScope;

ConfigureDevicePage::ConfigureDevicePage(bool isInitFromSettings, TransientSettings *ts, QWidget *parent):
    QWizardPage(parent),
    SettingsAwareTrait(isInitFromSettings, ts),
    ui(new Ui::ConfigureDevicePage)
{
    ui->setupUi(this);
}

void ConfigureDevicePage::initializePage()
{
    ui->leSerialPortName->setText(SERIAL_PORT_DEFAULT);
    ui->cbBaudRate->clear();
    // NOTE: This line emit's signal currentIndex_Changed()
    ui->cbBaudRate->addItems(Settings::getSupportedSerialPortBaudRates());

    registerField("serialPort", ui->leSerialPortName);
    registerField("baudRate", ui->cbBaudRate);

}

void ConfigureDevicePage::cleanupPage()
{
    setField("serialPort", "");
    setField("baudRate", 0);
}

bool ConfigureDevicePage::validatePage()
{
    QString portName = field("serialPort").toString();
    int baudRate = field("baudRate").toInt();
    if (field("isAdalight").toBool()) {
        _transSettings->ledDevice.reset(new LedDeviceAdalight(portName, baudRate));
    } else if (field("isArdulight").toBool()){
        _transSettings->ledDevice.reset(new LedDeviceArdulight(portName, baudRate));
    } else {
        QMessageBox::information(NULL, "Wrong device", "Try to restart the wizard");
        qCritical() << "couldn't create LedDevice, unexpected state, device is not selected or device is not configurable";
        return false;
    }

    return true;
}


ConfigureDevicePage::~ConfigureDevicePage()
{
    delete ui;
}
