/*
 * AndromedaZoneDistributor.hpp
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

#ifndef ANDROMEDAZONEDISTRIBUTOR_HPP
#define ANDROMEDAZONEDISTRIBUTOR_HPP
#include "ZoneDistributor.hpp"

class AndromedaDistributor : public AreaDistributor
{
public:
    AndromedaDistributor(int screenId, bool isStandPresent, size_t fragmentCount):
        AreaDistributor(screenId, fragmentCount),
        _isStandPresent(isStandPresent),
        _dx(0), _dy(0)
    {}

    virtual ScreenArea * next();

protected:
    bool _isStandPresent;
    char _dx, _dy;
    float _width, _height;

    virtual size_t areaCountOnLeftEdge() const;
    virtual size_t areaCountOnTopEdge() const;
    virtual size_t areaCountOnBottomEdge() const;
    virtual size_t areaCountOnRightEdge() const;

private:
    void cleanCurrentFragment() {
        delete _currentArea;
        _currentArea = NULL;
    }
};

#endif // ANDROMEDAZONEDISTRIBUTOR_HPP
