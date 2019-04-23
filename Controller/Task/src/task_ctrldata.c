#include "task_ctrldata.h"
#include "task_radio.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

ctrlData carCtrlData=
{
	.MotoSel=0x00,
	.ControlMode=0x00,
	.Gear=0x00,
	.Throttle=0x00,
	.Rail=0x00,
	.Brake=0x00,
};
/*发送控制命令任务*/
void vCtrldataTask(void *param)
{
	while (1)
	{
		radioReceivePacketBlocking(&carCtrlData);
		//TO DO 解包carCtrlData
		vTaskDelay(1);
	}
}

u8 getBrake(void)
{
	return carCtrlData.Brake;
}

u8 getRail(void)
{
	return carCtrlData.Rail;
}
