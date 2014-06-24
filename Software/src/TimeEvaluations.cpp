/*
 * TimeEvaluations.cpp
 *
 *  Created on: 4.08.2010
 *      Author: Andrey Kraynov
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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

#include "TimeEvaluations.hpp"
#include <QtDebug>
#include <QtGlobal>

#if defined _MSC_VER
#include <winsock.h>    // timeval is defined in here
#include <sys/timeb.h>

static int gettimeofday(struct timeval* tv, struct timezone * /* tzp */)
{
    struct _timeb curSysTime;

    _ftime(&curSysTime);
    tv->tv_sec = (long)curSysTime.time;
    tv->tv_usec = curSysTime.millitm * 1000;
    return 0;
}

#else
#include <sys/time.h> // for timeval and gettimeofday()
#endif

void TimeEvaluations::howLongItStart()
{
    struct timeval tv;
    if(gettimeofday(&tv, 0) < 0){
        qWarning() << "howLongItStart(): Error gettimeofday()";
    }else{
        this->timevalue_ms = (tv.tv_sec % 1000) * 1000.0 + tv.tv_usec / 1000.0;
    }
}

double TimeEvaluations::howLongItEnd()
{
    struct timeval tv;
    if(gettimeofday(&tv, 0) < 0){
        qWarning() << "howLongItEnd(): Error gettimeofday()";
        return -1;
    }else{
        double now = ((tv.tv_sec % 1000) * 1000.0 + tv.tv_usec / 1000.0);
        double dt_ms = now - this->timevalue_ms;
        this->timevalue_ms = -1;
        return dt_ms;
    }
}
