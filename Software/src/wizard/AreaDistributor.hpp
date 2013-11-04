/*
 * ZoneDistributor.hpp
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

#ifndef AREADISTRIBUTOR_HPP
#define AREADISTRIBUTOR_HPP
#include <QList>
#include <QApplication>
#include <QDesktopWidget>
#include <math.h>

class ScreenArea {
public:
    ScreenArea(double hScanStart, double hScanEnd,
                   double vScanStart, double vScanEnd):
        _hScanStart(hScanStart),
        _hScanEnd(hScanEnd),
        _vScanStart(vScanStart),
        _vScanEnd(vScanEnd){}

    double hScanStart() { return _hScanStart; }
    double hScanEnd() { return _hScanEnd; }
    double vScanStart() { return _vScanStart; }
    double vScanEnd() { return _vScanEnd; }
protected:
    double _hScanStart;
    double _hScanEnd;
    double _vScanStart;
    double _vScanEnd;
};

template <class T>
inline T within1(T x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

template <class T>
inline int cmp(T x, T y, T e) {
    T d = x - y;
    if (fabs(d) < e) return 0;
    if (d < 0) return -1;
    return 1;
}

class AreaDistributor {
public:
    AreaDistributor(int screenId, size_t areaCount):
        _screenId(screenId),
        _areaCount(areaCount),
        _currentArea(NULL)
    {}
    virtual ~AreaDistributor(){}

    virtual ScreenArea * next() = 0;

    size_t areaCount() const { return _areaCount; }

protected:
    int _screenId;
    size_t _areaCount;
    ScreenArea * _currentArea;

    double aspect() const {
       QRect screenRect = QApplication::desktop()->screenGeometry(_screenId);
       return (double)screenRect.width() / screenRect.height();
    }

    virtual size_t areaCountOnSideEdge() const = 0;
    virtual size_t areaCountOnTopEdge() const = 0;
    virtual size_t areaCountOnBottomEdge() const = 0;
};

#endif // AREADISTRIBUTOR_HPP
