#include "Timer.h"

u8 mu8TimerDelay;

static u8 msu8Seconds = 0;

typedef enum
{
   nSTART = 0,
   START
}START_REQ;

typedef enum
{
   nSTOP = 0,
   STOP
}STOP_REQ;

static START_REQ msenStartReq = nSTART;
static STOP_REQ msenStopReq = nSTOP;

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voTimerStart
 * 
 * DESCRIPTION    : Request the timer to start.
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : voTimerTask() must run simultaneously.
 * 
******************************************************************************** 
*/
vo voTimerStart(vo)
{
   msenStartReq = START;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voTimerStop
 * 
 * DESCRIPTION    : Request the timer to stop.
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : voTimerTask() must run simultaneously.
 * 
******************************************************************************** 
*/
vo voTimerStop(vo)
{
   msenStopReq = STOP;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voTimerReset
 * 
 * DESCRIPTION    : Resets the time variable counted in voTimerTask().
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
vo voTimerReset(vo)
{
   msu8Seconds = 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8TimerGetSeconds
 * 
 * DESCRIPTION    : Returns the time variable counted in voTimerTask().
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number (number of seconds since timer start).
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8TimerGetSeconds(vo)
{
   return msu8Seconds;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voTimerTask()
 * 
 * DESCRIPTION    : Runs the timer.
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Start/stop/reset/get (value) using the other functions in 
 *                  this module.
 * 
******************************************************************************** 
*/
vo voTimerTask(vo)
{
   typedef enum
   {
      CHECK_START = 0,
      WAIT,
      COUNT_SECONDS,
      CHECK_STOP
   }TIMER_TYPE;
   
   static TIMER_TYPE senState = CHECK_START;
   
   switch (senState)
   {
      case CHECK_START:
         if (msenStartReq == START)
         {
            /* Start timer */
            msenStartReq = nSTART;
            mu8TimerDelay = 100;
            senState = WAIT;
         }
      break;

      case WAIT:
         if (mu8TimerDelay == 0)
         {
            mu8TimerDelay = 100;
            senState = COUNT_SECONDS;
         }
      break;

      case COUNT_SECONDS:
         msu8Seconds++;
         senState = CHECK_STOP;
      break;
      
      case CHECK_STOP:
         if (msenStopReq == STOP)
         {
            msenStopReq = nSTOP;
            senState = CHECK_START;
         }
         else
         {
            senState = WAIT;
         }
      break;
   }
}
