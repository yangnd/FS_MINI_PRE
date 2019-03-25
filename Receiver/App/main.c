/**
  ********************************  STM32F10x  *********************************
  * @文件名     ： main.c
  * @作者       ： 
  * @库版本     ： V3.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2018年9月10日
  * @摘要       ： 主函数
  ******************************************************************************/
/*----------------------------------------------------------------------------
  更新日志:
  2018-09-10 V1.0.0:初始版本
  ----------------------------------------------------------------------------*/
/* 包含的头文件 --------------------------------------------------------------*/
/* Standard includes. */
//#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
//#include "queue.h"

/* Library includes. */
#include "stm32f10x_it.h"

/* app includes. */
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "led.h"
#include "beep.h"
#include "lora.h"
#include "task_key.h"
#include "task_beep.h"
#include "task_lora.h"
#include "task_usart.h"

static TaskHandle_t startTaskHandle;
static void startTask(void *param);

/************************************************
函数名称 ： main
功    能 ： 主函数入口
参    数 ： 无
返 回 值 ： int
作    者 ： 
*************************************************/
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);							  /*中断配置初始化*/
	delay_init();
	uart_init(115200);
	KEY_Init();																  //按键初始化
	LED_Init();																  //LED灯初始化
	BEEP_Init();															  //Beep初始化

	LED0 = 0;
	LED1 = 0;
	while(LoRa_Init());
	LED0=1;
	LED1=0;
	lora_task_init();
	LED0 = 1;
	LED1 = 1;

	xTaskCreate(startTask, "START_TASK", 100, NULL, 2, &startTaskHandle); /*创建起始任务*/
	vTaskStartScheduler();												  /*开启任务调度*/

	while (1)
	{
	}; /* 任务调度后不会执行到这 */
}

/*创建任务*/
void startTask(void *param)
{
	taskENTER_CRITICAL();											/*进入临界区*/

	xTaskCreate(vUsartTask, "USART", 100, NULL, 4, NULL);		/*创建Radio任务*/
	xTaskCreate(vLoraTask,"Lora",100,NULL,3,NULL);
	xTaskCreate(vKeyTask, "Key", 100, NULL, 2, NULL);   /*创建按键扫描任务*/
//	xTaskCreate(vBeepTask, "Beep", 100, NULL, 1, NULL); /*创建beep任务*/

	vTaskDelete(startTaskHandle); /* 删除开始任务 */

	taskEXIT_CRITICAL(); /*退出临界区*/
}

/**** Copyright (C)2018 Feisuo. All Rights Reserved **** END OF FILE ****/
