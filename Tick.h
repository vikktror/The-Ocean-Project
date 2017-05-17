/* 
 * File:   Tick.h
 * Author: Viktor
 *
 * Created on den 5 mars 2017, 17:01
 */

#include "DataTypes.h"

#ifndef TICK_H
#define	TICK_H

typedef enum
{
    OFF = 0,
    ON
}TICK_STATUS;

vo voTickTask(u8 u8Delay);
vo voTickReset(vo);
TICK_STATUS enGetTick(vo);

#endif	/* TICK_H */

