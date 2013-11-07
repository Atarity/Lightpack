/*
 * PegasusDistributor.cpp
 *
 *  Created on: 11/5/2013
 *     Project: Prismatik 
 *
 *  Copyright (c) 2013 tim
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

#include "PegasusDistributor.hpp"

PegasusDistributor::~PegasusDistributor() {
    if (_currentArea)
        cleanCurrentArea();
}

ScreenArea * PegasusDistributor::next() {
    const double thikness = 0.15;
    double x, y;

    if (_dx == 0 && _dy == 0) {
        _dx = 0;
        _dy = -1;
        _width = thikness;
        _height = 1.0f / areaCountOnSideEdge();
        x = 1.0f - _width;
        y = 1.0f - _height;
    } else if (_dy < 0 && cmp(_currentArea->vScanStart(), 0.0, .01) <= 0) {
        cleanCurrentArea();
        _dx = 0;
        _dy = 1;
        _width = thikness;
        _height = 1.0 / areaCountOnSideEdge();
        x = 0.0;
        y = 0.0;
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

size_t PegasusDistributor::areaCountOnTopEdge() const
{
    return 0;
}

size_t PegasusDistributor::areaCountOnBottomEdge() const
{
    return 0;
}

size_t PegasusDistributor::areaCountOnSideEdge() const
{
    return _areaCount / 2;
}
