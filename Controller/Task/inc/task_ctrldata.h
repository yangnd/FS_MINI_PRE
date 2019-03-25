#ifndef __TASK_CTRLDATA_H
#define __TASK_CTRLDATA_H
#include "sys.h"

/*ͨѶ�����ź����ݽṹ*/
typedef struct
{
	u8 MotoSel;
	u8 ControlMode;
	u8 Gear;
	u8 Throttle;
	u8 Rail;
	u8 Brake;
}ctrlData;

void vCtrldataTask(void *param);
u8 getBrake(void);
u8 getRail(void);
#endif
