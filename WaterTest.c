#include "WaterTest.h"
#include "Watering.h"
#include "OLED.h"

#include <stdio.h>

u16 mu16WaterTestDelay;


typedef enum WATER_TEST
{
   FIRST = 0,
   WATER,
   WAIT1,
   STOP,
   WAIT2,
   DONE
}WATER_TEST;

vo voWaterTest(vo)
{
   static WATER_TEST senState = FIRST;
   static u8 su8Counter = 0;
   
   switch (senState)
   {
      case FIRST:
         mu16WaterTestDelay = 500;
         senState = WAIT2;
      break;
      
      case WATER:
         su8Counter++;
         PUMP = 1;
         mu16WaterTestDelay = 850;
         senState = WAIT1;
      break;
      
      case WAIT1:
         if (mu16WaterTestDelay == 0)
         {
            senState = STOP;
         }
      break;
      
      case STOP:
         PUMP = 0;
         mu16WaterTestDelay = 1000;
         senState = WAIT2;
      break;
      
      case WAIT2:
         if (mu16WaterTestDelay == 0)
         {
            if(su8Counter == 5)
            {
               senState = DONE;
               voOLEDClear();
               voOLEDHome();
               printf("DONE!");
               break;
            }
            senState = WATER;
         }
      break;
      
      case DONE:
      break;
      
      default:
      break;
   }
}
