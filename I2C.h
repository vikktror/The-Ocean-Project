/* 
 * File:   I2C.h
 * Author: Viktor
 *
 * Created on den 2 march 2017, 22:42
 */

#include "Datatypes.h"

#ifndef I2C_H
#define	I2C_H

vo voI2COpen(vo);
vo voI2CStart(vo);
vo voI2CTx(u8 u8Byte);
vo voI2CACK(vo);
vo voI2CNACK(vo);
vo voI2CRepeatedStart(vo);
u8 u8I2CRx(vo);
vo voI2CStop(vo);

#endif	/* I2C_H */

