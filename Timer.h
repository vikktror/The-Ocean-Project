/* 
 * File:   Timer.h
 * Author: Viktor
 *
 * Created on den 3 augusti 2017, 18:09
 */

#ifndef TIMER_H
#define	TIMER_H

#include "DataTypes.h"
#include <xc.h>

vo voTimerStart(vo);
vo voTimerReset(vo);
vo voTimerStop(vo);
u8 u8TimerGetSeconds(vo);
vo voTimerTask(vo);

#endif	/* TIMER_H */

