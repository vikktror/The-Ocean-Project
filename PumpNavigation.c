#include <stdio.h>

#include "PumpNavigation.h"
#include "ADCDriver.h"
#include "MotorDriver.h"
#include "OLED.h"
#include "Timer.h"

u8 mu8PumpNavigationDelay;

typedef enum
{
   INITIAL_CHECK = 0,
   NAVIGATE,
   CHECK_TIMER,
   READ_SENSOR,
   INTERPRET_SENSOR,
   FINE_TUNING,
   CHECK_POT_NO,
   WAIT
}NAVIGATION_TYPE;


/*
 * > 600 = right in front of pot
 * < 600 && > 400 = close to pot
 * < 400 = in between pots
 */
typedef enum
{
   IN_POSITION = 0,
   CLOSE,
   IN_BETWEEN
}POSITION;

typedef enum
{
   FORWARD = 0,
   BACKWARD
}DIRECTION;

/*
 *******************************************************************************
 * FUNCTION NAME : u8PumpNavigationTask
 *
 * DESCRIPTION   : Automatic control of the motor. Counts pot and starts timer, 
 *                 etc...
 *
 * INPUT         : The desired pot to stop at.
 *
 * OUTPUT        : 8-bit number (1 = success, 0 = nDone, 2 = timer error).
 *
 * NOTE          : The current pot number must be 1 at first call after reboot.
 *
 *******************************************************************************
 */
u8 u8PumpNavigationTask(u8 u8DesiredPot)
{
   /* The starting position of the pump shall be at first pot (looking at device from behind, left -> right). */
   static u8 su8CurrentPot = 1;
   
   static NAVIGATION_TYPE senState = INITIAL_CHECK;
   static POSITION senPosition = IN_POSITION;
   static POSITION senPrePosition = IN_POSITION;
   static DIRECTION senDirection = FORWARD;
   
   static u16 su16SensorVal = 0;
   static u8 u8MaxTime = 0;
   static u8 u8DistanceInPots = 0;  /* The distance in pots */
   
   switch (senState)
   {
      /* Make an initial check, once for each new watering */
      case INITIAL_CHECK:
         if (u8DesiredPot == su8CurrentPot)
         {
            return 1;
         }
         else
         {
            voTimerStart();
            
            /* Calculate number of pots to travel */
            if (u8DesiredPot > su8CurrentPot)
            {
               u8DistanceInPots = (u8DesiredPot - su8CurrentPot);
            }
            else
            {
               u8DistanceInPots = (su8CurrentPot - u8DesiredPot);
            }
            
            /* Calculate maxtime */
            u8MaxTime = (16 * u8DistanceInPots) + 2;
            
            senState = NAVIGATE;
         }
      break;
      
      case NAVIGATE:
         voMotorSetSpeed(400);
         
         if (u8DesiredPot > su8CurrentPot)
         {
            voMotorForward();
            senDirection = FORWARD;
         }
         else if (u8DesiredPot < su8CurrentPot)
         {
            voMotorBackward();
            senDirection = BACKWARD;
         }
         
         senState = CHECK_TIMER;
      break;
      
      case CHECK_TIMER: 
         if (u8MaxTime < u8TimerGetSeconds())
         {
            /* Timer error */
            voTimerStop();
            voTimerReset();
            
            voMotorStop();
            return 2;
         }
         
         senState = READ_SENSOR;
      break;
      
      /* Read the sensor and set position variable */
      case READ_SENSOR:
         su16SensorVal = u16ADCRead10Bit();
         senState = INTERPRET_SENSOR;
      break;
      
      case INTERPRET_SENSOR:
         senPrePosition = senPosition;
         
         /* Interpret the sensor value */
         if (su16SensorVal >= 600)
         {
            senPosition = IN_POSITION;
         }
         else if (su16SensorVal < 600 && su16SensorVal >= 400)
         {
            senPosition = CLOSE;
         }
         else
         {
            senPosition = IN_BETWEEN;
         }
         
         /* Change the current pot number if needed */
         if ( senPosition != IN_BETWEEN && senPrePosition == IN_BETWEEN)
         {
            if (senDirection == FORWARD)
            {
               su8CurrentPot++;
            }
            else if (senDirection == BACKWARD)
            {
               su8CurrentPot--;
            }
         }
         
         if (senPosition == IN_POSITION && u8DesiredPot == su8CurrentPot)
         {
            /* DONE */
            voMotorStop();
            voMotorSetSpeed(0);
            
            voTimerStop();
            voTimerReset();
            
            senState = INITIAL_CHECK;
            return 1;
         }
         else if (senPosition == CLOSE && u8DesiredPot == su8CurrentPot)
         {
            senState = FINE_TUNING;
         }
         else
         {
            mu8PumpNavigationDelay = 10;
            senState = WAIT;
         }
      break;
      
      case FINE_TUNING:
         voMotorSetSpeed(200);
         mu8PumpNavigationDelay = 5;
         senState = WAIT;
      break;
      
      case WAIT:
         if (mu8PumpNavigationDelay == 0)
         {
            senState = CHECK_TIMER;
         }
      break;
      
      default:
      break;
   }
   
   return 0;
}
