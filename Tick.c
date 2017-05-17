#include <xc.h>

#include "Tick.h"

u8 mu8TickDelay;

static TICK_STATUS msenTick = ON;

typedef enum
{
   TICK = 0,
   WAIT
}TICK_STATE;

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voGetTick
 * 
 * DESCRIPTION    : Returns the current tick value
 * 
 * INPUT          : -
 * 
 * OUTPUT         : Enum TICK_STATUS
 * 
 * NOTE           : -
******************************************************************************** 
*/
TICK_STATUS enGetTick(vo)
{
   return msenTick;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voTickReset
 * 
 * DESCRIPTION    : Resets the TICK_STATUS variable to OFF
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voTickReset(vo)
{
   msenTick = OFF;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voTickTask
 * 
 * DESCRIPTION    : Ticks on/off every (u8Delay * 0,01)s
 * 
 * INPUT          : 8-bit number
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voTickTask(u8 u8Delay)
{
   static TICK_STATUS senState = TICK;
   
   switch(senState)
   {
      case TICK:
         msenTick ^= 1;
         senState = WAIT;
      break;
         
      case WAIT:
         if (mu8TickDelay == 0)
         {
            mu8TickDelay = u8Delay;
            senState = TICK;
         }
      break;
         
      default:
      break;
   }
}
