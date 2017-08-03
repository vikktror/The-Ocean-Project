#include <xc.h>
#include <stdio.h>
#include <string.h>

#include "DataTypes.h"
#include "OLED.h"
#include "PumpNavigation.h"
#include "RTC.h"
#include "ScheduleDriver.h"
#include "Watering.h"

#define WATER_PUMP      (LATDbits.LATD1)
#define WATER_PUMP_TRIS (TRISDbits.TRISD1)

#define FEED_MAX_SIZE   (5)

/* Lock when a pot is to be watered */
static u8 msu8WateringMutex = 0;

static SCHEDULE_ITEM msstWateringFeedArr[FEED_MAX_SIZE];
static u8 msu8FeedSize = 0;

u16 mu16WateringDelay;
u8 mu8FeedDelay;

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voWateringInit
 * 
 * DESCRIPTION    : Initiates I/Os used by the watering driver
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
vo voWateringInit(vo)
{
   WATER_PUMP_TRIS = 0;
   WATER_PUMP = 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8FeedPushBack
 * 
 * DESCRIPTION    : Adds an item to the end of feed
 * 
 * INPUT          : Pointer to SCHEDULE_ITEM to push
 * 
 * OUTPUT         : 8-bit number. 1 if OK else 0.
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8FeedPushBack(SCHEDULE_ITEM* stItem)
{
   if (msu8FeedSize < (FEED_MAX_SIZE))
   {
      msstWateringFeedArr[msu8FeedSize].u8Pot = stItem->u8Pot;
      msstWateringFeedArr[msu8FeedSize].u8Duration = stItem->u8Duration;
      msstWateringFeedArr[msu8FeedSize].u8Repeat = stItem->u8Repeat;
      memset(msstWateringFeedArr[msu8FeedSize].u8Time, '\0', sizeof(msstWateringFeedArr[msu8FeedSize].u8Time));
      memcpy(msstWateringFeedArr[msu8FeedSize].u8Time, stItem->u8Time, sizeof(msstWateringFeedArr[msu8FeedSize].u8Time));
      
      msu8FeedSize++;
  
      return 1;
   }
   return 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voFeedPopFront
 * 
 * DESCRIPTION    : Removes item at front of the feed
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Shifts all items one step left as removal
 * 
******************************************************************************** 
*/
vo voFeedPopFront(vo)
{
   if (msu8FeedSize > 0)
   {
      u8 u8Counter;

      for (u8Counter = 0; u8Counter < msu8FeedSize; u8Counter++)
      {
         msstWateringFeedArr[u8Counter].u8Pot = msstWateringFeedArr[u8Counter + 1].u8Pot;
         msstWateringFeedArr[u8Counter].u8Duration = msstWateringFeedArr[u8Counter + 1].u8Duration;
         msstWateringFeedArr[u8Counter].u8Repeat = msstWateringFeedArr[u8Counter + 1].u8Repeat;
         memcpy(msstWateringFeedArr[u8Counter].u8Time, msstWateringFeedArr[u8Counter + 1].u8Time, sizeof(msstWateringFeedArr[u8Counter].u8Time));
      }

      msstWateringFeedArr[msu8FeedSize].u8Pot = '\0';
      msstWateringFeedArr[msu8FeedSize].u8Duration = '\0';
      msstWateringFeedArr[msu8FeedSize].u8Repeat = '\0';
      memset(msstWateringFeedArr[msu8FeedSize].u8Time, '\0', sizeof(msstWateringFeedArr[msu8FeedSize].u8Time));
      
      msu8FeedSize--;
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8WateringGetMutexStatus
 * 
 * DESCRIPTION    : Get status of the watering mutex
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
******************************************************************************** 
*/
u8 u8WateringGetMutexStatus(vo)
{
   return msu8WateringMutex;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voWateringFeedTask
 * 
 * DESCRIPTION    : Updates the watering feed
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Can hang the program if feed is full!
******************************************************************************** 
*/
vo voWateringFeedTask(vo)
{
   typedef enum
   {
      UPDATE_FEED = 0,
      WAIT,
      FATAL_ERROR
   }FEED_TYPE;
   
   static FEED_TYPE senState = UPDATE_FEED;
   
   static SCHEDULE_ITEM sstEventToAddPtr;
   
   /* This variable cannot be cleared in SW. Reboot PIC24 */
   static u8 u8Status = 1;
   
   switch (senState)
   {      
      case UPDATE_FEED:
         /* While loop for reading every possible pending watering at once */
         while (u8ScheduleCompareFirst() && msu8FeedSize < FEED_MAX_SIZE) /* When 4 elements in feed, size = 4, [0, 1, 2, 3] */
         {
            /* Get the first element from the schedule */
            voSchdeuleGetFirstElement(&sstEventToAddPtr);
            
            /* Copy the first element from the schedule to the watering feed */
            u8Status = u8FeedPushBack(&sstEventToAddPtr);
            
            if(!u8Status)
            {
               /* Feed was (over)full, fatal error, break out of while loop */
               senState = FATAL_ERROR;
               break;
            }
            
            /* Pop front of schedule */
            voSchedulePopFront();
         }
         
         if(u8Status)
         {
            senState = WAIT;
            mu8FeedDelay = 50;
         }
      break;
      
      case WAIT:
         if (mu8FeedDelay == 0)
         {
            senState = UPDATE_FEED;
         }
      break;
      
      case FATAL_ERROR:
         voOLEDClear();
         voOLEDHome();
         printf("Watering feed");
         voOLEDRowTwo();
         printf("full! Reboot.");
         
         /**************************************************************************************************************************
          * Stop motors and shut off relay
          **************************************************************************************************************************/
         while(1);
      break;
      
      default:
      break;
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voWateringTask
 * 
 * DESCRIPTION    : The watering task 
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Locks the mutex when a watering is due
******************************************************************************** 
*/
vo voWateringTask(vo)
{
   typedef enum
   {
      CHECK_FEED_SIZE = 0,
      NAVIGATE,
      START_PUMP,
      WATER,
      WAIT
   }WATERING_TYPE;

   static WATERING_TYPE senState = CHECK_FEED_SIZE;
   static u8 su8ScreenArr[32];
   
   switch (senState)
   {
      case CHECK_FEED_SIZE:
         if (msu8FeedSize > 0)
         {
            /* A watering is due. Lock the mutex! */
            msu8WateringMutex = 1;
            voRTCPrintDisable(WATERING);
            
            voOLEDReadScreen(su8ScreenArr);
            
            voOLEDClear();
            voOLEDHome();
            
            printf("Watering pot: %u", msstWateringFeedArr[0].u8Pot);
            
            senState = NAVIGATE;
         }
         else
         {
            senState = WAIT;
            mu16WateringDelay = 50;
         }
      break;
      
      case NAVIGATE:
         if (u8PumpNavigationTask(msstWateringFeedArr[0].u8Pot) == 1)
         {
            senState = START_PUMP;
         }
         else if (u8PumpNavigationTask(msstWateringFeedArr[0].u8Pot) == 2)
         {
            voOLEDClear();
            voOLEDHome();
            printf("Motor error!");
            voOLEDRowTwo();
            printf("Please reboot...");
            while(1);
         }
      break;
      
      case START_PUMP:
         WATER_PUMP = 1;
         mu16WateringDelay = (10 * (u16)msstWateringFeedArr[0].u8Duration);
         senState = WATER;
      break;
      
      case WATER:
         // Check if the watering duration has reached 0.
         if (mu16WateringDelay == 0)
         {
            WATER_PUMP = 0;
            
            /* Reschedule watering if required */
            voScheduleReschedule(msstWateringFeedArr[0]);
            
            /* Enable printing for the RTC */
            voRTCPrintEnable(WATERING);
            
            /* Pop front of the feed */
            voFeedPopFront();
            
            voOLEDClear();
            voOLEDHome();
            
            if (msu8FeedSize == 0)
            {
               voOLEDPrint32BArr(su8ScreenArr);
            }
            
            /* Unlock the watering mutex */
            msu8WateringMutex = 0;
            senState = CHECK_FEED_SIZE;
         }
      break;
      
      case WAIT:
         if (mu16WateringDelay == 0)
         {
            senState = CHECK_FEED_SIZE;
         }
      break;
      
      default:
      break;
   }
}

