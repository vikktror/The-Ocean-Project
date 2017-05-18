/* 
 * File:   ScheduleDriver.h
 * Author: Viktor
 *
 * Created on den 13 mars 2017, 22:48
 */

#ifndef SCHEDULEDRIVER_H
#define	SCHEDULEDRIVER_H

typedef struct
{
   u8 u8Pot;
   u8 u8Duration;
   u8 u8Time[7];
   u8 u8Repeat;
}SCHEDULE_ITEM;

u8 u8ScheduleGetSize(vo);
vo voScheduleGetElementTime(u8 u8ElementNum, u8* u8DateTimeArr);
vo voSchdeuleGetFirstElement(SCHEDULE_ITEM* stFirstElementPtr);
vo voSchedulePrintElement(u8 u8ElementNum);
u8 u8SchedulePushBack(u8 u8Pot, u8 u8Duration, u8* u8DateTimeArr);
vo voSchedulePopFront(vo);
vo voScheduleRemoveAny(u8 u8ElementNum);
u8 u8ScheduleChangeAny(u8 u8ElementNum, u8 u8Pot, u8 u8Duration, u8* u8DateTimeArr);
u8 u8ScheduleCompareFirst(vo);

#endif	/* SCHEDULEDRIVER_H */

