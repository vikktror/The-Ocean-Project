/* 
 * File:   MotorDriver.h
 * Author: Viktor
 *
 * Created on den 19 mars 2017, 20:44
 */

#ifndef MOTORDRIVER_H
#define	MOTORDRIVER_H

vo voMotorInit(vo);
vo voMotorForward(vo);
vo voMotorBackward(vo);
vo voMotorSetSpeed(u16 u16MotorSpeed);
vo voMotorStop(vo);

#endif	/* MOTORDRIVER_H */

