#include "task_beep.h"
#include "task_radio.h"
#include "beep.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
bool uConnectStatus;
void vBeepTask(void *param)
{
	while (1)
	{
		vTaskDelay(300);
		BEEP = 0;
		vTaskDelay(300);
//		uConnectStatus = radioConnectStatus();
		if (!uConnectStatus)
			BEEP = 1;
	}
}
