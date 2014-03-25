/*
 * EndSessionDetector.hpp
 *
 *  Created on: 01.03.2014
 *      Author: EternalWind
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  Lightpack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Lightpack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef END_SESSION_DETECTOR
#define END_SESSION_DETECTOR
#include "QAbstractNativeEventFilter"

class EndSessionDetector :
    public QAbstractNativeEventFilter
{
public:
    EndSessionDetector();

    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) Q_DECL_OVERRIDE;

    ~EndSessionDetector();

private:
	void Destroy();

	bool m_isDestroyed;
};
#endif