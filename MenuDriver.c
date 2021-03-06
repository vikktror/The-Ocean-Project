#include <xc.h>
#include <stdio.h>
#include <string.h>

#include "ADCDriver.h"
#include "DataTypes.h"
#include "MenuDriver.h"
#include "MotorDriver.h"
#include "OLED.h"
#include "RotaryEncoder.h"
#include "RTC.h"
#include "ScheduleDriver.h"
#include "Tick.h"

static const u8 mscu8DayArr[7][5] = {"Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat.", "Sun."};
static const u8 mscu8RepeatStrArr[4][16] = {"Never", "Every Day", "Every Other Day", "Every Week"};
static const u8 mscu8DirectionArr[2][9] = {"Forward", "Backward"};

u8 mu8MenuHandlerDelay;

typedef enum
{
   UNLOCKED = 0,
   LOCKED
}LOCK_STATUS;

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuSingleLine
 * 
 * DESCRIPTION    : Handling single lined menues of any size
 * 
 * INPUT          : 8-bit number (number of headers in menu - 1),
 *                  8-bit pointer to 2-dimensional array containing headers of
 *                  max 13 bytes each.
 * 
 * OUTPUT         : 8-bit number (choosen header).
 * 
 * NOTE           : Returns 150 when the button is not pressed
 * 
