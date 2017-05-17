/*
 * File:   OLED.c
 * Author: Viktor
 *
 * Created on den 02 march 2017, 21:37
 */
#include <string.h>

#include "OLED.h"

#define PMDATA      (PMDIN1)

u8 mu8OLEDDelay;

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8OLEDReadBusyFlag
 * 
 * DESCRIPTION    : Reads from the OLED to see if it's busy.
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number
 * 
 * NOTE           : -
******************************************************************************** 
*/
u8 u8OLEDReadBusyFlag(vo)
{
   u16 u16Dummy;
   while (PMMODEbits.BUSY);
   PMADDR = COMMAND;
   u16Dummy = PMDATA;
   while (PMMODEbits.BUSY);
   return PMDATA;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8OLEDReadSign
 * 
 * DESCRIPTION    : Reads a sign from OLED.
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number (ASCII)
 * 
 * NOTE           : -
******************************************************************************** 
*/
u8 u8OLEDReadSign(vo)
{
   u16 u16Dummy;
   while (PMMODEbits.BUSY);
   PMADDR = DATA;
   u16Dummy = PMDATA;
   while (PMMODEbits.BUSY);
   return PMDATA;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voOLEDReadScreen
 * 
 * DESCRIPTION    : Reads the entire screen
 * 
 * INPUT          : 8-bit pointer to array of size 32
 * 
 * OUTPUT         : Output in form of a changed input-array
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voOLEDReadScreen(u8* u8ScreenArr)
{ 
   u8 u8Row;
   u8 u8Column;
   u8 u8Counter = 0;
   
   for (u8Row = 0; u8Row < 2; u8Row++)
   {
      for (u8Column = 0; u8Column < 16; u8Column ++)
      {
         voOLEDMoveCursor(u8Row, u8Column);
         u8ScreenArr[u8Counter] = u8OLEDReadSign();
         u8Counter++;
      }
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voOLEDWrite
 * 
 * DESCRIPTION    : Writes data to OLED
 * 
 * INPUT          : 8-bit number, 16-bit number
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voOLEDWrite(u8 u8Character, 
               u16 u16Type)
{
   /* & 0x80 to bit mask the  MSb*/
   while ( u8OLEDReadBusyFlag() & 0x80 );
   while (PMMODEbits.BUSY);
   PMADDR = u16Type;
   PMDATA = u8Character;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voPutsOLED
 * 
 * DESCRIPTION    : Loops through a text string and calls write function
 * 
 * INPUT          : 8-bit pointer
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : Preferably call printf() instead.
******************************************************************************** 
*/
vo voOLEDPuts(u8 *u8StringPtr)
{
   while (*u8StringPtr)
   {
      voOLEDWrite(*u8StringPtr++, DATA);
   }  
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voOLEDHome
 * 
 * DESCRIPTION    : Moves the cursor to pos 0, row 1
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voOLEDHome(vo)
{
   voOLEDWrite(128, COMMAND); /* Set DDRAM address = 0 */
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voOLEDRowTwo
 * 
 * DESCRIPTION    : Moves the cursor to pos 0, row 1
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : In this application rows are numbered 0 and 1.
******************************************************************************** 
*/
vo voOLEDRowTwo(vo)
{
   voOLEDWrite(128+64, COMMAND); /* Set DDRAM address to 64 as it corresponds row 1 */
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voOLEDMoveCursor
 * 
 * DESCRIPTION    : Moves the cursor to an arbitrary position
 * 
 * INPUT          : 2x 8-bit numbers
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voOLEDMoveCursor(u8 u8Row, 
                    u8 u8Column)
{
   if (u8Row == 1)
   {
      u8Row = 64;
   }
   voOLEDWrite(128 + u8Row + u8Column, COMMAND);
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voOLEDClear
 * 
 * DESCRIPTION    : Clears the display from all previous symbols
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voOLEDClear(vo)
{
   voOLEDWrite(1, COMMAND);
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voOLEDPrint32BArr
 * 
 * DESCRIPTION    : Prints a 32 byte Array on first and second row, 16 chars
 *                  on each row.
 * 
 * INPUT          : 8-bit pointer to arr of size 32
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voOLEDPrint32BArr(u8* u8ScreenArr)
{
   u8 u8Row = 0;
   u8 u8Column = 0;
   u8 u8CharCounter = 0;
   
   for (u8Row = 0; u8Row < 2; u8Row++)
   {
      for (u8Column = 0; u8Column < 16; u8Column++)
      {
         voOLEDMoveCursor(u8Row, u8Column);
         voOLEDWrite(u8ScreenArr[u8CharCounter], DATA);
         u8CharCounter++;
      }
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8OLEDBlinkScreen
 * 
 * DESCRIPTION    : Reads the screen and blinks it <u8Cycles> times at 
 *                  (<u8BlinkSpeed> * 0,01)s  intervals
 * 
 * INPUT          : 2x 8-bit numbers
 * 
 * OUTPUT         : 8-bit number (1 when done, else 0) 
 * 
 * NOTE           : -
******************************************************************************** 
*/
u8 u8OLEDBlinkScreen(u8 u8Cycles, u8 u8BlinkSpeed)
{
   typedef enum
   {
      READ_OLED = 0,
      CLEAR_SCREEN,
      WAIT1_OR_DONE,
      PRINT_SCREEN,
      WAIT2
   }BLINK_TYPE;
   
   static BLINK_TYPE senState = READ_OLED;
   static u8 su8BlinkCounter = 0;
   static u8 su8ReadArr[32];
   
   switch (senState)
   {
      case READ_OLED:
         memset(su8ReadArr, '\0', sizeof(su8ReadArr));
         voOLEDReadScreen(su8ReadArr);
         senState = CLEAR_SCREEN;
      break;
         
      case CLEAR_SCREEN:
         voOLEDClear();
         su8BlinkCounter++;
         mu8OLEDDelay = u8BlinkSpeed;
         senState = WAIT1_OR_DONE;
      break;
         
      case WAIT1_OR_DONE:
         if (mu8OLEDDelay == 0)
         {
            if (su8BlinkCounter == u8Cycles)  /* DONE */
            {
               senState = READ_OLED;
               su8BlinkCounter = 0;
               return 1;
            }
            
            senState = PRINT_SCREEN;
         }
      break;
         
      case PRINT_SCREEN:
         voOLEDPrint32BArr(su8ReadArr);
//         for (u8Row = 0; u8Row < 2; u8Row++)
//         {
//            for (u8Column = 0; u8Column < 16; u8Column++)
//            {
//               voOLEDMoveCursor(u8Row, u8Column);
//               voOLEDWrite(su8ReadArr[u8CharCounter], DATA);
//               u8CharCounter++;
//            }
//         }
         mu8OLEDDelay = u8BlinkSpeed;
         senState = WAIT2;
      break;
         
      case WAIT2:
         if (mu8OLEDDelay == 0)
         {
            senState = CLEAR_SCREEN;
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
 * FUNCTION NAME  : voInitOLED
 * 
 * DESCRIPTION    : Initiates the OLED
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voOLEDInit(vo)
{
   /* Initiate PMP (Parallell Master Port) */
   PMCON = 0x83BF;
   PMMODE = 0x3FF;
   PMAEN = 0x0001;
   
   /* Wait at least 40ms before doing anything after power on  */
   mu8OLEDDelay = 4;
   while (mu8OLEDDelay > 0);
   
   /* 
    * Sets interface data length to 8-bit. Sets number of display lines to 2. 
    * Sets Character Font to 5x8 dots. Sets Font Table to Western European 1 
    */
   PMDATA = 0b00111001;
   mu8OLEDDelay = 1;
   while (mu8OLEDDelay > 0);
   
   /* Turn off display, no cursor, no blink */
   PMDATA = 0b00001000;
   mu8OLEDDelay = 1;
   while (mu8OLEDDelay > 0);
   
   /* Increment cursor, no display shift */
   PMDATA = 0b00000110;
   mu8OLEDDelay = 1;
   while (mu8OLEDDelay > 0);
   
   /* Turn on internal power */
   PMDATA = 0b00010111;
   mu8OLEDDelay = 1;
   while (mu8OLEDDelay > 0);
   
   /* Clear display */
   PMDATA = 0b00000001;
   mu8OLEDDelay = 7;
   while (mu8OLEDDelay > 0);
   
   /* Return home */
   PMDATA = 0b00000010;
   mu8OLEDDelay = 1;
   while (mu8OLEDDelay > 0);
   
   /* Display on, cursor off, blink off */
   PMDATA = 0b00001100;
   mu8OLEDDelay = 1;
   while (mu8OLEDDelay > 0);
   
   mu8OLEDDelay = 1;
   while (mu8OLEDDelay > 0);
   
   voOLEDHome();
}

