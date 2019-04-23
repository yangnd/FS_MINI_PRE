#include "task_fdbkdata.h"
#include "task_radio.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

fdbkData carFdbkDataF, carFdbkDataR;
/*发送反馈信息*/
void vFdbkdataTask(void *param)
{
	while (1)
	{
		vTaskDelay(10);
		//发送反馈数据
		radioSendPacket(&carFdbkDataF);
		vTaskDelay(10);
		//发送反馈数据
		radioSendPacket(&carFdbkDataR);
	}
}
