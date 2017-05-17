#include <xc.h>
#include "stdio.h"
#include "I2C.h"

#define SDA_TRIS  (_TRISG3)
#define SDA_LAT   (_LATG3)
#define SCL_TRIS  (_TRISG2)
#define SCL_LAT   (_LATG2)

u8 mu8I2CDelay;

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voI2CStart
 * 
 * DESCRIPTION    : Sets SDA and SCL in start condition
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voI2CStart(vo)
{
   I2C1CONbits.SEN = 1;          /* Set start condition */
   while (I2C1CONbits.SEN);     /* Wait for bit to be cleared in HW */
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voI2CRepeatedStart
 * 
 * DESCRIPTION    : Sets SDA and SCL in repeated start condition
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voI2CRepeatedStart(vo)
{
   I2C1CONbits.RSEN = 1;
   while (I2C1CONbits.RSEN);
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voI2CStop
 * 
 * DESCRIPTION    : Sets SDA and SCL in stop condition
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voI2CStop(vo)
{
   I2C1CONbits.PEN = 1;
   while (I2C1CONbits.PEN);
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voI2CACK
 * 
 * DESCRIPTION    : Sends ACK to slave
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voI2CACK(vo)
{
   I2C1CONbits.ACKDT = 0;        /* Send ACK during ACK */
   I2C1CONbits.ACKEN = 1;        /* Send ACK */
   while (I2C1CONbits.ACKEN);
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voI2CNACK
 * 
 * DESCRIPTION    : Sends NACK to slave
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voI2CNACK(vo)
{
   I2C1CONbits.ACKDT = 1;        /* Send NACK during ACK */
   I2C1CONbits.ACKEN = 1;        /* Send ACK */
   while (I2C1CONbits.ACKEN);
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voI2CTx
 * 
 * DESCRIPTION    : Trancieves one byte of data to slave
 * 
 * INPUT          : 8-bit number
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : -
******************************************************************************** 
*/
vo voI2CTx(u8 u8Byte)
{
   I2C1TRN = u8Byte;             /* Write a byte to RTC */
   while (I2C1STATbits.TRSTAT);
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voI2CRx
 * 
 * DESCRIPTION    : Recives one byte of data from slave
 * 
 * INPUT          : -
 * 
 * OUTPUT         : 8-bit number 
 * 
 * NOTE           : -
******************************************************************************** 
*/
u8 u8I2CRx(vo)
{
   I2C1CONbits.RCEN = 1;         /* Enable recieve */
   while (I2C1CONbits.RCEN);
   
   return I2C1RCV;
}

/*
********************************************************************************
 * 
 * FUNCTION NAME  : voI2COpen
 * 
 * DESCRIPTION    : Manually wakes up slave, enables I2C1
 * 
 * INPUT          : -
 * 
 * OUTPUT         : - 
 * 
 * NOTE           : This code is written specific for DS1307 RTC I2C interface
******************************************************************************** 
*/
vo voI2COpen(vo)
{
   /* SCL Output, SDA input */
   SCL_TRIS = 0;
   SDA_TRIS = 1;
   
   SCL_LAT = 0;
   
   /* Manually and slowly generates clockpulses on SCL to sync PIC with RTC */
   while (PORTGbits.RG3 == 0)
   {
      mu8I2CDelay = 1;
      while (mu8I2CDelay > 0);
      
      SCL_LAT = !SCL_LAT;
   }
   
   /* SDA as output */
   SDA_TRIS = 0;
   
   /* Manually generates stop condition on the bus */
   SCL_LAT = 1;
   SDA_LAT = 0;
   
   /* Wait a few ms */
   mu8I2CDelay = 1;
   while (mu8I2CDelay > 0);
   
   SDA_LAT = 1;
   
   /* RTC is now unlocked and I2C can be initialized*/
   
   /* Set both ports as inputs */
   SCL_TRIS = 1;
   SDA_TRIS = 1;
   
   I2C1BRG = 0x9D;               /* Baud rate 100kHz for Fcy = 16MHz */
   I2C1CONbits.DISSLW = 1;       /* Disable slew rate control bit */
   I2C1CONbits.I2CEN = 1;        /* Enable I2C1 */
   
}
