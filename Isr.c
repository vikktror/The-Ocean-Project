#include <xc.h>

#include "ADCDriver.h"
#include "I2C.h"
#include "MenuDriver.h"
#include "OLED.h"
#include "PumpNavigation.h"
#include "RotaryEncoder.h"
#include "RTC.h"
#include "StatusLED.h"
#include "Tick.h"
#include "Timer.h"
#include "Watering.h"


extern u8 mu8LEDDelay;
extern u8 mu8OLEDDelay;
extern u8 mu8I2CDelay;
extern u8 mu8RTCDelay;
extern u8 mu8RotaryDelay;
extern ROTARY_EVENT menRotaryDir;
extern ROTARY_BUTTON menRotaryButton;
extern u8 mu8TickDelay;
extern u8 mu8TimerDelay;
extern u8 mu8MenuHandlerDelay;
extern u8 mu8ADCDelay;
extern u16 mu16WateringDelay;
extern u8 mu8FeedDelay;
extern u8 mu8PumpNavigationDelay;

static u8 msu8PushButtonDelay;

/*
********************************************************************************
 * FUNCTION NAME  : _T1Interrupt
 * 
 * DESCRIPTION    : Timer1 interrupt routine. Interrupts every 0,01s.
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : -
 * 
******************************************************************************** 
*/
void _ISR __attribute__((interrupt, auto_psv)) _T1Interrupt(void)
{   
   if (mu8LEDDelay > 0)
   {
      mu8LEDDelay--;
   }
   if (mu8OLEDDelay > 0)
   {
      mu8OLEDDelay--;
   }
   if (mu8I2CDelay > 0)
   {
      mu8I2CDelay--;
   }
   if (mu8RTCDelay > 0)
   {
      mu8RTCDelay--;
   }
   if (mu8RotaryDelay > 0)
   {
      mu8RotaryDelay--;
   }
   if (mu8TickDelay > 0)
   {
      mu8TickDelay--;
   }
   if (mu8TimerDelay > 0)
   {
      mu8TimerDelay--;
   }
   if (mu8MenuHandlerDelay > 0)
   {
      mu8MenuHandlerDelay--;
   }
   if (mu8ADCDelay > 0)
   {
      mu8ADCDelay--;
   }
   if (mu16WateringDelay > 0)
   {
      mu16WateringDelay--;
   }
   if (mu8FeedDelay > 0)
   {
      mu8FeedDelay--;
   }
   if (msu8PushButtonDelay > 0)
   {
      msu8PushButtonDelay--;
   }
   if (mu8PumpNavigationDelay > 0)
   {
      mu8PumpNavigationDelay--;
   }
   
   _T1IF = 0;  /* Reset interrupt flag */
}

/*
********************************************************************************
 * FUNCTION NAME  : _CNInterrupt
 * 
 * DESCRIPTION    : CN0 interrupt routine. Interrupt is trigged by ROTARY_PORT_A
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Setup is located in RotaryEncoder-module.
 * 
******************************************************************************** 
*/
void _ISR __attribute__((interrupt, auto_psv)) _CNInterrupt(void)
{
   if (ROTARY_A_PORT == 0)
   {
      if (ROTARY_B_PORT == 0)
      {
         menRotaryDir = CW;
      }
      else if (ROTARY_B_PORT == 1)
      {
         menRotaryDir = CCW;
      }
   }
      
   _CNIF = 0; /* Clear interrupt flag */
}

/*
********************************************************************************
 * FUNCTION NAME  : _INT4Interrupt
 * 
 * DESCRIPTION    : INT4 (External) interrupt routine. Interrupts on positive edge.
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Setup is located in RotaryEncoder-module.
 * 
******************************************************************************** 
*/
void _ISR __attribute__((interrupt, auto_psv)) _INT4Interrupt(void)
{
   if (menRotaryButton == N_PRESSED && msu8PushButtonDelay == 0)
   {
         menRotaryButton = PRESSED;
         msu8PushButtonDelay = 10;
   }
   _INT4IF = 0;
}
