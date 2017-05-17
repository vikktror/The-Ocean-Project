#include <xc.h>
#include <stdio.h>

#include "RotaryEncoder.h"
#include "OLED.h"

#define ROTARY_A_TRIS      (TRISCbits.TRISC14)
#define ROTARY_B_TRIS      (TRISGbits.TRISG12)
#define ROTARY_BUTTON_TRIS (TRISAbits.TRISA15) 

u8 mu8RotaryDelay;
static u8 su8Counter;

ROTARY_EVENT menRotaryDir = CLEAR;
ROTARY_BUTTON menRotaryButton = N_PRESSED;

typedef enum
{
   UPDATE = 0,
   WAIT
}ROTARY_TYPE;


/*
********************************************************************************
 * 
 * FUNCTION NAME  : voRotaryEncoderInit
 * 
 * DESCRIPTION    : Initates timers, I/Os for the rotary encoder
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voRotaryEncoderInit(vo)
{
   /* Set both rotary switch I/Os as inputs */
   ROTARY_A_TRIS = 1;
   ROTARY_B_TRIS = 1;

   /* Change notification interrupt init */
   _CN0PUE = 1;   /* Enable weak pullup for CN0-pin */
   _CNIP = 4;     /* Change notification interrupt priority */
   _CNIF = 0;     /* Clear interrupt flag */
   _CN0IE = 1;    /* Enable interrupt on pin CN0 */
   _CNIE = 1;     /* Enable CN Interrupts */
   
   ROTARY_BUTTON_TRIS = 1;
   _INT4EP = 1;   /* Interrupt on negative flank, when button is released */
   _INT4IF = 0;
   _INT4IE = 1;
   _INT4IP = 3;   /* Lower priority than rotary functions */
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : enGetRotaryButton
 * 
 * DESCRIPTION    : Return the value on the pushbutton of rotary encoder
 * 
 * INPUT          : -
 * 
 * OUTPUT         : Enum (N_PRESSED or PRESSED)
 * 
 * NOTE           : This is the ONLY function to use for the button.
 *                  EVERY time the button is checked, use this function.
 * 
******************************************************************************** 
*/
ROTARY_BUTTON enGetRotaryButton(vo)
{
   if (menRotaryButton == PRESSED)
   {
      menRotaryButton = N_PRESSED;
      return PRESSED;
   }
   else
   {
      return N_PRESSED;
   }
}


/*
********************************************************************************
 * 
 * FUNCTION NAME  : voRotaryEncoderTask
 * 
 * DESCRIPTION    : Updates the value counted by the rotary encoder
 * 
 * INPUT          : 3x 8-bit value, max/min value to count, wrapAround OFF/ON
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
vo voRotaryEncoderTask(u8 u8MaxCount, 
                       u8 u8MinCount, 
                       u8 u8WrapAround)
{
   static ROTARY_TYPE senState = UPDATE;
   
   switch(senState)
   {
      case UPDATE:
         
         /* Clockwise rotation */
         if (menRotaryDir == CW)
         {
            /* If u8WrapAround is active, don't stop at max. Wrap around instead. */
            if (u8WrapAround == 1)
            {
               if (su8Counter == u8MaxCount)
               {
                  su8Counter = u8MinCount;
               }
               else
               {
                  su8Counter++;
               }
            }
            else if(su8Counter < u8MaxCount)
            {
               su8Counter++;
            }
            menRotaryDir = CLEAR;
         }
         /* Counter clockwise rotation */
         else if (menRotaryDir == CCW)
         {
            if (u8WrapAround == 1)
            {
               if (su8Counter == u8MinCount)
               {
                  su8Counter = u8MaxCount;
               }
               else
               {
                  su8Counter--;
               }
            }  
            else if (su8Counter > u8MinCount)
            {
               su8Counter--;
            }
            menRotaryDir = CLEAR;
         }
         senState = WAIT;
      break;
         
      case WAIT:
         if (mu8RotaryDelay == 0)
         {
            mu8RotaryDelay = 5;
            senState = UPDATE;
         }
      break;
         
      default:
      break;
   }
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : u8GetRotaryValue
 * 
 * DESCRIPTION    : Returns the value counted by the rotary encoder
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit value
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
u8 u8GetRotaryValue(vo)
{
   return su8Counter;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voResetRotaryValue
 * 
 * DESCRIPTION    : Resets the rotary encoder value to 0
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
vo voResetRotaryValue(vo)
{
   su8Counter = 0;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voSetRotaryValue
 * 
 * DESCRIPTION    : Set the value counted by the rotary encoder
 * 
 * INPUT          : 8-bit value
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
vo voSetRotaryValue(u8 u8Value)
{
   su8Counter = u8Value;
}