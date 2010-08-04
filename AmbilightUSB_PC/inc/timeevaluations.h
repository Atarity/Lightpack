/*
 * timeevaluations.h
 *
 *  Created on: 4.08.2010
 *      Author: brunql
 */

#ifndef TIMEEVALUATIONS_H
#define TIMEEVALUATIONS_H

#include <sys/time.h> // for timeval and gettimeofday()

class TimeEvaluations
{

public:
    TimeEvaluations()
    {
        this->timevalue_ms = -1;
    }

    struct timeval tv;
    double timevalue_ms;

    void howLongItStart()
    {
        if(gettimeofday(&tv, 0) < 0){
            qWarning() << "howLongItStart(): Error gettimeofday()";
        }else{
            this->timevalue_ms = (tv.tv_sec % 1000) * 1000.0 + tv.tv_usec / 1000.0;
        }
    }

    double howLongItEnd()
    {
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

};

#endif // TIMEEVALUATIONS_H
