#ifndef __TASK_CTRLDATA_H
#define __TASK_CTRLDATA_H
#include <stdbool.h>
#include "sys.h"
#include "task_joystick.h"
/********************************************************************************	 
 * All rights reserved
********************************************************************************/

/*通讯控制信号数据结构*/
typedef struct
{
	u8 MotoSel;
	u8 ControlMode;
	u8 Gear;
	u8 Throttle;
	u8 Rail;
	u8 Break;
}ctrlData;

u8 limit(u8 value,u8 min, u8 max);
void vCtrldataTask(void* param);
fJoyStick getPercent(void);

#endif /*__COMMANDER_H*/
