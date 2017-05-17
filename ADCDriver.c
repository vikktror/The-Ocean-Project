#include "ADCDriver.h"

u8 mu8ADCDelay;

typedef enum
{
   READ = 0,
   WRITE,
   WAIT
}ADC_TYPE;

 /*
 *******************************************************************************
 * FUNCTION NAME : voADCInit
 *
 * DESCRIPTION   : Initiates ADC on AN0 pin
 *
 * INPUT         : -
 *
 * OUTPUT        : -
 *
 * NOTE          : -
 *
 *******************************************************************************
 */
vo voADCInit(vo)
{
   AD1CON1bits.FORM = 0b00;                  /* Output data forms as an integer (0000 00dd dddd dddd) */
   AD1CON1bits.SSRC = 0x0;                   /* Clearing the SAMP bit ends sampling and starts conversion */
   AD1CON2bits.SMPI = 0x0;                   /* Interrupts at the completion of conversion for each sample/convert sequence */
   AD1CON2bits.VCFG = 0x0;                   /* Voltage reference at AVDD(3,3V) & AVSS pins */
   AD1CON3bits.ADCS = 0xFF;                  /* ADCON clk select bit */
   AD1CON3bits.SAMC = 0b10000;               /* TAD = 16 */

   AD1CON1bits.ADON = 1;                     /* Turn on ADC1 */

   AD1PCFGbits.PCFG0 = 0;                    /* Set AN0 as analog input */
}

 /*
 *******************************************************************************
 * FUNCTION NAME : u16ADCRead10Bit
 *
 * DESCRIPTION   : Read value of AN0 pin
 *
 * INPUT         : -
 *
 * OUTPUT        : 16-bit number
 *
 * NOTE          : -
 *
 *******************************************************************************
 */
u16 u16ADCRead10Bit(vo)
{
   u16 u16Index;

   AD1CHSbits.CH0SA = 0;
   AD1CHSbits.CH0SB = 0;

   AD1CON1bits.SAMP = 1;                             /* Start sampling */
   for (u16Index = 0; u16Index < 1000; u16Index++);  /* Sample delay, conversion start automatically */

   AD1CON1bits.SAMP = 0;                             /* Stop sampling to start conversion*/
   for (u16Index = 0; u16Index < 1000; u16Index++);  /* Delay, conversion start automatically */

   while (!AD1CON1bits.DONE);                        /* Wait for conversion to complete */
     
   return ADC1BUF0;
}