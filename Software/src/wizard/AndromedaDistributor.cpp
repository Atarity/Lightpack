/*
 * AndromedaZoneDistributor.cpp
 *
 *  Created on: 10/28/2013
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

#include "AndromedaDistributor.hpp"
#include "PrismatikMath.hpp"

#define STAND_WIDTH 0.3333 //33%

AndromedaDistributor::~AndromedaDistributor() {
    if (_currentArea)
        cleanCurrentArea();
}

ScreenArea * AndromedaDistributor::next() {
    const double thikness = 0.15;
    double x, y;

    if (_dx == 0 && _dy == 0) {
        _dx = 1;
        _width = 1.0 / areaCountOnTopEdge();
        _height = thikness;
        x = 1.0 - areaCountOnBottomEdge() * _width / 2;
        y = 1.0 - _height;
    } else if (_dx > 0 && cmp(_currentArea->hScanEnd(), 1.0, 0.01) >= 0 ) {
        cleanCurrentArea();
        _dx = 0;
        _dy = -1;
        _width = thikness;
        _height = 1.0f / areaCountOnSideEdge();
        x = 1.0f - _width;
        y = 1.0f - _height;
    } else if (_dy < 0 && cmp(_currentArea->vScanStart(), 0.0, .01) <= 0) {
        cleanCurrentArea();
        _dx = -1;
        _dy = 0;
        _width = 1.0 / areaCountOnTopEdge();
        _height = thikness;
        x = 1.0 - _width;
        y = 0.0;
    } else if (_dx < 0 && cmp(_currentArea->hScanStart(), 0.0, .01) <= 0) {
        cleanCurrentArea();
        _dx = 0;
        _dy = 1;
        _width = thikness;
        _height = 1.0 / areaCountOnSideEdge();
        x = 0.0;
        y = 0.0;
    } else if (_dy > 0 && cmp(_currentArea->vScanEnd(), 1.0, .01) >= 0) {
        cleanCurrentArea();
        _dx = 1;
        _dy = 0;
        _width = 1.0 / areaCountOnTopEdge();
        _height = thikness;
        x = 0.0;
        y = 1.0 - _height;
    }

    ScreenArea *result = NULL;
    if (!_currentArea) {
        result = new ScreenArea( x, x + _width, y, y + _height);
    } else {
        ScreenArea *cf = _currentArea;
        double dx = _width * (double)_dx;
        double dy = _height * (double)_dy;

        result = new ScreenArea(within1(cf->hScanStart() + dx), within1(cf->hScanEnd() + dx),
                                    within1(cf->vScanStart() + dy), within1(cf->vScanEnd() + dy));
        cleanCurrentArea();
    }

    _currentArea = result;
    return new ScreenArea(*_currentArea);

}

size_t AndromedaDistributor::areaCountOnTopEdge() const
{
    return _areaCount - 2 * areaCountOnSideEdge() - areaCountOnBottomEdge();
}

size_t AndromedaDistributor::areaCountOnBottomEdge() const
{
    size_t baseCount = (_areaCount - 2 * areaCountOnSideEdge()) / 2;
    size_t rawCount = _isStandPresent ? PrismatikMath::round(baseCount * (1 - STAND_WIDTH )) : baseCount;
    // we need symmetric bottom
    return rawCount + rawCount % 2;
}

size_t AndromedaDistributor::areaCountOnSideEdge() const
{
    double a = aspect();
    size_t roundDistrib = PrismatikMath::round(_areaCount /(2 + 2*a));
    return roundDistrib;
}
