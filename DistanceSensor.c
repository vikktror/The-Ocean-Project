#include "DistanceSensor.h"
#include "ADCDriver.h"

u8 mu8DistanceSensorDelay;
u8 mu16DistanceSensorVal = 0;

typedef enum
{
   READ10BIT = 0,
   WAIT
}SENSOR_TYPE;

u16 u16DistanceSensorGetValue(vo)
{
   return mu16DistanceSensorVal;
}

vo voDistanceSensorTask(vo)
{
   static SENSOR_TYPE senState = READ10BIT;
   
   switch (senState)
   {
      case READ10BIT:
         mu16DistanceSensorVal = u16ADCRead10Bit();
         senState = WAIT;
      break;
      
      case WAIT:
         if (mu8DistanceSensorDelay == 0)
         {
            mu8DistanceSensorDelay = 10;
            senState = READ10BIT;
         }
      break;
      
      default:
      break;
   }
}