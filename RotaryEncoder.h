/* 
 * File:   RotaryEncoder.h
 * Author: Viktor
 *
 * Created on den 4 mars 2017, 13:57
 */

#include "DataTypes.h"

#define ROTARY_A_PORT   (PORTCbits.RC14)
#define ROTARY_B_PORT   (PORTGbits.RG12)

/* Event enum of the rotary function of encoder, must be cleared when an event is acknowledged */
typedef enum
{
    CLEAR = 0,
    CW,
    CCW
}ROTARY_EVENT;

typedef enum
{
    N_PRESSED = 0,
    PRESSED
}ROTARY_BUTTON;

#ifndef ROTARYENCODER_H
#define	ROTARYENCODER_H

vo voRotaryEncoderInit(vo);
vo voRotaryEncoderTask(u8 u8MaxCount, 
                       u8 u8MinCount, 
                       u8 u8WrapAround);
ROTARY_BUTTON enGetRotaryButton(vo);
u8 u8GetRotaryValue(vo);
vo voResetRotaryValue(vo);
vo voSetRotaryValue(u8 u8Value);

#endif	/* ROTARYENCODER_H */

