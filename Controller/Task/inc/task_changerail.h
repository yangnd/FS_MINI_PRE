#ifndef __TASK_CHANGERAIL_H
#define __TASK_CHANGERAIL_H			 
#include "sys.h"

#define LEFTRAIL 1
#define RIGHTRAIL 2

void RailCb_Init(void);
void vChangeRailTask(void *param);
#endif
