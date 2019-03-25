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
#include "key.h"
#include "led.h"
#include "beep.h"
#include "can.h"
#include "rs485.h"
#include "24l01.h"
#include "lora.h"
#include "task_key.h"
#include "task_beep.h"
#include "task_can.h"
#include "task_brake.h"
#include "task_brake_servo.h"
#include "task_changerail.h"
#include "task_ctrldata.h"
#include "task_fdbkdata.h"
#include "task_radio.h"
#include "task_upload.h"

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
	delay_ms(3000);
	KEY_Init();																  //按键初始化
	LED_Init();																  //LED灯初始化
	BEEP_Init();															  //Beep初始化
	RS485_Init(115200);															//rs485初始化
	BrakeCb_Init();
	RailCb_Init();
	CAN_Mode_Init(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_9tq, 8, CAN_Mode_Normal); //CAN初始化正常模式,波特率250Kbps
	radiolinkInit();														  //检查无线模块并初始化
	LED0 = 0;
	LED1 = 0;
	while(LoRa_Init());
	LoRa_Set();
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
//	xTaskCreate(vBrakeTask, "Brake", 100, NULL, 8, NULL);			/*创建brake任务*/
	xTaskCreate(vBrakeServoTask, "BrakeServer", 100, NULL, 8, NULL);/*创建brakeServer任务*/	
	xTaskCreate(vChangeRailTask, "RailChange", 100, NULL, 7, NULL);	/*创建Change Rail任务*/
	xTaskCreate(vCanSendTask, "CanSend", 100, NULL, 6, NULL);		/*创建CAN发送任务*/
	xTaskCreate(vCanReceiveTask, "CanReceive", 100, NULL, 5, NULL); /*创建CAN接收任务*/

	xTaskCreate(vRadioTask, "Radio", 100, NULL, 4, NULL);		/*创建Radio任务*/
	xTaskCreate(vCtrldataTask, "CtrlData", 100, NULL, 3, NULL); /*创建CtrlData解析任务*/
	xTaskCreate(vFdbkdataTask, "FdbkData", 100, NULL, 3, NULL); /*创建FdbkData组包任务*/

	xTaskCreate(vKeyTask, "Key", 100, NULL, 2, NULL);   /*创建按键扫描任务*/
	xTaskCreate(vUploadTask,"Upload",100,NULL,2,NULL);
	xTaskCreate(vBeepTask, "Beep", 100, NULL, 1, NULL); /*创建beep任务*/

	vTaskDelete(startTaskHandle); /* 删除开始任务 */

	taskEXIT_CRITICAL(); /*退出临界区*/
}

/**** Copyright (C)2018 Feisuo. All Rights Reserved **** END OF FILE ****/
