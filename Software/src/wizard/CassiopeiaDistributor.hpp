/*
 * CassiopeiaDistributor.hpp
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

#ifndef CASSIOPEIADISTRIBUTOR_HPP
#define CASSIOPEIADISTRIBUTOR_HPP
#include "AreaDistributor.hpp"

class CassiopeiaDistributor : public AreaDistributor
{
public:
    CassiopeiaDistributor(int screenId, size_t areaCount):
        AreaDistributor(screenId, areaCount),
        _dx(0), _dy(0)
    {}

    virtual ~CassiopeiaDistributor();

    ScreenArea * next();

protected:
    char _dx, _dy;
    double _width, _height;

    virtual size_t areaCountOnSideEdge() const;
    virtual size_t areaCountOnTopEdge() const;
    virtual size_t areaCountOnBottomEdge() const;

private:
    void cleanCurrentArea() {
        delete _currentArea;
        _currentArea = NULL;
    }
};

#endif // CASSIOPEIADISTRIBUTOR_HPP
