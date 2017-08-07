#include <stdio.h>

#include "ADCDriver.h"
#include "Configuration.h"
#include "DataTypes.h"
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
#include "Timer.h"
#include "Watering.h"

u16 mu16BootDelay;


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
   voRotaryEncoderInit();
   voMotorInit();
   voADCInit();
   voWateringInit();
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8PrintBoot
 * 
 * DESCRIPTION    : Prints info about the application at power on
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number (0 = nDone, 1 = Done)
 * 
 * NOTE           : -
******************************************************************************** 
*/
u8 u8PrintBoot(vo)
{  
   typedef enum
   {
      LOGO = 0,
      MADE_BY,
      INTERACTION,
      INFO,
      WAIT
   }BOOT_INFO;
   
   static BOOT_INFO senState = 0;
   static BOOT_INFO senPreState = 0;
   
   switch (senState)
   {
      case LOGO:
         voOLEDClear();
         voOLEDHome();
         printf("   ExtremeH2O   ");
         
         senPreState = senState;
         mu16BootDelay = 400;
         senState = WAIT;
      break;
      
      case MADE_BY:
         voOLEDClear();
         voOLEDHome();
         printf("Made by:");
         voOLEDRowTwo();
         printf("Viktor Holmgren");
         
         senPreState = senState;
         mu16BootDelay = 400;
         senState = WAIT;
      break;
      
      case INTERACTION:
         voOLEDClear();
         voOLEDHome();
         printf("Twist / push btn");
         voOLEDRowTwo();
         printf("to interact.");
         
         senPreState = senState;
         mu16BootDelay = 600;
         senState = WAIT;
      break;
      
      case INFO:
         voOLEDClear();
         voOLEDHome();
         printf("Place sensor at");
         voOLEDRowTwo();
         printf("location no 1.");
         
         senPreState = senState;
         mu16BootDelay = 600;
         senState = WAIT;
      break;
      
      case WAIT:
         if (mu16BootDelay == 0)
         {
            if (senPreState == INFO)
            {
               voOLEDClear();
               voOLEDHome();
               return 1;
            }
            
            senState = senPreState + 1;
         }
      break;
      
      default:
      break;
   }
   
   return 0;
}

int main(vo)
{
   /* Initiation sequense */
   voBoardInit();
   while(!u8PrintBoot());
   voRTCInit();
   
   while (1)
   {
      voLEDBlink();
      voTimerTask();
      voRTCTask();
      voWateringFeedTask();
      voWateringTask();
      
      if (!u8WateringGetMutexStatus())
      {
         voMainProgramTask();
      } 
   }
}
