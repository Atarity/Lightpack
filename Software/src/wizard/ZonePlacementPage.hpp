/*
 * ZoneConfiguration.hpp
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

#ifndef ZONECONFIGURATION_HPP
#define ZONECONFIGURATION_HPP

#include <QWizardPage>
#include "SettingsAwareTrait.hpp"

namespace Ui {
class ZonePlacementPage;
}

class ZoneWidget;
class AbstractLedDevice;
class AreaDistributor;

class ZonePlacementPage : public QWizardPage, SettingsAwareTrait
{
    Q_OBJECT

public:
    explicit ZonePlacementPage(bool isInitFromSettings, TransientSettings *ts, QWidget *parent = 0);
    ~ZonePlacementPage();

protected:
    void initializePage();
    void cleanupPage();
    bool validatePage();
    size_t getLedCountOnTopEdge(size_t ledCount);

public slots:
    void turnLightOn(int id);
    void turnLightsOff();

private slots:
    void on_pbAndromeda_clicked();
    void on_pbCassiopeia_clicked();

    void on_numberOfLeds_valueChanged(int arg1);

    void on_pbPegasus_clicked();

private:
    void addGrabArea(int id, const QRect &rect);
    void removeLastGrabArea();
    void cleanupGrabAreas();
    void distributeAreas(AreaDistributor *);
    void resetNewAreaRect();
    AbstractLedDevice * device();

    Ui::ZonePlacementPage *_ui;
    QList<ZoneWidget*> _zoneWidgets;
    int _screenId;
    QList<QWidget*> _grabAreas;
    QRect _newAreaRect;
    int _x0;
    int _y0;

};

#endif // ZONECONFIGURATION_HPP
