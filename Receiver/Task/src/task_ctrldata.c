#include "task_ctrldata.h"
#include "task_radio.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

ctrlData carCtrlData;
/*���Ϳ�����������*/
void vCtrldataTask(void *param)
{
	while (1)
	{
		radioReceiveCtrlPacket(&carCtrlData);
		//TO DO ���carCtrlData

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
