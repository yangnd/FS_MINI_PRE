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
/*lora外部中断回调函数*/
static void lora_interruptCallback(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(loraIT, &xHigherPriorityTaskWoken);
//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //如果需要的话进行一次任务切换
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
	LoRa_SendData(rx_buf);		//需要发送一次模块才正常工作，原因未知？？？			
	while (1)
	{
		if(xSemaphoreTake(loraIT, 2000)==pdTRUE)
		{
			if (LoRa_ReceData(rx_buf,&rx_len) == 1)
			{
				uLoraTimeoutFlag = 0; //清除超时标识
				LED0=0;
				t=0;
			}
						
		}
		else
		{
			t++;
			rx_len=0;
			if (t == 2) //连续2次没有接收，则超时
			{
				t = 0;
				uLoraTimeoutFlag = 1; //置超时标识
				LED0=1;
			}
		}
	}
}

