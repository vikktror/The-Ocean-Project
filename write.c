
/*
 * File:   Write.c
 * Author: Viktor
 *
 * Created on den 02 november 2016, 13:27
 */
#include <stdio.h>
#include "OLED.h"

/*
********************************************************************************
 * FUNCTION NAME  : write
 * 
 * DESCRIPTION    : Directs output by printf() to the OLED
 * 
 * INPUT          : -
 * 
 * OUTPUT         : -
 * 
 * NOTE           : Called by automatic when using printf()
 * 
******************************************************************************** 
*/
int write(int handle, void *buffer, unsigned int len)
{
   int i;
   switch(handle)
   {
      case 0:
      case 1:
         
      case 2:
         for(i = len; i;i--){
            voOLEDWrite(*(char*)buffer++, DATA);
         }
      break;
      
      default:
      break;
   }
   
   return len;
}
