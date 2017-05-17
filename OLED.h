#include <xc.h>
#include "DataTypes.h"

#ifndef OLED_H
#define	OLED_H

/* The type of data that is being sent to the OLED */
typedef enum
{
    COMMAND = 0,
    DATA
}enOLEDDataType;

vo voOLEDInit(vo);
vo voOLEDPuts(u8 *u8StringPtr);
vo voOLEDHome(vo);
vo voOLEDRowTwo(vo);
vo voOLEDClear(vo);

vo voOLEDWrite(u8 u8Character, 
               u16 u16Type);

vo voOLEDMoveCursor(u8 u8Row, 
                    u8 u8Column);
u8 u8OLEDReadSign(vo);
vo voOLEDReadScreen(u8* u8ScreenArr);

u8 u8OLEDBlinkScreen(u8 u8Cycles, 
                     u8 u8BlinkSpeed);

vo voOLEDPrint32BArr(u8* u8ScreenArr);


#endif	/* OLED_H */

