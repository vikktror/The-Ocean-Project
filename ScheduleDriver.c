#include <xc.h>
#include <stdio.h>
#include <string.h>

#include "DataTypes.h"
#include "ScheduleDriver.h"
#include "RTC.h"

#define MAX_SIZE  (5)

#define TRUE      (1)
#define FALSE     (0)



static SCHEDULE_ITEM msstScheduleArr[MAX_SIZE];
static u8 msu8Size = 0;
/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8ScheduleGetSize
 * 
 * DESCRIPTION    : Interface-funcion, returns size of current schedule
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8ScheduleGetSize(vo)
{
   return msu8Size;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voScheduleGetElementTime
 * 
 * DESCRIPTION    : Returns the time array of a choosen element
 * 
 * INPUT          : 8-bit number (element number), 8-bit pointer to arr of size 7
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Doesn't return function, but changes the input array
 * 
******************************************************************************** 
*/
vo voScheduleGetElementTime(u8 u8ElementNum, u8* u8DateTimeArr)
{
   u8 u8Index;
   
   for (u8Index = 0; u8Index < 7; u8Index++)
   {
      u8DateTimeArr[u8Index] = msstScheduleArr[u8ElementNum].u8Time[u8Index];
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voSchedulePrintElement
 * 
 * DESCRIPTION    : Prints the scheduled element
 * 
 * INPUT          : 8-bit number.
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Does not print the duration.      
 *                  Format: YYYY-MM-DD XXX.
 *                          HH:MM:SS Pot: X
 * 
******************************************************************************** 
*/
vo voSchedulePrintElement(u8 u8ElementNum)
{
   if (u8ElementNum < msu8Size)
   {
      voRTCPrintDateTime(msstScheduleArr[u8ElementNum].u8Time);
      printf("Pot: %u", msstScheduleArr[u8ElementNum].u8Pot);
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voSchdeuleGetFirstElement
 * 
 * DESCRIPTION    : Returns the first element from the schedule
 * 
 * INPUT          : pointer to SCHEDULE_ITEM
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Changes the item pointed at by the pointer
 * 
******************************************************************************** 
*/
vo voSchdeuleGetFirstElement(SCHEDULE_ITEM* stFirstElementPtr)
{
   u8 u8Index;
   
   stFirstElementPtr->u8Pot = msstScheduleArr[0].u8Pot;
   stFirstElementPtr->u8Amount = msstScheduleArr[0].u8Amount;
   stFirstElementPtr->u8Repeat = msstScheduleArr[0].u8Repeat;
   
   for (u8Index = 0; u8Index < 7; u8Index++)
   {
      stFirstElementPtr->u8Time[u8Index] = msstScheduleArr[0].u8Time[u8Index];
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8FirstIsSmaller
 * 
 * DESCRIPTION    : Checks if the first array is smaller than the second
 * 
 * INPUT          : 2x 8-bit pointers to arrays of size 7 
 * 
 * OUTPUT         : 8-bit number. 1 if first arr is smaller, else 0.
 * 
 * NOTE           : No interface function
 * 
******************************************************************************** 
*/
u8 u8FirstIsSmaller(u8* u8FirstArr, u8* u8SecArr)
{
   u8 u8Counter;
   
   for (u8Counter = 0; u8Counter < 7; u8Counter++)
   {
      if (u8Counter == 3)
      {
         continue;
      }
      if (u8FirstArr[u8Counter] < u8SecArr[u8Counter])
      {
         return 1;
      }
      else if (u8FirstArr[u8Counter] > u8SecArr[u8Counter])
      {
         return 0;
      }
   }
   return 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8AreEqual
 * 
 * DESCRIPTION    : Checks if the arrays are equal
 * 
 * INPUT          : 2x 8-bit pointers to arrays of size 7 
 * 
 * OUTPUT         : 8-bit number. 1 if equal, else 0
 * 
 * NOTE           : No interface function
 * 
******************************************************************************** 
*/
u8 u8AreEqual(u8* u8FirstArr, u8* u8SecArr)
{
   u8 u8Index;
   
   for (u8Index = 0; u8Index < 7; u8Index++)
   {
      if (u8Index == 3)
      {
         continue;
      }
      
      if (u8FirstArr[u8Index] != u8SecArr[u8Index])
      {
         return 0;
      }
   }
   
   return 1;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voScheduleSort
 * 
 * DESCRIPTION    : Sorts the schedule by scheduled date and time
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Uses insert sort algoritm.
 * 
******************************************************************************** 
*/
vo voScheduleSort(vo)
{
   u8 u8Outer;
   u8 u8Inner;
   
   SCHEDULE_ITEM stTemp;
   
   if (msu8Size > 0)
   {
      /* Start sorting from the next leftmost item because the leftmost item has no item on left hand side */
      for (u8Outer = 1; u8Outer < msu8Size; u8Outer++)
      {
         for (u8Inner = u8Outer; u8Inner > 0; u8Inner--)
         {
            /* If right element is closer in time -> shift left */
            if (u8FirstIsSmaller(msstScheduleArr[u8Inner].u8Time, msstScheduleArr[u8Inner - 1].u8Time))
            {
               /* Copy right hand item to a temporary item */
               stTemp.u8Pot = msstScheduleArr[u8Inner].u8Pot;
               stTemp.u8Amount = msstScheduleArr[u8Inner].u8Amount;
               stTemp.u8Repeat = msstScheduleArr[u8Inner].u8Repeat;
               memset(stTemp.u8Time, '\0', sizeof(stTemp.u8Time));
               memcpy(stTemp.u8Time, msstScheduleArr[u8Inner].u8Time, sizeof(msstScheduleArr[u8Inner].u8Time));

               /* Replace the right hand item with the left hand one */
               msstScheduleArr[u8Inner].u8Pot = msstScheduleArr[u8Inner - 1].u8Pot;
               msstScheduleArr[u8Inner].u8Amount = msstScheduleArr[u8Inner - 1].u8Amount;
               msstScheduleArr[u8Inner].u8Repeat = msstScheduleArr[u8Inner -1].u8Repeat;
               memcpy(msstScheduleArr[u8Inner].u8Time, msstScheduleArr[u8Inner - 1].u8Time, sizeof(msstScheduleArr[u8Inner].u8Time));

               /* At last: copy the temporary to the left hand item */
               msstScheduleArr[u8Inner - 1].u8Pot = stTemp.u8Pot;
               msstScheduleArr[u8Inner -1].u8Amount = stTemp.u8Amount;
               msstScheduleArr[u8Inner -1].u8Repeat = stTemp.u8Repeat;
               memcpy(msstScheduleArr[u8Inner - 1].u8Time, stTemp.u8Time, sizeof(msstScheduleArr[u8Inner - 1].u8Time));
            }
            /* If the current inner element is bigger than the left one, it will be bigger than all left hand elements */
            else                      
            {
               break;
            }
         }
      }
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8SchedulePushBack
 * 
 * DESCRIPTION    : Adds an item to the end of schedule
 * 
 * INPUT          : 8-bit number (the pot to be watered), 8-bit number (duration),
 *                  8-bit number (repeat), 8-bit pointer of size 7
 * 
 * OUTPUT         : 8-bit number (0 = fail, 1 = OK)
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8SchedulePushBack(u8 u8Pot, u8 u8Amount, u8 u8Repeat, u8* u8DateTime)
{
   u8 u8CurrentTimeArr[7];
   
   voRTCGetDateTime(u8CurrentTimeArr);
   
   if (msu8Size < (MAX_SIZE) && u8FirstIsSmaller(u8CurrentTimeArr, u8DateTime))
   {
      msstScheduleArr[msu8Size].u8Pot = u8Pot;
      msstScheduleArr[msu8Size].u8Amount = u8Amount;
      msstScheduleArr[msu8Size].u8Repeat = u8Repeat;
      memset(msstScheduleArr[msu8Size].u8Time, '\0', sizeof(msstScheduleArr[msu8Size].u8Time));
      memcpy(msstScheduleArr[msu8Size].u8Time, u8DateTime, sizeof(msstScheduleArr[msu8Size].u8Time));
      
      msu8Size++;
      voScheduleSort();
      
      return 1;
   }
   else
   {
      return 0;
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8SchedulePopFront
 * 
 * DESCRIPTION    : Removes item at front of schedule
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Shifts all items one step left as removal
 * 
******************************************************************************** 
*/
vo voSchedulePopFront(vo)
{
   if (msu8Size > 0)
   {
      u8 u8Counter;

      for (u8Counter = 0; u8Counter < msu8Size; u8Counter++)
      {
         msstScheduleArr[u8Counter].u8Pot = msstScheduleArr[u8Counter + 1].u8Pot;
         msstScheduleArr[u8Counter].u8Amount = msstScheduleArr[u8Counter + 1].u8Amount;
         msstScheduleArr[u8Counter].u8Repeat = msstScheduleArr[u8Counter + 1].u8Repeat;
         memcpy(msstScheduleArr[u8Counter].u8Time, msstScheduleArr[u8Counter + 1].u8Time, sizeof(msstScheduleArr[u8Counter].u8Time));
      }

      msstScheduleArr[msu8Size].u8Pot = '\0';
      msstScheduleArr[msu8Size].u8Amount = '\0';
      msstScheduleArr[msu8Size].u8Repeat = '\0';
      memset(msstScheduleArr[msu8Size].u8Time, '\0', sizeof(msstScheduleArr[msu8Size].u8Time));
      
      msu8Size--;
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voScheduleRemoveAny
 * 
 * DESCRIPTION    : Removes any element from schedule
 * 
 * INPUT          : 8-bit number (element number)
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Shifts all items one step left as removal
 * 
******************************************************************************** 
*/
vo voScheduleRemoveAny(u8 u8ElementNum)
{
   if (u8ElementNum < msu8Size && msu8Size > 0)
   {
      u8 u8Counter;

      for (u8Counter = u8ElementNum; u8Counter < msu8Size; u8Counter++)
      {
         msstScheduleArr[u8Counter].u8Pot = msstScheduleArr[u8Counter + 1].u8Pot;
         msstScheduleArr[u8Counter].u8Amount = msstScheduleArr[u8Counter + 1].u8Amount;
         msstScheduleArr[u8Counter].u8Repeat = msstScheduleArr[u8Counter + 1].u8Repeat;
         memcpy(msstScheduleArr[u8Counter].u8Time, msstScheduleArr[u8Counter + 1].u8Time, sizeof(msstScheduleArr[u8Counter].u8Time));
      }

      msstScheduleArr[msu8Size].u8Pot = '\0';
      msstScheduleArr[msu8Size].u8Amount = '\0';
      msstScheduleArr[msu8Size].u8Repeat = '\0';
      memset(msstScheduleArr[msu8Size].u8Time, '\0', sizeof(msstScheduleArr[msu8Size].u8Time));
      
      msu8Size--;
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voScheduleChangeAny
 * 
 * DESCRIPTION    : Removes any element from schedule
 * 
 * INPUT          : 8-bit number (event number), 8-bit number (Pot number),
 *                  8-bit number (duration), 8-bit number (repeat),
 *                  8-bit pointer to array of size 7.
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8ScheduleChangeAny(u8 u8ElementNum, u8 u8Pot, u8 u8Amount, u8 u8Repeat, u8* u8DateTimeArr)
{
   u8 u8CurrentTimeArr[7];
   voRTCGetDateTime(u8CurrentTimeArr);
   
   if (u8ElementNum < msu8Size && msu8Size > 0 && u8FirstIsSmaller(u8CurrentTimeArr, u8DateTimeArr))
   {
      msstScheduleArr[u8ElementNum].u8Pot = u8Pot;
      msstScheduleArr[u8ElementNum].u8Amount = u8Amount;
      msstScheduleArr[u8ElementNum].u8Repeat = u8Repeat;
      memcpy(msstScheduleArr[u8ElementNum].u8Time, u8DateTimeArr, sizeof(msstScheduleArr[u8ElementNum].u8Time));
      voScheduleSort();
      
      return 1;
   }
   
   return 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8ScheduleCompareFirst
 * 
 * DESCRIPTION    : Compares the first watering in array with current time
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number (1 if equal, 0 else)
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8ScheduleCompareFirst(vo)
{
   u8 u8Index;
   u8 u8CurrentTime[7];
   u8 u8ScheduledTime[7];
   
   if (msu8Size > 0)
   {
      voRTCGetDateTime(u8CurrentTime);

      for (u8Index = 0; u8Index < 7; u8Index++)
      {
         u8ScheduledTime[u8Index] = msstScheduleArr[0].u8Time[u8Index];
      }

      if (u8AreEqual(u8CurrentTime, u8ScheduledTime))
      {
         return 1;
      }
   }
   return 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voScheduleReschedule
 * 
 * DESCRIPTION    : Called upon when a watering shall be rescheduled after it's
 *                  executed. Calculates the date for the new watering.
 * 
 * INPUT          : struct SCHEDULE_ITEM
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
vo voScheduleReschedule(SCHEDULE_ITEM stWatering)
{
   u8 u8DaysInMonthArr[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   const static u8 csu8DaysToAddArr[4] = {0, 1, 2, 7};
   
   u8 u8NewDay;
   u8 u8NewMonth;
   u8 u8NewYear;
   
   if (stWatering.u8Repeat != 0)
   {
      /* Check if leap year */
      if ( (stWatering.u8Time[0] % 4) == 0 )
      {
         u8DaysInMonthArr[1] = 29;
      }
      
      u8NewYear = stWatering.u8Time[0];
      u8NewMonth = stWatering.u8Time[1];
      u8NewDay = stWatering.u8Time[2] + csu8DaysToAddArr[stWatering.u8Repeat];
      
      /* If new watering is to be scheduled for next month */
      if (u8NewDay > u8DaysInMonthArr[stWatering.u8Time[1] - 1])
      {
         /* Calculate new day number */
         u8NewDay = u8NewDay % u8DaysInMonthArr[stWatering.u8Time[1] - 1];
         u8NewMonth = u8NewMonth + 1;
         
         /* If new watering is to be scheduled for next year */
         if (u8NewMonth > 12)
         {
            /* Calculate new month and year number */
            u8NewMonth = u8NewMonth % 12;
            u8NewYear = (u8NewYear + 1) % 100;
         }
      }
      
      stWatering.u8Time[0] = u8NewYear;
      stWatering.u8Time[1] = u8NewMonth;
      stWatering.u8Time[2] = u8NewDay;
      
      /* Add item to the schedule */
      u8SchedulePushBack(stWatering.u8Pot, stWatering.u8Amount, stWatering.u8Repeat, stWatering.u8Time);
   }
}