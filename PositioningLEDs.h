/* 
 * File:   PositioningLEDs.h
 * Author: Viktor
 *
 * Created on den 24 april 2017, 23:40
 * 
 * Contains code for the LEDs used for positioning the watering device in front
 * of correct pot.
 * 
 */

#ifndef POSITIONINGLEDS_H
#define	POSITIONINGLEDS_H

#include "DataTypes.h"

vo voPositioningLEDsInit(vo);
u8 u8PositioningLEDsSet(u8 u8PotNumber);
vo voPositioningLEDsOff(vo);

#endif	/* POSITIONINGLEDS_H */

