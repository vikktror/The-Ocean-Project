/*
 * File:   MotorDriver.c
 * Author: Viktor
 *
 * Created on den 19 mars 2017, 20:19
 */

#include <xc.h>

#include "DataTypes.h"
#include "MotorDriver.h"

#define MOTOR_DRIVER_PHASE_TRIS (TRISDbits.TRISD11)

/* When 0 -> current flows from OUT1 to OUT2 on motor driver carrier */
#define MOTOR_DRIVER_PHASE       (LATDbits.LATD11)

/* Using PWM1 channel, DC = duty cycle */
#define MOTOR_DC                 (OC1RS)

#define UPPER_LIMIT_DC           (639 + 1)


 /*
 *******************************************************************************
 * FUNCTION NAME : voMotorInit
 *
 * DESCRIPTION   : Initiate Timer2 for PWM channel 1, init I/O:s etc.
 *
 * INPUT         : -
 *
 * OUTPUT        : -
 *
 * NOTE          : -
 *
 *******************************************************************************
 */
vo voMotorInit(vo)
{
   /* PWM on Timer2 */
   T2CONbits.T32 = 0;      /* Act as 16 bit Timer */
   T2CONbits.TCKPS = 0b00; /* Prescaler 1:1 */
   T2CONbits.TCS = 0;      /* Clock source = Internal clock (FOSC/2) */
   T2CONbits.TGATE = 0;    /* Gated time accumulation is disabled */
   PR2 = 639;              /* Makes it run at 25kHz      */
   T2CONbits.TON = 1;      /* Starts timer               */

   /* Initiate IO-PORTS */
   MOTOR_DRIVER_PHASE_TRIS = 0;
   
   /* Initial duty cycle*/
   OC1R = MOTOR_DC = 0;

   OC1CON = 0x0006;        /* Timer2, fault pin disabled for Motor */
   MOTOR_DRIVER_PHASE = 0;
}

 /*
 *******************************************************************************
 * FUNCTION NAME : voMotorForward
 *
 * DESCRIPTION   : Sets the direction of motor to forward
 *
 * INPUT         : -
 *
 * OUTPUT        : -
 *
 * NOTE          : -
 *
 *******************************************************************************
 */
vo voMotorForward(vo)
{
   MOTOR_DRIVER_PHASE = 0;
}

 /*
 *******************************************************************************
 * FUNCTION NAME : voMotorBackward
 *
 * DESCRIPTION   : Sets the direction of motor to backward
 *
 * INPUT         : -
 *
 * OUTPUT        : -
 *
 * NOTE          : -
 *
 *******************************************************************************
 */
vo voMotorBackward(vo)
{
   MOTOR_DRIVER_PHASE = 1;
}

 /*
 *******************************************************************************
 * FUNCTION NAME : voMotorSetSpeed
 *
 * DESCRIPTION   : Changes the motor speed (PWM)
 *
 * INPUT         : 16-bit number (max 639)
 *
 * OUTPUT        : -
 *
 * NOTE          : -
 *
 *******************************************************************************
 */
vo voMotorSetSpeed(u16 u16MotorSpeed)
{
   if (u16MotorSpeed < UPPER_LIMIT_DC)
   {
      MOTOR_DC = u16MotorSpeed;
   }
}

 /*
 *******************************************************************************
 * FUNCTION NAME : voMotorStop
 *
 * DESCRIPTION   : Stops the motor
 *
 * INPUT         : -
 *
 * OUTPUT        : -
 *
 * NOTE          : -
 *
 *******************************************************************************
 */
vo voMotorStop(vo)
{
   MOTOR_DC = 0;
}