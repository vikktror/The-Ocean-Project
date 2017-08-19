/* 
 * File:   ScheduleDriver.h
 * Author: Viktor
 *
 * Created on den 13 mars 2017, 22:48
 */

#ifndef SCHEDULEDRIVER_H
#define	SCHEDULEDRIVER_H

/* The watering struct */
typedef struct
{
    u8 u8Pot;       /* Pot to be watered */
    u8 u8Amount;    /* Amount of water. u8Amount * 0,5 = dl */
    u8 u8Repeat;    /* Repeat watering? 0 = never, 1 = every day, 2 = every other day, 3 = every week */
    u8 u8Time[7];   /* Date & time for the watering */
}SCHEDULE_ITEM;

u8 u8ScheduleGetSize(vo);
vo voScheduleGetElementTime(u8 u8ElementNum, u8* u8DateTimeArr);
vo voSchdeuleGetFirstElement(SCHEDULE_ITEM* stFirstElementPtr);
vo voSchedulePrintElement(u8 u8ElementNum);
u8 u8SchedulePushBack(u8 u8Pot, u8 u8Amount, u8 u8Repeat, u8* u8DateTimeArr);
vo voSchedulePopFront(vo);
vo voScheduleRemoveAny(u8 u8ElementNum);
u8 u8ScheduleChangeAny(u8 u8ElementNum, u8 u8Pot, u8 u8Amount, u8 u8Repeat, u8* u8DateTimeArr);
u8 u8ScheduleCompareFirst(vo);
vo voScheduleReschedule(SCHEDULE_ITEM stWatering);

#endif	/* SCHEDULEDRIVER_H */

