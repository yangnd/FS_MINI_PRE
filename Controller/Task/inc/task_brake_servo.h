#ifndef __TASK_BRAKE_SERVO_H
#define __TASK_BRAKE_SERVO_H
#include "sys.h"



void BrakeCb_Init(void);
void vBrakeServoTask(void *param);
float getDec(void);

#endif

