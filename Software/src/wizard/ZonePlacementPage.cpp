/*
 * ZoneConfiguration.cpp
 *
 *  Created on: 10/25/2013
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

#include "ZonePlacementPage.hpp"
#include "ui_ZonePlacementPage.h"
#include "QDesktopWidget"
#include "AbstractLedDevice.hpp"
#include "Settings.hpp"
#include "AndromedaDistributor.hpp"
#include "CassiopeiaDistributor.hpp"
#include "PegasusDistributor.hpp"
#include "GrabAreaWidget.hpp"
#include "LedDeviceLightpack.hpp"


ZonePlacementPage::ZonePlacementPage(bool isInitFromSettings, TransientSettings *ts, QWidget *parent):
    QWizardPage(parent),
    SettingsAwareTrait(isInitFromSettings, ts),
    _ui(new Ui::ZonePlacementPage)
{
    _ui->setupUi(this);

    QRect screen = QApplication::desktop()->screenGeometry(_screenId);

    _x0 = screen.left() + 150;
    _y0 = screen.top() + 150;

    resetNewAreaRect();
}

ZonePlacementPage::~ZonePlacementPage()
{
    delete _ui;
}

AbstractLedDevice * ZonePlacementPage::device()
{
    return _transSettings->ledDevice.data();
}

void ZonePlacementPage::initializePage()
{
    using namespace SettingsScope;

    _screenId = field("screenId").toInt();
    registerField("numberOfLeds", _ui->sbNumberOfLeds);

    device()->setSmoothSlowdown(70);

    _ui->sbNumberOfLeds->setMaximum(device()->maxLedsCount());
    _ui->sbNumberOfLeds->setValue(device()->defaultLedsCount());

    _ui->sbNumberOfLeds->setValue(device()->maxLedsCount());
    if (_isInitFromSettings) {
        size_t ledCount = Settings::getNumberOfLeds(Settings::getConnectedDevice());

        for(size_t i = 0; i < ledCount; i++) {
            QPoint topLeft = Settings::getLedPosition(i);
            QSize size = Settings::getLedSize(i);
            QRect r(topLeft, size);
            addGrabArea(i, r);
        }
    } else {
        on_pbAndromeda_clicked();
    }
    turnLightsOff();
}

void ZonePlacementPage::cleanupPage()
{
    cleanupGrabAreas();
}

void ZonePlacementPage::cleanupGrabAreas()
{
    for(int i = 0; i < _grabAreas.size(); i++) {
        delete _grabAreas[i];
    }
    _grabAreas.clear();
}

bool ZonePlacementPage::validatePage()
{
    using namespace SettingsScope;
    QString deviceName = device()->name();
    SupportedDevices::DeviceType devType;
    if (deviceName.compare("lightpack", Qt::CaseInsensitive) == 0) {
        devType = SupportedDevices::DeviceTypeLightpack;

    } else if (deviceName.compare("adalight", Qt::CaseInsensitive) == 0) {
        devType = SupportedDevices::DeviceTypeAdalight;
        Settings::setAdalightSerialPortName(field("serialPort").toString());
        Settings::setAdalightSerialPortBaudRate(field("baudRate").toString());
        Settings::setColorSequence(devType, field("colorFormat").toString());

    } else if (deviceName.compare("ardulight", Qt::CaseInsensitive) == 0) {
        devType = SupportedDevices::DeviceTypeArdulight;
        Settings::setArdulightSerialPortName(field("serialPort").toString());
        Settings::setArdulightSerialPortBaudRate(field("baudRate").toString());
        Settings::setColorSequence(devType, field("colorFormat").toString());

    } else {
        devType = SupportedDevices::DeviceTypeVirtual;
    }
    Settings::setConnectedDevice(devType);
    Settings::setNumberOfLeds(devType, field("numberOfLeds").toInt());

    for(int i = 0; i < _grabAreas.size(); i++) {
        Settings::setLedPosition(i, _grabAreas[i]->geometry().topLeft());
        Settings::setLedSize(i, _grabAreas[i]->geometry().size());
    }

    cleanupGrabAreas();
    return true;
}

void ZonePlacementPage::resetNewAreaRect()
{
    _newAreaRect.setX(_x0);
    _newAreaRect.setY(_y0);
    _newAreaRect.setWidth(100);
    _newAreaRect.setHeight(100);
}

void ZonePlacementPage::turnLightOn(int id)
{
    QList<QRgb> lights;
    for(size_t i=0; i < device()->maxLedsCount(); i++)
    {
        if (i == static_cast<size_t>(id))
            lights.append(qRgb(255,255,255));
        else
            lights.append(0);
    }
    device()->setColors(lights);
}

void ZonePlacementPage::turnLightsOff()
{
    QList<QRgb> lights;
    for(size_t i=0; i < device()->maxLedsCount(); i++)
    {
        lights.append(0);
    }
    device()->setColors(lights);
}

void ZonePlacementPage::distributeAreas(AreaDistributor *distributor) {

    cleanupGrabAreas();
    for(size_t i = 0; i < distributor->areaCount(); i++) {
        ScreenArea *sf = distributor->next();
        qDebug() << sf->hScanStart() << sf->vScanStart();

        QRect s = QApplication::desktop()->screenGeometry(_screenId);
        QRect r(s.left() + sf->hScanStart() * (float)s.width(),
                s.top()  + sf->vScanStart() * (float)s.height(),
                (sf->hScanEnd() - sf->hScanStart()) * (float)s.width(),
                (sf->vScanEnd() - sf->vScanStart()) * (float)s.height());
        addGrabArea(i, r);

        delete sf;
    }
    resetNewAreaRect();
}

void ZonePlacementPage::addGrabArea(int id, const QRect &r)
{
    GrabAreaWidget *zone = new GrabAreaWidget(id);

    zone->move(r.topLeft());
    zone->resize(r.size());
    connect(zone, SIGNAL(resizeOrMoveStarted(int)), this, SLOT(turnLightOn(int)));
    connect(zone, SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(turnLightsOff()));
    zone->show();
    _grabAreas.append(zone);
}

void ZonePlacementPage::removeLastGrabArea()
{
    delete _grabAreas.last();
    _grabAreas.removeLast();
}

void ZonePlacementPage::on_pbAndromeda_clicked()
{
    AndromedaDistributor *andromeda = new AndromedaDistributor(_screenId, true, _ui->sbNumberOfLeds->value());

    distributeAreas(andromeda);

    delete andromeda;

}

void ZonePlacementPage::on_pbCassiopeia_clicked()
{
    CassiopeiaDistributor *cassiopeia = new CassiopeiaDistributor(_screenId, _ui->sbNumberOfLeds->value());

    distributeAreas(cassiopeia);

    delete cassiopeia;
}

void ZonePlacementPage::on_pbPegasus_clicked()
{
    PegasusDistributor *pegasus = new PegasusDistributor(_screenId, _ui->sbNumberOfLeds->value());

    distributeAreas(pegasus);

    delete pegasus;

}


void ZonePlacementPage::on_sbNumberOfLeds_valueChanged(int numOfLed)
{
    while (numOfLed < _grabAreas.size()) {
        removeLastGrabArea();
    }
    QRect screen = QApplication::desktop()->screenGeometry(_screenId);

    const int dx = 10;
    const int dy = 10;

    while (numOfLed > _grabAreas.size()) {
        addGrabArea(_grabAreas.size(), _newAreaRect);
        if (_newAreaRect.right() + dx < screen.right()) {
            _newAreaRect.moveTo(_newAreaRect.x() + dx, _newAreaRect.y());
        } else if (_newAreaRect.bottom() + dy < screen.bottom()) {
            _newAreaRect.moveTo(_x0, _newAreaRect.y() + dy);
        } else {
            _newAreaRect.moveTo(_x0,_y0);
        }
    }
}

