#ifndef __TASK_RADIO_H
#define __TASK_RADIO_H

#include <stdbool.h>
#include "sys.h"
#include "task_ctrldata.h"
#include "task_fdbkdata.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

enum motoSel
{
	FRONT_ONLY,
	REAR_ONLY,
	FRONT_REAR,
};
enum motoID
{
	MOTO_FRONT,
	MOTO_REAR,
};
enum motoMode
{
	THROTTLE,
	TORQUE,
	SPEED,
};
enum motoGear
{
	NONE,
	DRIVE,
	REVERSE,
	LOW,
};

extern xTaskHandle radiolinkTaskHandle;

void radiolinkInit(void);

bool radioReceiveCtrlPacket(ctrlData *p);
bool radioReceiveFdbkPacket(fdbkData *p);
void vRadioTask(void* param);
u16 getRxCount(void);
bool radioConnectStatus(void);
void radioEnable(FunctionalState state);

#endif /*RADIO_LINK_H*/
