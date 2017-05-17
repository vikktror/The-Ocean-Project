/* 
 * File:   Watering.h
 * Author: Viktor
 *
 * Created on den 29 mars 2017, 23:30
 */

#ifndef WATERING_H
#define	WATERING_H

vo voWateringInit(vo);
u8 u8WateringGetMutexStatus(vo);
vo voWateringFeedTask(vo);
vo voWateringTask(vo);

#endif	/* WATERING_H */

