#include <xc.h>
#include "stdio.h"
#include "RTC.h"
#include "OLED.h"
#include "I2C.h"

u8 mu8RTCDelay;

/* 
 * Has to be 7x5 because "Mon." contains 4 chars, but printf() looks for a \n sign
 * to know when to stop writing. The fifth char will now automatically be \n 
 *  */
static const u8 mscu8DayArr[7][5] = {"Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat.", "Sun."};

static u8 msu8PrintEnableWatering = 1;
static u8 msu8PrintEnableMenu = 1;

typedef enum
{
   READ_DATE = 0,
   PRINT_DATE,
   WAIT
}RTC_TYPE;

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voRTCInit
 * 
 * DESCRIPTION    : Initiates the RTC
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
********************************************************************************
*/
vo voRTCInit(vo)
{
   u8 u8DateTimeArr[7] = {0, 1, 1, 1, 0, 0, 0};
   voRTCSetDateTime(u8DateTimeArr);
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8ByteToBCD
 * 
 * DESCRIPTION    : Converts an 8-bit value into packed BCD code
 * 
 * INPUT          : 8-bit number
 * 
 * OUTPUT         : 8-bit BCD code
 * 
 * NOTE           : Only works for input numbers < 100 (wouldn't be BCD else)
******************************************************************************** 
*/
u8 u8ByteToBCD(u8 u8Byte)
{
   u8 u8BCD = 0;
   u8 u8TensVal = 0;
   u8 u8OnesVal = 0;
   
   while (u8Byte >= 10)
   {
      u8TensVal++;
      u8Byte = u8Byte - 10;
   }
   
   u8OnesVal = u8Byte;
   
   u8BCD = (u8TensVal << 4) | u8OnesVal;
   
   return u8BCD;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voRTCSetDate
 * 
 * DESCRIPTION    : Writes date and time to RTC
 * 
 * INPUT          : 8-bit pointer to array of size 7
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voRTCSetDateTime(u8* u8DateTimeArr)
{
   s8 s8Index;
   
   for (s8Index = 0; s8Index < 7; s8Index++)
   {
      u8DateTimeArr[s8Index] = u8ByteToBCD(u8DateTimeArr[s8Index]);
   }
   
   voI2CStart();      
   voI2CTx(0b11010000);
   voI2CTx(0);
   
   /* Transmit the values to RTC */
   for (s8Index = 6; s8Index >= 0; s8Index--)
   {
      voI2CTx(u8DateTimeArr[s8Index]);
   }
  
   voI2CStop();
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voRTCGetDate
 * 
 * DESCRIPTION    : Get current date and time from the RTC
 * 
 * INPUT          : 8-bit pointer
 * 
 * OUTPUT         : Declared as void, but the input poniter is linked to msu8RTCArr 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voRTCGetDateTime(u8* u8DateTimePtr)
{
   s8 s8Index;
   
   /* Read the time and date from RTC */
   voI2CStart();
   voI2CTx(0b11010000);    /* Send slave adress + 0 for write */
   voI2CTx(0x00);          /* Reset adress pointer in slave to point on seconds */
   voI2CRepeatedStart();
   voI2CTx(0b11010001);    /* Slave adress + 1 for read */
   
   /* Reading the registers and puts in array */
   for (s8Index = 6; s8Index >= 0; s8Index--)
   {
      u8DateTimePtr[s8Index] = u8I2CRx();
      
      if (s8Index == 0)
      {
         continue;
      }
      voI2CACK();
   }
   
   voI2CNACK();
   voI2CStop();

   /* Link the numbers to the pointer */
   for (s8Index = 0; s8Index < 7; s8Index++)
   {
      u8DateTimePtr[s8Index] = (((u8DateTimePtr[s8Index] >> 4) & 0x0F) * 10) + (u8DateTimePtr[s8Index] & 0x0F);
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voRTCPrintDateTime
 * 
 * DESCRIPTION    : Print the date in correct format
 * 
 * INPUT          : 8-bit pointer to array of size 7
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Does not clear the OLED.
 * 
******************************************************************************** 
*/
vo voRTCPrintDateTime(u8* u8DateTimePtr)
{
   u8 u8Index;
   const u8 cu8Spacing[7] = {'-','-',' ',' ',':',':',' '};
   
   voOLEDHome();
   
   printf("20");
   
   for (u8Index = 0; u8Index < 7; u8Index++)
   {
      if (u8Index != 3)
      {
         if (u8DateTimePtr[u8Index] < 10)
         {
            printf("0");
         }

         printf("%u%c", u8DateTimePtr[u8Index], cu8Spacing[u8Index]);
      }
      else
      {
         printf("%s", mscu8DayArr[u8DateTimePtr[u8Index] - 1]);
         voOLEDRowTwo();
      }
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voRTCPrintEnable
 * 
 * DESCRIPTION    : Enable printing of date time
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voRTCPrintEnable(ORDER_FROM enSource)
{
   if (enSource == WATERING)
   {
      msu8PrintEnableWatering = 1;
   }
   else if (enSource == MENU)
   {
      msu8PrintEnableMenu = 1;
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voRTCPrintDisable
 * 
 * DESCRIPTION    : Enable printing of date time
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voRTCPrintDisable(ORDER_FROM enSource)
{
   if (enSource == WATERING)
   {
      msu8PrintEnableWatering = 0;
   }
   else if (enSource == MENU)
   {
      msu8PrintEnableMenu = 0;
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voRTCTask
 * 
 * DESCRIPTION    : Reads time registers from slave and writes on LCD every 0,5s
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voRTCTask(vo)
{
   static RTC_TYPE senState = READ_DATE;
   static u8 su8CurrentTime[7];

   switch (senState)
   {
      case READ_DATE:
         mu8RTCDelay = 50;       /* Read once every 0,5s */
         voRTCGetDateTime(su8CurrentTime);
         senState = PRINT_DATE;
      break;
      
      case PRINT_DATE:
         if (msu8PrintEnableWatering == 1 && msu8PrintEnableMenu == 1)
         {
            voRTCPrintDateTime(su8CurrentTime);
         }
         senState = WAIT;
      break;
      
      case WAIT:
         if (mu8RTCDelay == 0)
         {
            senState = READ_DATE;
         }
      break;
      
      default:
      break;
   }
}


