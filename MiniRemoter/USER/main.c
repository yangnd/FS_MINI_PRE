#include "usart.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "oled.h"
#include "24l01.h"
#include "adc.h"
//#include "hw_config.h"
#include "config_param.h"
#include "task_beep.h"
#include "task_radio.h"
//#include "usblink.h"
#include "task_ctrldata.h"
#include "task_fdbkdata.h"
#include "display.h"
#include "task_key.h"
/* FreeRtos includes */
#include "FreeRTOS.h"
#include "task.h"

/********************************************************************************	 
 * main.c	
 * 包括系统初始化和创建任务
 * 创建日期:2018/10/23
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 江苏飞梭
 * All rights reserved
********************************************************************************/

static TaskHandle_t startTaskHandle;
static void startTask(void *param);

int main(void)
{
	NVIC_SetVectorTable(FIRMWARE_START_ADDR, 0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); /*中断配置初始化*/
	delay_init();									/*delay初始化*/
	configParamInit();								/*配置参数初始化*/
	ledInit();										/*led初始化*/
	oledInit();										/*oled初始化*/
	beepInit();										/*蜂鸣器初始化*/
	keyInit();										/*按键初始化*/
	joystickInit();									/*摇杆初始化*/
	// usb_vcp_init();		/*usb虚拟串口初始化*/

	radiolinkInit(); /*无线通信初始化*/
	// usblinkInit();		/*usb通信初始化*/
	displayInit();   /*显示初始化*/

	xTaskCreate(startTask, "START_TASK", 100, NULL, 2, &startTaskHandle); /*创建起始任务*/
	vTaskStartScheduler();												  /*开启任务调度*/

	while (1)
	{
	}; /* 任务调度后不会执行到这 */
}

/*创建任务*/
void startTask(void *param)
{
	taskENTER_CRITICAL(); /*进入临界区*/

	xTaskCreate(vRadioTask, "RADIOLINK", 100, NULL, 6, &radiolinkTaskHandle); /*创建无线连接任务，信号量1000*/

	//	xTaskCreate(usblinkTxTask, "USBLINK_TX", 100, NULL, 5, NULL);	/*创建usb发送任务，队列max*/
	//	xTaskCreate(usblinkRxTask, "USBLINK_RX", 100, NULL, 5, NULL);	/*创建usb接收任务*/

	xTaskCreate(vCtrldataTask, "COMMANDER", 100, NULL, 4, NULL); /*创建飞控指令发送任务，10*/

	xTaskCreate(vFdbkdataTask, "DATA_PROCESS", 100, NULL, 4, NULL); /*创建无线通信数据处理任务，1*/

	xTaskCreate(keyTask, "BUTTON_SCAN", 100, NULL, 3, NULL); /*创建按键扫描任务，20*/

	xTaskCreate(displayTask, "DISPLAY", 200, NULL, 1, NULL); /*创建显示任务50*/

	xTaskCreate(configParamTask, "CONFIG_TASK", 100, NULL, 1, NULL); /*创建参数配置任务1000*/

	vTaskDelete(startTaskHandle); /*删除开始任务*/

	taskEXIT_CRITICAL(); /*退出临界区*/
}
