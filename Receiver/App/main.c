/**
  ********************************  STM32F10x  *********************************
  * @�ļ���     �� main.c
  * @����       �� 
  * @��汾     �� V3.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2018��9��10��
  * @ժҪ       �� ������
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2018-09-10 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
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
�������� �� main
��    �� �� ���������
��    �� �� ��
�� �� ֵ �� int
��    �� �� 
*************************************************/
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);							  /*�ж����ó�ʼ��*/
	delay_init();
	uart_init(115200);
	KEY_Init();																  //������ʼ��
	LED_Init();																  //LED�Ƴ�ʼ��
	BEEP_Init();															  //Beep��ʼ��

	LED0 = 0;
	LED1 = 0;
	while(LoRa_Init());
	LED0=1;
	LED1=0;
	lora_task_init();
	LED0 = 1;
	LED1 = 1;

	xTaskCreate(startTask, "START_TASK", 100, NULL, 2, &startTaskHandle); /*������ʼ����*/
	vTaskStartScheduler();												  /*�����������*/

	while (1)
	{
	}; /* ������Ⱥ󲻻�ִ�е��� */
}

/*��������*/
void startTask(void *param)
{
	taskENTER_CRITICAL();											/*�����ٽ���*/

	xTaskCreate(vUsartTask, "USART", 100, NULL, 4, NULL);		/*����Radio����*/
	xTaskCreate(vLoraTask,"Lora",100,NULL,3,NULL);
	xTaskCreate(vKeyTask, "Key", 100, NULL, 2, NULL);   /*��������ɨ������*/
//	xTaskCreate(vBeepTask, "Beep", 100, NULL, 1, NULL); /*����beep����*/

	vTaskDelete(startTaskHandle); /* ɾ����ʼ���� */

	taskEXIT_CRITICAL(); /*�˳��ٽ���*/
}

/**** Copyright (C)2018 Feisuo. All Rights Reserved **** END OF FILE ****/
