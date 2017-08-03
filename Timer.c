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

vo voTimerStart(vo)
{
   msenStartReq = START;
}

vo voTimerStop(vo)
{
   msenStopReq = STOP;
}

vo voTimerReset(vo)
{
   msu8Seconds = 0;
}

u8 u8TimerGetSeconds(vo)
{
   return msu8Seconds;
}

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
