#include "task_fdbkdata.h"
#include "task_radio.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

fdbkData carFdbkData;
/*���ͷ�����Ϣ*/
void vFdbkdataTask(void *param)
{
	while (1)
	{
		vTaskDelay(1);
		radioReceiveFdbkPacket(&carFdbkData);
	}
}
