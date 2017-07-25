#include <stdio.h>

#include "DistanceSensor.h"
#include "ADCDriver.h"
#include "OLED.h"

u8 mu8DistanceSensorDelay;

typedef enum
{
   READ10BIT = 0,
   PRINT,
   WAIT
}SENSOR_TYPE;

vo voDistanceSensorTask(vo)
{
   static SENSOR_TYPE senState = READ10BIT;
   static u16 su16MeanValue = 0;
   
   u16 u16ReadValue = 0;
   
   
   switch (senState)
   {
      case READ10BIT:
         u16ReadValue = u16ADCRead10Bit();
         u16ReadValue += u16ADCRead10Bit();
         u16ReadValue += u16ADCRead10Bit();
         
         su16MeanValue = (u16ReadValue / 3);
         
         senState = PRINT;
      break;
      
      case PRINT:
         voOLEDClear();
         voOLEDHome();
         printf("ADC: %u", su16MeanValue);
         senState = WAIT;
      break;
      
      case WAIT:
         if (mu8DistanceSensorDelay == 0)
         {
            mu8DistanceSensorDelay = 100;
            senState = READ10BIT;
         }
      break;
      
      default:
      break;
   }
}