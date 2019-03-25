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
	delay_ms(3000);
	KEY_Init();																  //������ʼ��
	LED_Init();																  //LED�Ƴ�ʼ��
	BEEP_Init();															  //Beep��ʼ��
	RS485_Init(115200);															//rs485��ʼ��
	BrakeCb_Init();
	RailCb_Init();
	CAN_Mode_Init(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_9tq, 8, CAN_Mode_Normal); //CAN��ʼ������ģʽ,������250Kbps
	radiolinkInit();														  //�������ģ�鲢��ʼ��
	LED0 = 0;
	LED1 = 0;
	while(LoRa_Init());
	LoRa_Set();
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
//	xTaskCreate(vBrakeTask, "Brake", 100, NULL, 8, NULL);			/*����brake����*/
	xTaskCreate(vBrakeServoTask, "BrakeServer", 100, NULL, 8, NULL);/*����brakeServer����*/	
	xTaskCreate(vChangeRailTask, "RailChange", 100, NULL, 7, NULL);	/*����Change Rail����*/
	xTaskCreate(vCanSendTask, "CanSend", 100, NULL, 6, NULL);		/*����CAN��������*/
	xTaskCreate(vCanReceiveTask, "CanReceive", 100, NULL, 5, NULL); /*����CAN��������*/

	xTaskCreate(vRadioTask, "Radio", 100, NULL, 4, NULL);		/*����Radio����*/
	xTaskCreate(vCtrldataTask, "CtrlData", 100, NULL, 3, NULL); /*����CtrlData��������*/
	xTaskCreate(vFdbkdataTask, "FdbkData", 100, NULL, 3, NULL); /*����FdbkData�������*/

	xTaskCreate(vKeyTask, "Key", 100, NULL, 2, NULL);   /*��������ɨ������*/
	xTaskCreate(vUploadTask,"Upload",100,NULL,2,NULL);
	xTaskCreate(vBeepTask, "Beep", 100, NULL, 1, NULL); /*����beep����*/

	vTaskDelete(startTaskHandle); /* ɾ����ʼ���� */

	taskEXIT_CRITICAL(); /*�˳��ٽ���*/
}

/**** Copyright (C)2018 Feisuo. All Rights Reserved **** END OF FILE ****/
