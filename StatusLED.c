#include <xc.h>
#include "StatusLED.h"

#define  LED_LAT  (LATAbits.LATA0)

u8 mu8LEDDelay;

typedef enum
{
   BLINK = 0,
   WAIT
}BLINK_TYPE;

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voLedBlink
 * 
 * DESCRIPTION    : Blink the LED once every second
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
vo voLEDBlink(vo)
{
   static BLINK_TYPE senState = BLINK;
   
   switch(senState)
   {
      case BLINK:
         LED_LAT ^= 1;
         senState = WAIT;
      break;
         
      case WAIT:
         if(mu8LEDDelay == 0)
         {
            mu8LEDDelay = 50;
            senState = BLINK;
         }
      break;
      
      default:
      break;
   }
}
