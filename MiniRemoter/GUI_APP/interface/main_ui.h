#ifndef __MAIN_UI_H
#define __MAIN_UI_H
#include <stdint.h>
#include <stdbool.h>
#include "task_ctrldata.h"
#include "task_fdbkdata.h"
/********************************************************************************	 
 * All rights reserved
********************************************************************************/ 



void main_ui(void);
void miniFlyMsgACKProcess(fdbkData* p);
	
void setRCLock(bool set);
bool getRCLock(void);

void setTrimFlag(bool set);
bool getTrimFlag(void);

void setMFLowPower(bool set);
bool getMFLowPower(void);

void setIsMFCanFly(bool set);
bool getIsMFCanFly(void);

u8 getRail(void);
#endif /*__MAIN_UI_H*/
