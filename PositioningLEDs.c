#include <xc.h>

#include "PositioningLEDs.h"

#define HOME_TRIS (TRISCbits.TRISC1)
#define LED1_TRIS (TRISCbits.TRISC2)
#define LED2_TRIS (TRISCbits.TRISC3)
#define LED3_TRIS (TRISCbits.TRISC4)
#define LED4_TRIS (TRISGbits.TRISG6)

#define HOME_LAT (LATCbits.LATC1)
#define LED1_LAT (LATCbits.LATC2)
#define LED2_LAT (LATCbits.LATC3)
#define LED3_LAT (LATCbits.LATC4)
#define LED4_LAT (LATGbits.LATG6)

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voPositioningLEDsInit
 * 
 * DESCRIPTION    : Initiates I/Os for positioning LEDs
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
vo voPositioningLEDsInit(vo)
{
   HOME_TRIS = 0;
   LED1_TRIS = 0;
   LED2_TRIS = 0;
   LED3_TRIS = 0;
   LED4_TRIS = 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8BinPow
 * 
 * DESCRIPTION    : 2 to the power of <u8Exponent>
 * 
 * INPUT          : 8-bit number (the exponent) 
 * 
 * OUTPUT         : 8-bit number
 * 
 * NOTE           : Only valid for input between 0 - 7.
 * 
******************************************************************************** 
*/
u8 u8BinPow(u8 u8Exponent)
{
   u8 u8Index;
   u8 u8ReturnValue = 1;
   
   for (u8Index = 0; u8Index < u8Exponent; u8Index++)
   {
      u8ReturnValue = u8ReturnValue * 2;
   }
   
   return u8ReturnValue;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8PositioningLEDsSet
 * 
 * DESCRIPTION    : Turn on the corresponding LED connected to a pot
 * 
 * INPUT          : 8-bit number (0 - 4)
 * 
 * OUTPUT         : 8-bit number (0 fail, 1 OK)
 * 
 * NOTE           : 0 = home LED, rest = LEDs by pot number
 * 
******************************************************************************** 
*/
u8 u8PositioningLEDsSet(u8 u8PotNumber)
{
   u8 u8LEDNumber;
   
   if (u8PotNumber >= 0 && u8PotNumber < 5)
   {
      u8LEDNumber = u8BinPow(u8PotNumber);
      
      HOME_LAT = (u8LEDNumber >> 0) & 1;
      LED1_LAT = (u8LEDNumber >> 1) & 1;
      LED2_LAT = (u8LEDNumber >> 2) & 1;
      LED3_LAT = (u8LEDNumber >> 3) & 1;
      LED4_LAT = (u8LEDNumber >> 4) & 1;
      
      return 1;
   }
   
   return 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voPositioningLEDsOff
 * 
 * DESCRIPTION    : Turn off all positioning LEDs
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
vo voPositioningLEDsOff(vo)
{
   HOME_LAT = 0;
   LED1_LAT = 0;
   LED2_LAT = 0;
   LED3_LAT = 0;
   LED4_LAT = 0;
}
