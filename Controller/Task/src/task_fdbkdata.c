#include "task_fdbkdata.h"
#include "task_radio.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

fdbkData carFdbkDataF, carFdbkDataR;
/*���ͷ�����Ϣ*/
void vFdbkdataTask(void *param)
{
	while (1)
	{
		vTaskDelay(10);
		//���ͷ�������
		radioSendPacket(&carFdbkDataF);
		vTaskDelay(10);
		//���ͷ�������
		radioSendPacket(&carFdbkDataR);
	}
}
