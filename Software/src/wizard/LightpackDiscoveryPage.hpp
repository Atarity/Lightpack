/*
 * LightpacksDiscoveryPage.hpp
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

#ifndef LIGHTPACKSDISCOVERYPAGE_HPP
#define LIGHTPACKSDISCOVERYPAGE_HPP

#include <QWizardPage>
#include "SettingsAwareTrait.hpp"

namespace Ui {
class LightpacksDiscoveryPage;
}

class LightpackDiscoveryPage : public QWizardPage, SettingsAwareTrait
{
    Q_OBJECT

public:
    explicit LightpackDiscoveryPage(bool isInitFromSettings, TransientSettings *ts, QWidget *parent = 0);
    ~LightpackDiscoveryPage();

protected:
    virtual void initializePage();
    virtual bool validatePage();
    virtual int nextId() const;

private:

    Ui::LightpacksDiscoveryPage *_ui;
};

#endif // LIGHTPACKSDISCOVERYPAGE_HPP
