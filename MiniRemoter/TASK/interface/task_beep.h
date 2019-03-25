#ifndef __TASK_BEEP_H
#define __TASK_BEEP_H	 
#include "sys.h"
#include <stdbool.h>
/********************************************************************************	 
 * All rights reserved
********************************************************************************/

#define  BEEP  PCout(14)


typedef struct 
{
	bool isActive;
	bool isLoop;
	u16 waitMS;
}beepSeq_t;


enum beepAction
{
	TRIM_BEEP = 0,
	FLIP_BEEP,
	LOWPOWER_BEEP,
	CANFLY_BEEP,
};


void beepInit(void);
void runBeepAcktion(enum beepAction runaction);
void stopBeepAcktion(enum beepAction runaction);
#endif