******************************************************************************** 
*/
u8 u8MenuSingleLine(u8 u8NumHeaders, u8 u8MenuHeadersArr[][13])
{
   static u8 su8RotaryCounter;
   static u8 su8PreVal = 150; /* Holds the value of previous value of rotary, 150 is default value*/
   
   /* Run the rotary task with no wrap around */
   voRotaryEncoderTask(u8NumHeaders, 0, 0);
   su8RotaryCounter = u8GetRotaryValue();
   
   /* Check if the rotary encoder is changed */
   if (su8RotaryCounter != su8PreVal)
   {
      /* UPDATE OLED */
      voOLEDClear();
      voOLEDHome();

      /* Row 0 */
      printf((char*)u8MenuHeadersArr[su8RotaryCounter]);
      voOLEDMoveCursor(0, 13);
      voOLEDWrite(0b01111111, DATA); /* Left arrow */

      if (su8RotaryCounter > 0)
      {
         voOLEDMoveCursor(0, 15);
         voOLEDWrite(0b10110011, DATA);   /* Up arrow */
      }

      /* Row 1 */

      if (su8RotaryCounter < u8NumHeaders)
      {
         voOLEDRowTwo();
         printf((char*)u8MenuHeadersArr[su8RotaryCounter + 1]);
         voOLEDMoveCursor(1, 15);
         voOLEDWrite(0b10110100, DATA);   /* Down arrow */
      }

      su8PreVal = su8RotaryCounter;
   }

   if (enGetRotaryButton() == PRESSED)
   {
      su8PreVal = 150;
      voResetRotaryValue();
      return su8RotaryCounter;
   }

   return 150;
   
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuScheduleMenu
 * 
 * DESCRIPTION    : Prints/Handling the schedule menu
 * 
 * INPUT          : 8-bit number (number of items in schedule)
 * 
 * OUTPUT         : 8-bit number (choosen header).
 * 
 * NOTE           : Returns 150 when the button is not pressed
 * 
******************************************************************************** 
*/
u8 u8MenuScheduleMenu(u8 u8NumItems)
{
   static u8 su8RotaryCounter;
   static u8 su8PreVal = 150; /* Holds the value of previous value of rotary, 150 is default value*/
   
   /* Run the rotary task with no wrap around */
   voRotaryEncoderTask(u8NumItems, 0, 0);
   su8RotaryCounter = u8GetRotaryValue();
   
   /* Check if the rotary encoder is changed */
   if (su8RotaryCounter != su8PreVal)
   {
      /* UPDATE OLED */
      voOLEDClear();
      voOLEDHome();
      
      if (su8RotaryCounter < (u8NumItems))
      {
         /* Print the whole element */
         voSchedulePrintElement(su8RotaryCounter);
      }
      else if (su8RotaryCounter == (u8NumItems))
      {
         printf("BACK");
         voOLEDMoveCursor(0, 5);
         voOLEDWrite(0b01111111, DATA); /* Left arrow */
      }

      if (su8RotaryCounter > 0)
      {
         voOLEDMoveCursor(0, 15);
         voOLEDWrite(0b10110011, DATA);   /* Up arrow */
      }

      if (su8RotaryCounter < u8NumItems)
      {
         voOLEDMoveCursor(1, 15);
         voOLEDWrite(0b10110100, DATA);   /* Down arrow */
      }

      su8PreVal = su8RotaryCounter;
   }

   if (enGetRotaryButton() == PRESSED)
   {
      su8PreVal = 150;
      voResetRotaryValue();
      return su8RotaryCounter;
   }

   return 150;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuConfigureTime
 * 
 * DESCRIPTION    : The user makes time settings using the rotary encoder
 * 
 * INPUT          : 8-bit pointer to array of size 7
 * 
 * OUTPUT         : Returns 1 when done, else 0
 * 
 * NOTE           : This function will change the array sent as parameter
 * 
******************************************************************************** 
*/
u8 u8MenuConfigureTime(u8* u8DateTimeArr)
{  
   /* 
    * Contains settings to be sent as parameters to different task handlers. 
    * {MaxCount, MinCount, RowNumber for printing, ColumnNumber}
    */
   static const u8 scu8SettingsArr[7][4] = {{99, 0, 0, 2}, {12, 1, 0, 5}, {31, 1, 0, 8}, 
                                    {6, 0, 0, 11}, {23, 0, 1, 0}, {59, 0, 1, 3}, 
                                    {59, 0, 1, 6}};
   
   static u8 su8Counter = 0;     /* Counts the number of laps the function has been run */
   static u8 su8RotaryCounter;   /* Holds the current value of the rotary */   
   static u8 su8RotPreVal = 150; /* Holds the value of previous value of rotary, 150 is default value*/
   
   u8 u8StringArr[5];            /* The string to print, maximum 4 characters */
   
   static LOCK_STATUS enLockStatus = UNLOCKED;
   
   static TICK_STATUS senTick;   /* Holds the current value of TICK */
   static TICK_STATUS senPreTick;/* Holds the value of previous value of rotary,  is default value */
   
   /* This code shall only run the first time this function is called */
   if (enLockStatus == UNLOCKED)
   {
      /* Set this  */
      voOLEDClear();
      voOLEDHome();
      voRTCPrintDateTime(u8DateTimeArr);
      voSetRotaryValue(u8DateTimeArr[0]); /* Make the OLED print the current value from the start */
      enLockStatus = LOCKED;  /* Lock to make this code just run once */
   }
   
   /* Run voRotaryEncoderTask with unique settings for each time parameter */
   voRotaryEncoderTask(scu8SettingsArr[su8Counter][0], scu8SettingsArr[su8Counter][1], 1);
   su8RotaryCounter = u8GetRotaryValue(); /* Get the value from module */ 
   
   /* Run the tick task and get the value */
   voTickTask(25);
   senTick = enGetTick();
   
   /* Check if the rotary encoder has been turned or if the tick has changed */
   if (su8RotaryCounter != su8RotPreVal || senTick != senPreTick)
   {
      voOLEDMoveCursor(scu8SettingsArr[su8Counter][2], scu8SettingsArr[su8Counter][3]);
      
      memset(u8StringArr, '\0', 5);    /* Reset the output string */
      if (senTick == ON)
      {
         if (su8Counter != 3)
         {
            if (su8RotaryCounter < 10)
            {
               sprintf((char*)u8StringArr, "0%u", su8RotaryCounter);
            }
            else
            {
               sprintf((char*)u8StringArr, "%u", su8RotaryCounter);
            }
         }
         else if (su8Counter == 3)
         {
            sprintf((char*)u8StringArr, "%s", mscu8DayArr[su8RotaryCounter]);
         }
      }
      else if (senTick == OFF)
      {
         if (su8Counter != 3)
         {
            sprintf((char*)u8StringArr, "  ");
         }
         else if (su8Counter == 3)
         {
            sprintf((char*)u8StringArr, "    ");
         }
      }
      printf("%s", u8StringArr);
      su8RotPreVal = su8RotaryCounter;
      senPreTick = senTick;
   }
   
   if (enGetRotaryButton() == PRESSED)
   {
      /* This code should not be needed here, fix if there's time */
      if (senTick == OFF)
      {
         voOLEDMoveCursor(scu8SettingsArr[su8Counter][2], scu8SettingsArr[su8Counter][3]);

         memset(u8StringArr, '\0', 5);    /* Reset the output string */
         
         if (su8Counter != 3)
         {
            if (su8RotaryCounter < 10)
            {
               sprintf((char*)u8StringArr, "0%u", su8RotaryCounter);
            }
            else
            {
               sprintf((char*)u8StringArr, "%u", su8RotaryCounter);
            }
         }
         else if (su8Counter == 3)
         {
            sprintf((char*)u8StringArr, "%s", mscu8DayArr[su8RotaryCounter]);
         }
         
         printf("%s", u8StringArr);
      }
      
      /* Save the choosen value in the array */
      if (su8Counter != 3)
      {
         u8DateTimeArr[su8Counter] = su8RotaryCounter;
      }
      else if(su8Counter == 3)
      {
         u8DateTimeArr[su8Counter] = su8RotaryCounter + 1;
      }
      
      /* Reset variables and prepare for next call */
      su8Counter++;
      su8RotPreVal = 150;
      
      if (su8Counter < 7)
      {
         /* Make the OLED print the correct value when next round starts */
         if (su8Counter != 3)
         {
            voSetRotaryValue(u8DateTimeArr[su8Counter]);
         }
         else if (su8Counter == 3)
         {
            voSetRotaryValue(u8DateTimeArr[su8Counter] - 1);
         }
      }
      else if (su8Counter >= 7) /* If last lap */
      {
         su8Counter = 0;
         senPreTick = OFF;
         voTickReset();
         voResetRotaryValue();
         enLockStatus = UNLOCKED;   /* Unlock the variable again for next session */
         return 1;
      }
   }
   
   /* Not done yet */
   return 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuConfigurePot
 * 
 * DESCRIPTION    : The user makes choose pot number using the rotary encoder
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number, the pot number
 * 
 * NOTE           : Returns 150 when not done
 * 
******************************************************************************** 
*/
u8 u8MenuConfigurePot(vo)
{
   static u8 su8Pot = 0;
   
   static u8 su8Counter = 0;
   static u8 su8RotaryCounter;  
   static u8 su8RotPreVal = 150;
   
   static TICK_STATUS senTick;
   static TICK_STATUS senPreTick;
   
   static LOCK_STATUS enLockStatus = UNLOCKED;
   
   if (enLockStatus == UNLOCKED)
   {
      /* Set this  */
      voOLEDClear();
      voOLEDHome();
      printf("Pot nr: ");
      voSetRotaryValue(1);
      enLockStatus = LOCKED;  /* Lock to make this code just run once */
   }
   
   /* Run voRotaryEncoderTask with max 5 pots */
   voRotaryEncoderTask(5, 1, 0);
   su8RotaryCounter = u8GetRotaryValue(); 
   
   /* Run the tick task and get the value */
   voTickTask(25);
   senTick = enGetTick();
  
   if (su8RotaryCounter != su8RotPreVal || senTick != senPreTick)
   {
      voOLEDMoveCursor(0, 8);
      if (senTick == ON)
      {
         printf("%u", su8RotaryCounter);
      }
      else if (senTick == OFF)
      {
         printf(" ");
      }
      su8RotPreVal = su8RotaryCounter;
      senPreTick = senTick;
   }
   
   if (enGetRotaryButton() == PRESSED)
   {
      su8Pot = su8RotaryCounter;
      su8RotPreVal = 150;
      su8Counter = 0;
      senPreTick = OFF;
      voTickReset();
      voResetRotaryValue();
      enLockStatus = UNLOCKED;
      
      return su8Pot;
   }
   return 150;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuConfigureWaterAmount
 * 
 * DESCRIPTION    : The user may choose duration of watering using the rotary 
 *                  encoder
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number, the amount of water in dl * 0,5
 * 
 * NOTE           : Returns 150 when not done
 * 
******************************************************************************** 
*/
u8 u8MenuConfigureWaterAmount(vo)
{
   static u8 su8Amount = 0;
   
   static u8 su8Counter = 0;
   static u8 su8RotaryCounter;  
   static u8 su8RotPreVal = 150;
   
   static TICK_STATUS senTick;
   static TICK_STATUS senPreTick;
   
   static LOCK_STATUS enLockStatus = UNLOCKED;
   
   if (enLockStatus == UNLOCKED)
   {
      /* Set this  */
      voOLEDClear();
      voOLEDHome();
      printf("Amount of water:");
      voSetRotaryValue(2);
      enLockStatus = LOCKED;  /* Lock to make this code just run once */
   }
   
   /* Run voRotaryEncoderTask with maxcount = 16 */
   voRotaryEncoderTask(16, 2, 0);
   su8RotaryCounter = u8GetRotaryValue(); 
   
   /* Run the tick task and get the value */
   voTickTask(25);
   senTick = enGetTick();
  
   if (su8RotaryCounter != su8RotPreVal || senTick != senPreTick)
   {
      voOLEDRowTwo();
      if (senTick == ON)
      {
         printf("ca %u.%u dl", (su8RotaryCounter / 2), (su8RotaryCounter % 2) * 5);
      }
      else if (senTick == OFF)
      {
         printf("         ");
      }
      su8RotPreVal = su8RotaryCounter;
      senPreTick = senTick;
   }
   
   if (enGetRotaryButton() == PRESSED)
   {
      su8Amount = su8RotaryCounter;
      su8RotPreVal = 150;
      su8Counter = 0;
      senPreTick = OFF;
      voTickReset();
      voResetRotaryValue();
      enLockStatus = UNLOCKED;
      
      return su8Amount;
   }
   return 150;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuConfigureRepeat
 * 
 * DESCRIPTION    : The user may choose if the watering is to be repeated
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number, the repeat type of the watering
 * 
 * NOTE           : Returns 150 when not done
 * 
******************************************************************************** 
*/
u8 u8MenuConfigureRepeat(vo)
{
   static u8 su8Repeat = 0;
   
   static u8 su8Counter = 0;
   static u8 su8RotaryCounter;  
   static u8 su8RotPreVal = 150;
   
   static TICK_STATUS senTick;
   static TICK_STATUS senPreTick;
   
   static LOCK_STATUS enLockStatus = UNLOCKED;
   
   if (enLockStatus == UNLOCKED)
   {
      /* Set this  */
      voOLEDClear();
      voOLEDHome();
      printf("Repeat watering?");
      voResetRotaryValue();
      enLockStatus = LOCKED;  /* Lock to make this code just run once */
   }
   
   /* Run voRotaryEncoderTask with maxcount = 3 */
   voRotaryEncoderTask(3, 0, 0);
   su8RotaryCounter = u8GetRotaryValue(); 
   
   /* Run the tick task and get the value */
   voTickTask(25);
   senTick = enGetTick();
  
   if (su8RotaryCounter != su8RotPreVal || senTick != senPreTick)
   {
      voOLEDRowTwo();
      if (senTick == ON)
      {
         printf("%s", mscu8RepeatStrArr[su8RotaryCounter]);
      }
      else if (senTick == OFF)
      {
         printf("                ");
      }
      su8RotPreVal = su8RotaryCounter;
      senPreTick = senTick;
   }
   
   if (enGetRotaryButton() == PRESSED)
   {
      su8Repeat = su8RotaryCounter;
      su8RotPreVal = 150;
      su8Counter = 0;
      senPreTick = OFF;
      voTickReset();
      voResetRotaryValue();
      enLockStatus = UNLOCKED;
      
      return su8Repeat;
   }
   return 150;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuAddNewToSchedule
 * 
 * DESCRIPTION    : The user adds a new watering event
 * 
 * INPUT          : -
 * 
 * OUTPUT         : Returns 0 when not done, 1 when done and OK, 2 when done but fail
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8MenuAddNewToSchedule(vo)
{
   typedef enum
   {
      CHOOSE_POT = 0,
      CHOOSE_DURATION,
      CHOOSE_REPEAT,
      CHOOSE_TIME
   }ADD_EVENT_TYPE;

   static ADD_EVENT_TYPE senStateAddEvent = CHOOSE_POT;
   static u8 su8Pot = 150;
   static u8 su8Duration = 150;
   static u8 su8Repeat = 150;
   static u8 su8SchedulingTime[7];
   
   switch(senStateAddEvent)
   {
      case CHOOSE_POT:
         su8Pot = u8MenuConfigurePot();

         if (su8Pot != 150) /* Returns 150 when not done */
         {
            senStateAddEvent = CHOOSE_DURATION;
         }
      break;
      
      case CHOOSE_DURATION:
         su8Duration = u8MenuConfigureWaterAmount();
         
         if (su8Duration != 150)
         {
            senStateAddEvent = CHOOSE_REPEAT;
         }
      break;
      
      case CHOOSE_REPEAT:
         su8Repeat = u8MenuConfigureRepeat();
         
         if (su8Repeat != 150)
         {
            memset(su8SchedulingTime, '\0', sizeof(su8SchedulingTime));
            voRTCGetDateTime(su8SchedulingTime);
            senStateAddEvent = CHOOSE_TIME;
         }
         
      break;

      case CHOOSE_TIME:
         if (u8MenuConfigureTime(su8SchedulingTime))
         {
            senStateAddEvent = CHOOSE_POT;
            
            if (u8SchedulePushBack(su8Pot, su8Duration, su8Repeat, su8SchedulingTime))
            {
               /* Added event ok */
               return 1;
            }
            else
            {
               /* Add failed */
               return 2;
            }
         }
      break;
   }
   return 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuChangeInSchedule
 * 
 * DESCRIPTION    : The user makes changes in a previous set watering
 * 
 * INPUT          : 8-bit number (event to change)
 * 
 * OUTPUT         : Returns 1 when done, else 0
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8MenuChangeInSchedule(u8 u8EventNum)
{
   typedef enum
   {
      CHOOSE_POT = 0,
      CHOOSE_DURATION,
      CHOOSE_REPEAT,
      CHOOSE_TIME
   }CHANGE_EVENT_TYPE;

   static CHANGE_EVENT_TYPE senStateChangeEvent = CHOOSE_POT;
   static u8 su8Pot = 150;
   static u8 su8Duration = 150;
   static u8 su8Repeat = 150;
   static u8 su8SchedulingTime[7];
   
   switch(senStateChangeEvent)
   {
      case CHOOSE_POT:
         su8Pot = u8MenuConfigurePot();

         if (su8Pot != 150) /* Returns 150 when not done */
         {
            senStateChangeEvent = CHOOSE_DURATION;
         }
      break;
      
      case CHOOSE_DURATION:
         su8Duration = u8MenuConfigureWaterAmount();
         
         if (su8Duration != 150)
         {
            senStateChangeEvent = CHOOSE_REPEAT;
         }
      break;
      
      case CHOOSE_REPEAT:
         su8Repeat = u8MenuConfigureRepeat();
         
         if (su8Repeat != 150)
         {
            memset(su8SchedulingTime, '\0', sizeof(su8SchedulingTime));
            voScheduleGetElementTime(u8EventNum, su8SchedulingTime);
            senStateChangeEvent = CHOOSE_TIME;
         }
      break;

      case CHOOSE_TIME:
         if (u8MenuConfigureTime(su8SchedulingTime))
         {  
            senStateChangeEvent = CHOOSE_POT;
            
            if (u8ScheduleChangeAny(u8EventNum, su8Pot, su8Duration, su8Repeat, su8SchedulingTime))
            {
               return 1;   /* OK */
            }
            else
            {
               return 2;   /* FAIL */
            }
         }
      break;
   }
   return 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuSchedulerTask
 * 
 * DESCRIPTION    : The user may browse through the schedule and make changes
 * 
 * INPUT          : enum (the type of event that the user has in mind (change, add, etc)).
 * 
 * OUTPUT         : 8-bit number (1 if done, else 0)
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8MenuSchedulerTask(EVENT_TYPE enEvent)
{
   typedef enum
   {
      CHOOSE_EVENT_NO = 0,
      ADD_NEW_TO_SCHEDULE,
      CHANGE_SCHEDULE,
      REMOVE_FROM_SCHEDULE,
      PRINT_MESSAGE,
      SHOW_MESSAGE
   }SCHEDULER_TYPE;
   
   static u8 su8ChoosenEvent = 150;
   static SCHEDULER_TYPE senState = CHOOSE_EVENT_NO;
   static u8 su8StringArr[16];
   
   static u8 su8ReturnVal;
   
   u8 u8ScheduleSize = u8ScheduleGetSize();
   
   switch (senState)
   {
      case CHOOSE_EVENT_NO:
         
         /* If user is in add-new-to-schedule-mode */
         if (enEvent == ADD)
         {
            senState = ADD_NEW_TO_SCHEDULE;
         }
         
         else if (u8ScheduleSize > 0)
         {
            /* Run the schedule menu */
            su8ChoosenEvent = u8MenuScheduleMenu(u8ScheduleSize);

            /* If user has pressed the button */
            if (su8ChoosenEvent != 150)
            {
               /* Check first if user has choosen "BACK" because it's a common feature of several paths in the state machine */
               if (su8ChoosenEvent == u8ScheduleSize)
               {
                  voOLEDClear();
                  voOLEDHome();
                  return 1;
               }
               
               /* If user is in change-schedule-mode */
               else if (enEvent == CHANGE)
               {
                  senState = CHANGE_SCHEDULE;
               }
               
               /* If user is in remove_from-schedule-mode */
               else if (enEvent == REMOVE)
               {
                  senState = REMOVE_FROM_SCHEDULE;
               }
            }  
         }
         else if (u8ScheduleSize == 0)
         {
            memset(su8StringArr, '\0', sizeof(su8StringArr));
            sprintf((char*)su8StringArr, "%s", "Schedule empty!");
            senState = PRINT_MESSAGE;
         }
      break;
      
      case ADD_NEW_TO_SCHEDULE:
         su8ReturnVal = u8MenuAddNewToSchedule();
         
         if (su8ReturnVal == 1)
         {
            memset(su8StringArr, '\0', sizeof(su8StringArr));
            sprintf((char*)su8StringArr, "%s", "Event added!");
            senState = PRINT_MESSAGE;
         }
         else if (su8ReturnVal == 2)
         {
            memset(su8StringArr, '\0', sizeof(su8StringArr));
            sprintf((char*)su8StringArr, "%s", "Adding failed!");
            senState = PRINT_MESSAGE;
         }
      break;
      
      case CHANGE_SCHEDULE:
         su8ReturnVal = u8MenuChangeInSchedule(su8ChoosenEvent);
         
         if (su8ReturnVal == 1)
         {
            memset(su8StringArr, '\0', sizeof(su8StringArr));
            sprintf((char*)su8StringArr, "%s", "Event changed!");
            senState = PRINT_MESSAGE;
         }
         else if (su8ReturnVal == 2)
         {
            memset(su8StringArr, '\0', sizeof(su8StringArr));
            sprintf((char*)su8StringArr, "%s", "Changing failed!");
            senState = PRINT_MESSAGE;
         }
      break;
      
      case REMOVE_FROM_SCHEDULE:
         voScheduleRemoveAny(su8ChoosenEvent);
         memset(su8StringArr, '\0', sizeof(su8StringArr));
         sprintf((char*)su8StringArr, "%s", "Event removed!");
         senState = PRINT_MESSAGE;
      break;
      
      case PRINT_MESSAGE:
         voOLEDClear();
         voOLEDHome();
         printf("%s", su8StringArr);
         senState = SHOW_MESSAGE;
      break;
      
      case SHOW_MESSAGE:
         if (u8OLEDBlinkScreen(5, 25))
         {
            senState = CHOOSE_EVENT_NO;
            return 1;
         }
      break;
      
      default:
      break;
   }
   
   return 0;
   
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuConfigureMotor
 * 
 * DESCRIPTION    : Prints/handling motor settings when steering the motor 
 *                  manually.
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number (1 = done, 150 = not done)
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8MenuConfigureMotor(vo)
{  
   static u8 su8Counter = 0;     /* Counts the number of laps the function has been run */
   static u8 su8RotaryCounter;   /* Holds the current value of the rotary */   
   static u8 su8RotPreVal = 150; /* Holds the value of previous value of rotary, 150 is default value*/
   static u8 su8Direction = 0;
   static u8 su8MotorSpeed = 5;  /* Set to 5 by default */
   
   static const u8 scu8SettingsArr[2][2] = {{1, 0}, {10, 1}};
   
   static LOCK_STATUS enLockStatus = UNLOCKED;
   
   static TICK_STATUS senTick;   /* Holds the current value of TICK */
   static TICK_STATUS senPreTick;/* Holds the value of previous value of rotary,  is default value */
   
   /* This code shall only run the first time this function is called */
   if (enLockStatus == UNLOCKED)
   {
      /* Set this  */
      voOLEDClear();
      voOLEDHome();
      printf("%s", mscu8DirectionArr[su8Direction]);
      voOLEDRowTwo();
      printf("Speed: %u", su8MotorSpeed);
      enLockStatus = LOCKED;  /* Lock to make this code just run once */
   }
   
   /* Run voRotaryEncoderTask with unique settings for each time parameter */
   voRotaryEncoderTask(scu8SettingsArr[su8Counter][0], scu8SettingsArr[su8Counter][1], 0);
   su8RotaryCounter = u8GetRotaryValue(); /* Get the value from module */ 
   
   /* Run the tick task and get the value */
   voTickTask(25);
   senTick = enGetTick();
   
   /* Check if the rotary encoder has been turned or if the tick has changed */
   if (su8RotaryCounter != su8RotPreVal || senTick != senPreTick)
   {
      if (su8Counter == 0)
      {
         su8Direction = su8RotaryCounter;
         voOLEDHome();
         if (senTick == ON)
         {
            printf("%s", mscu8DirectionArr[su8RotaryCounter]);
         }
         else if (senTick == OFF)
         {
            printf("        ");
         }
      }
      else if (su8Counter == 1)
      {
         su8MotorSpeed = su8RotaryCounter;
         voOLEDRowTwo();
         if (senTick == ON)
         {
            printf("Speed: %u", su8MotorSpeed);
            
            if (su8MotorSpeed < 10)
            {
               voOLEDMoveCursor(1, 8);
               printf(" ");
            }
         }
         else if (senTick == OFF)
         {
            printf("         ");
         }
      }
      
      su8RotPreVal = su8RotaryCounter;
      senPreTick = senTick;
   }
   
   if (enGetRotaryButton() == PRESSED)
   {
      if (su8Counter == 0)
      {
         voSetRotaryValue(su8MotorSpeed);
      
         if (senTick == OFF)
         {
            voOLEDHome();
            printf("%s", mscu8DirectionArr[su8Direction]);
         }
      }
      
      /* Reset variables and prepare for next call */
      su8Counter++;
      su8RotPreVal = 150;
      
      if (su8Counter > 1 ) /* If last lap */
      {
         su8Counter = 0;
         senPreTick = OFF;
         voTickReset();
         voResetRotaryValue();
         enLockStatus = UNLOCKED;   /* Unlock the variable again for next session */
         
         if (su8Direction == 0)
         {
            voMotorForward();
         }
         else if (su8Direction == 1)
         {
            voMotorBackward();
         }
         
         voMotorSetSpeedByLevel(su8MotorSpeed);
         
         return 1;
      }
   }
   return 150;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuStopMotor
 * 
 * DESCRIPTION    : Lets the user manually stop the motor
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number (1 = done, 150 = not done)
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8MenuStopMotor(vo)
{
   static TICK_STATUS senTick;
   static TICK_STATUS senPreTick;
   
   /* Run the tick task and get the value */
   voTickTask(25);
   senTick = enGetTick();
  
   if (senTick != senPreTick)
   {
      voOLEDHome();
      if (senTick == ON)
      {
         printf("STOP");
      }
      else if (senTick == OFF)
      {
         printf("    ");
      }
      senPreTick = senTick;
   }
   
   if (enGetRotaryButton() == PRESSED)
   {
      senPreTick = OFF;
      voTickReset();
      return 1;
   }
   return 150;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8MenuMovePump
 * 
 * DESCRIPTION    : The menu for the the manual motor control.
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number (1 = done, 0 = not done)
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8MenuMovePump(vo)
{
   typedef enum
   {
      CONFIGURE_MOTOR = 0,
      PUMP_RUNNING,
      PUMP_STOPPED,
      CHECK_SENSOR,
      BLINK_SCREEN
   }MOVE_PUMP_TYPE;
   
   u8 u8MenuCD[2][13] = {"CONTINUE", "DONE"}; /* CD = Continue/Done */
   
   static MOVE_PUMP_TYPE senState = CONFIGURE_MOTOR;
   static u8 su8MotorConfigDone;
   static u8 su8MotorStop;
   static u8 su8UserDone;
   
   switch (senState)
   {
      case CONFIGURE_MOTOR:
         su8MotorConfigDone = u8MenuConfigureMotor();
         if (su8MotorConfigDone != 150) /* Returns 150 when not done */
         {
            voOLEDClear();
            senState = PUMP_RUNNING;
         }
      break;
      
      case PUMP_RUNNING:
         su8MotorStop = u8MenuStopMotor();
         if (su8MotorStop != 150) /* Returns 150 when not done */
         {
            voMotorStop();
            voMotorSetSpeed(0);
            senState = PUMP_STOPPED;
         }
      break;
      
      case PUMP_STOPPED:
         su8UserDone = u8MenuSingleLine(1, u8MenuCD);
         
         if (su8UserDone == 0)
         {
            senState = CONFIGURE_MOTOR;
         }
         else if (su8UserDone == 1)
         {
            senState = CHECK_SENSOR;
         }
         
      break;
      
      case CHECK_SENSOR:
         if (u16ADCRead10Bit() >= 600)
         {
            senState = CONFIGURE_MOTOR;
            return 1;
         }
         else
         {
            voOLEDClear();
            voOLEDHome();
            printf("Move the sensor");
            voOLEDRowTwo();
            printf("in front of pot.");
            senState = BLINK_SCREEN;
         }
      break;
      
      case BLINK_SCREEN:
         if (u8OLEDBlinkScreen(5, 40) != 0)
         {
            senState = CONFIGURE_MOTOR;
         }
      break;
      
      default:
      break;
   }
   return 0;
}