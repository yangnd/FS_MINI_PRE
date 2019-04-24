#ifndef __TASK_CHANGERAIL_H
#define __TASK_CHANGERAIL_H			 
#include "sys.h"

#define MID			0
#define LEFTRAIL 	1
#define RIGHTRAIL 	2
#define ERRORRAIL	3
#define LEFT GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_12)
#define RIGHT GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_13)
#define CHANGE_EN PFout(14)
#define TURN_RIGHT PFout(15)

void RailCb_Init(void);
void ChangerailInit(void);
void vChangeRailTask(void *param);
#endif
