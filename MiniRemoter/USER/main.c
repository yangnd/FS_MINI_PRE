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
 * ����ϵͳ��ʼ���ʹ�������
 * ��������:2018/10/23
 * �汾��V1.0
 * ��Ȩ���У�����ؾ���
 * Copyright(C) ���շ���
 * All rights reserved
********************************************************************************/

static TaskHandle_t startTaskHandle;
static void startTask(void *param);

int main(void)
{
	NVIC_SetVectorTable(FIRMWARE_START_ADDR, 0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); /*�ж����ó�ʼ��*/
	delay_init();									/*delay��ʼ��*/
	configParamInit();								/*���ò�����ʼ��*/
	ledInit();										/*led��ʼ��*/
	oledInit();										/*oled��ʼ��*/
	beepInit();										/*��������ʼ��*/
	keyInit();										/*������ʼ��*/
	joystickInit();									/*ҡ�˳�ʼ��*/
	// usb_vcp_init();		/*usb���⴮�ڳ�ʼ��*/

	radiolinkInit(); /*����ͨ�ų�ʼ��*/
	// usblinkInit();		/*usbͨ�ų�ʼ��*/
	displayInit();   /*��ʾ��ʼ��*/

	xTaskCreate(startTask, "START_TASK", 100, NULL, 2, &startTaskHandle); /*������ʼ����*/
	vTaskStartScheduler();												  /*�����������*/

	while (1)
	{
	}; /* ������Ⱥ󲻻�ִ�е��� */
}

/*��������*/
void startTask(void *param)
{
	taskENTER_CRITICAL(); /*�����ٽ���*/

	xTaskCreate(vRadioTask, "RADIOLINK", 100, NULL, 6, &radiolinkTaskHandle); /*�����������������ź���1000*/

	//	xTaskCreate(usblinkTxTask, "USBLINK_TX", 100, NULL, 5, NULL);	/*����usb�������񣬶���max*/
	//	xTaskCreate(usblinkRxTask, "USBLINK_RX", 100, NULL, 5, NULL);	/*����usb��������*/

	xTaskCreate(vCtrldataTask, "COMMANDER", 100, NULL, 4, NULL); /*�����ɿ�ָ�������10*/

	xTaskCreate(vFdbkdataTask, "DATA_PROCESS", 100, NULL, 4, NULL); /*��������ͨ�����ݴ�������1*/

	xTaskCreate(keyTask, "BUTTON_SCAN", 100, NULL, 3, NULL); /*��������ɨ������20*/

	xTaskCreate(displayTask, "DISPLAY", 200, NULL, 1, NULL); /*������ʾ����50*/

	xTaskCreate(configParamTask, "CONFIG_TASK", 100, NULL, 1, NULL); /*����������������1000*/

	vTaskDelete(startTaskHandle); /*ɾ����ʼ����*/

	taskEXIT_CRITICAL(); /*�˳��ٽ���*/
}
