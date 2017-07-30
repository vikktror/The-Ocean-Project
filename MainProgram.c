#include <xc.h>
#include <stdio.h>
#include <string.h>

#include "MainProgram.h"
#include "MenuDriver.h"
#include "OLED.h"
#include "PumpNavigation.h"
#include "RotaryEncoder.h"
#include "RTC.h"
#include "ScheduleDriver.h"
#include "Tick.h"

#define MENU1_ARR_ITEMS    (4)
#define MENU1_ITEMS        (MENU1_ARR_ITEMS - 1)

#define SCHEDULE_ARR_ITEMS (5)
#define SCHEDULE_ITEMS     (SCHEDULE_ARR_ITEMS - 1)

/* Enums for state machines */
typedef enum
{
   CHECK_BUTTON = 0,
   MENU1,
   SCHEDULE,
   SET_CLOCK,
   MOVE_PUMP
}MAIN_TYPE;

typedef enum
{
   SCHEDULE_MENU = 0,
   SCHEDULER
}SCHEDULE_TYPE;

typedef enum
{
   SET_TIME = 0,
   BLINK_TO_CONFIRM
}CLOCK_TYPE;

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voMainProgramTask
 * 
 * DESCRIPTION    : The main program task
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Variables declared in the beginning is not necessary used 
 *                  for the first time in the case specified by the comment.
 * 
******************************************************************************** 
*/
vo voMainProgramTask(vo)
{
   static MAIN_TYPE senState = CHECK_BUTTON;
   u8 u8ChoosenHeader;
   
   /* MENU1 ASSOCIATED */
   u8 u8Menu1 [MENU1_ARR_ITEMS][13] = {"SCHEDULE", "SET CLOCK", "MOVE PUMP", "DONE"};
   
   /* SCHEDULE ASSOCIATED */
   u8 u8ScheduleMenu [SCHEDULE_ARR_ITEMS][13] = {"SEE SCHEDULE", "ADD EVENT", "CHANGE EVENT", "REMOVE EVENT", "BACK"};
   static SCHEDULE_TYPE senStateSchedule = SCHEDULE_MENU;
   static EVENT_TYPE enEvent;
   
   /* SET_CLOCK ASSOCIATED */
   static CLOCK_TYPE senStateClock = SET_TIME;
   static u8 su8DateTimeArr[7];
   
   
   // <editor-fold defaultstate="collapsed" desc="switch(senState)">
   switch(senState)
   {
      // <editor-fold defaultstate="collapsed" desc="case CHECK_BUTTON:">
      case CHECK_BUTTON:
         if (enGetRotaryButton() == PRESSED)
         {
            voRTCPrintDisable(MENU);
            voResetRotaryValue();
            senState = MENU1;
         }
      break;
      // </editor-fold>
      
      // <editor-fold defaultstate="collapsed" desc="case MENU1:">
      case MENU1:
         /* Run the single line menu task */
         u8ChoosenHeader = u8MenuSingleLine(MENU1_ITEMS, u8Menu1);
         
         /* SCHEDULE */
         if (u8ChoosenHeader == 0)
         {
            voOLEDClear();
            voOLEDHome();
            senState = SCHEDULE;
         }
         
         /* SET CLOCK */
         else if (u8ChoosenHeader == 1)
         {
            memset(su8DateTimeArr, '\0', sizeof(su8DateTimeArr));
            voRTCGetDateTime(su8DateTimeArr);
            senState = SET_CLOCK;
         }

         /* MOVE PUMP */
         else if (u8ChoosenHeader == 2)
         {
            voOLEDClear();
            voOLEDHome();
            senState = MOVE_PUMP;
         }
         /* DONE */
         else if (u8ChoosenHeader == 3)
         {
            voOLEDClear();
            voOLEDHome();
            voRTCPrintEnable(MENU);
            senState = CHECK_BUTTON;
         }
      break;
      // </editor-fold>
      
      // <editor-fold defaultstate="collapsed" desc="case SCHEDULE:">
      case SCHEDULE:
         switch(senStateSchedule)
         {
            case SCHEDULE_MENU:
               /* Run the single line menu task */
               u8ChoosenHeader = u8MenuSingleLine(SCHEDULE_ITEMS, u8ScheduleMenu);
               
               /* SEE SCHEDULE */
               if (u8ChoosenHeader == 0)
               {
                  enEvent = VIEW;
                  senStateSchedule = SCHEDULER;
               }
               
               /* ADD EVENT */
               else if (u8ChoosenHeader == 1)
               {  
                  enEvent = ADD;
                  senStateSchedule = SCHEDULER;
               }
               
               /* CHANGE EVENT */
               else if (u8ChoosenHeader == 2)
               {
                  enEvent = CHANGE;
                  senStateSchedule = SCHEDULER;
               }
               
               /* REMOVE EVENT */
               else if (u8ChoosenHeader == 3)
               {
                  enEvent = REMOVE;
                  senStateSchedule = SCHEDULER;
               }
               
               /* BACK */
               else if (u8ChoosenHeader == 4)
               {
                  voOLEDClear();
                  voOLEDHome();
                  senState = MENU1;
               }
            break;
            
            case SCHEDULER:
               if (u8MenuSchedulerTask(enEvent) == 1)
               {
                  senStateSchedule = SCHEDULE_MENU;
                  senState = SCHEDULE;
               }
            break;
            
            default:
            break;
         }
      break;
      // </editor-fold>
      
      // <editor-fold defaultstate="collapsed" desc="case SET_CLOCK:">
      case SET_CLOCK:
         
         switch(senStateClock)
         {
            case SET_TIME:
               /* Let the user set the time */
               if (u8MenuConfigureTime(su8DateTimeArr))
               {
                  voRTCSetDateTime(su8DateTimeArr);
                  senStateClock = BLINK_TO_CONFIRM;
               }
            break;
            
            case BLINK_TO_CONFIRM:               
               if (u8OLEDBlinkScreen(4, 25))
               {
                  senStateClock = SET_TIME;
                  voRTCPrintEnable(MENU);
                  senState = CHECK_BUTTON;
               }
            break;
         }
      break;
      // </editor-fold>
      
      case MOVE_PUMP:
         if (u8MenuMovePump())
         {
            senState = MENU1;
         }
      break;
         
      default:
      break;
   }
   // </editor-fold>

}