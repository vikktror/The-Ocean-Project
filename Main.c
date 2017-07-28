#include <stdio.h>

#include "ADCDriver.h"
#include "Configuration.h"
#include "DataTypes.h"
#include "DistanceSensor.h"
#include "I2C.h"
#include "MainProgram.h"
#include "MenuDriver.h"
#include "MotorDriver.h"
#include "OLED.h"
#include "PumpNavigation.h"
#include "RotaryEncoder.h"
#include "RTC.h"
#include "ScheduleDriver.h"
#include "StatusLED.h"
#include "Tick.h"
#include "Watering.h"


/*
********************************************************************************
 * 
 * FUNCTION NAME  : voBoardInit
 * 
 * DESCRIPTION    : Initates timers, I/Os
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Run this function first at power on
******************************************************************************** 
*/
vo voBoardInit(vo)
{
   TRISAbits.TRISA0 = 0;         /* Set RA0 as output */
   
   /* Initiate timer1 interrupt every 0,01s */
   _T1IP = 6;                    /* Next highest priority on timer1 interrupt */
   PR1 = 625;                    /* To get an interrupt exactly every 0,01s */
   T1CON = 0b1000000000110000;   /* Set prescaler = 256 and turn on timer1 ect... */
   _IPL = 1;                     /* CPU priority */
   INTCON1bits.NSTDIS = 1;       /* Disable nested interrupts */
   _T1IE = 1;                    /* Enable timer1 interrupt */
   _T1IF = 0;                    /* Pull down interrupt flag */
   
   voOLEDInit();
   voI2COpen();
   voRTCInit();
   voRotaryEncoderInit();
   voMotorInit();
   voADCInit();
   voWateringInit();
}

int main(vo)
{
   /* Initiation sequense */
   voBoardInit();
   
   while (1)
   {
      voLEDBlink();
      voRTCTask();
      voWateringFeedTask();
      voWateringTask();
      
      if (!u8WateringGetMutexStatus())
      {
         voMainProgramTask();
      } 
   }
}
