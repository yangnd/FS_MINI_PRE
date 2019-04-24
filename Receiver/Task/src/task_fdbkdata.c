#include "task_fdbkdata.h"
#include "task_radio.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

fdbkData carFdbkData;
/*发送反馈信息*/
void vFdbkdataTask(void *param)
{
	while (1)
	{
		vTaskDelay(1);
		radioReceiveFdbkPacket(&carFdbkData);
	}
}
