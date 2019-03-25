#include "task_lora.h"
#include "lora.h"
#include "led.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

u8 rx_buf[100];
u16 rx_len;
u8 uLoraTimeoutFlag;
static xSemaphoreHandle loraIT;
/*lora�ⲿ�жϻص�����*/
static void lora_interruptCallback(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(loraIT, &xHigherPriorityTaskWoken);
//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //�����Ҫ�Ļ�����һ�������л�
}
void lora_task_init(void)
{
	LoRa_Set();
	loraIT = xSemaphoreCreateBinary();
	lora_setIterruptCallback(lora_interruptCallback);
}
void vLoraTask(void *param)
{
	static u8 t=0;
	Lora_mode=1;
	while(LORA_AUX);
	LoRa_SendData(rx_buf);		//��Ҫ����һ��ģ�������������ԭ��δ֪������			
	while (1)
	{
		if(xSemaphoreTake(loraIT, 2000)==pdTRUE)
		{
			if (LoRa_ReceData(rx_buf,&rx_len) == 1)
			{
				uLoraTimeoutFlag = 0; //�����ʱ��ʶ
				LED0=0;
				t=0;
			}
						
		}
		else
		{
			t++;
			rx_len=0;
			if (t == 2) //����2��û�н��գ���ʱ
			{
				t = 0;
				uLoraTimeoutFlag = 1; //�ó�ʱ��ʶ
				LED0=1;
			}
		}
	}
}

