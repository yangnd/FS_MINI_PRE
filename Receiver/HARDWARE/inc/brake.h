#ifndef __BRAKE_H
#define __BRAKE_H
#include "sys.h"

#define BRAKE_EN PFout(0)  // PF0
#define BRAKE_IN1 PFout(2) // PF2
#define BRAKE_IN2 PFout(4) // PF4
//#define STOP_BRAKE 0
//#define RELEASE_BRAKE 1
//#define NORMAL_BRAKE 2
//#define EMERGENCY_BRAKE 3

void BRAKE_Init(void); //??IO???

#endif
