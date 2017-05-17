/* 
 * File:   RTC.h
 * Author: Viktor
 *
 * Created on den 3 march 2017, 22:38
 */

#include "Datatypes.h"

#ifndef RTC_H
#define	RTC_H

/* This enum is used for keeping track of which module that is 
 * enabling/disabling printing of calendar data. */
typedef enum
{
    WATERING = 0,
    MENU
}ORDER_FROM;

vo voRTCInit(vo);
vo voRTCSetDateTime(u8* u8DateTimeArr);
vo voRTCGetDateTime(u8* u8DateTimePtr);
vo voRTCPrintDateTime(u8* u8DateTimePtr);
vo voRTCPrintEnable(ORDER_FROM enSource);
vo voRTCPrintDisable(ORDER_FROM enSource);
vo voRTCTask(vo);

#endif	/* RTC_H */



