/* 
 * File:   MenuDriver.h
 * Author: Viktor
 *
 * Created on den 8 mars 2017, 21:47
 */

#ifndef MENUDRIVER_H
#define	MENUDRIVER_H

typedef enum
{
    VIEW = 0,
    ADD,
    CHANGE,
    REMOVE
}EVENT_TYPE;

u8 u8MenuSingleLine(u8 u8NumHeaders, u8 u8MenuHeadersArr[][13]);
u8 u8MenuScheduleMenu(u8 u8NumItems);
u8 u8MenuConfigureTime(u8* u8DateTimeArr);
u8 u8MenuConfigurePot(vo);
u8 u8MenuConfigureDuration(vo);
u8 u8MenuAddNewToSchedule(vo);
u8 u8MenuChangeInSchedule(u8 u8EventNum);
u8 u8MenuSchedulerTask(EVENT_TYPE enEvent);
u8 u8MenuMovePump(vo);

#endif	/* MENUDRIVER_H */

