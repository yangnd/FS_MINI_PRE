#include "task_upload.h"
#include "task_brake_servo.h"
#include "lora.h"
#include "usart3.h"
#include "led.h"
#include "delay.h"

/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

uploadData uploaddata;
extern ctrlData carCtrlData;
extern fdbkData carFdbkDataF, carFdbkDataR;
static u16 len;

void vUploadTask(void *param)		//DMA方式
{
	LoRa_Set();
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	USART3_DMA1_2_Init((u8*)&uploaddata);
	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime,200);
		Lora_mode=2;//进入发送模式
		uploaddata.cData=carCtrlData;
		//Front Moto
		uploaddata.F_Gear=carFdbkDataF.Gear;
		uploaddata.F_ThrottleL=carFdbkDataF.ThrottleL;
		uploaddata.F_ThrottleH=carFdbkDataF.ThrottleH;
		uploaddata.F_MotoMode=carFdbkDataF.MotoMode;
		uploaddata.F_RPML=carFdbkDataF.RPML;
		uploaddata.F_RPMH=carFdbkDataF.RPMH;
		uploaddata.F_MotoTemp=carFdbkDataF.MotoTemp;
		uploaddata.F_DriverTemp=carFdbkDataF.DriverTemp;
		uploaddata.F_VoltL=carFdbkDataF.VoltL;
		uploaddata.F_VoltH=carFdbkDataF.VoltH;
		uploaddata.F_CurrentL=carFdbkDataF.CurrentL;
		uploaddata.F_CurrentH=carFdbkDataF.CurrentH;
		uploaddata.F_DistanceL=carFdbkDataF.DistanceL;
		uploaddata.F_DistanceH=carFdbkDataF.DistanceH;
		uploaddata.F_ErrCodeL=carFdbkDataF.ErrCodeL;
		uploaddata.F_ErrCodeH=carFdbkDataF.ErrCodeH;
		uploaddata.F_TorqueL=carFdbkDataF.TorqueL;
		uploaddata.F_TorqueH=carFdbkDataF.TorqueH;
		//Rear Moto
		uploaddata.R_Gear=carFdbkDataR.Gear;
		uploaddata.R_ThrottleL=carFdbkDataR.ThrottleL;
		uploaddata.R_ThrottleH=carFdbkDataR.ThrottleH;
		uploaddata.R_MotoMode=carFdbkDataR.MotoMode;
		uploaddata.R_RPML=carFdbkDataR.RPML;
		uploaddata.R_RPMH=carFdbkDataR.RPMH;
		uploaddata.R_MotoTemp=carFdbkDataR.MotoTemp;
		uploaddata.R_DriverTemp=carFdbkDataR.DriverTemp;
		uploaddata.R_VoltL=carFdbkDataR.VoltL;
		uploaddata.R_VoltH=carFdbkDataR.VoltH;
		uploaddata.R_CurrentL=carFdbkDataR.CurrentL;
		uploaddata.R_CurrentH=carFdbkDataR.CurrentH;
		uploaddata.R_DistanceL=carFdbkDataR.DistanceL;
		uploaddata.R_DistanceH=carFdbkDataR.DistanceH;
		uploaddata.R_ErrCodeL=carFdbkDataR.ErrCodeL;
		uploaddata.R_ErrCodeH=carFdbkDataR.ErrCodeH;
		uploaddata.R_TorqueL=carFdbkDataR.TorqueL;
		uploaddata.R_TorqueH=carFdbkDataR.TorqueH;
		uploaddata.res1=0xFF;
		uploaddata.res2=0xFF;
		uploaddata.fDec=getDec();	//8.25 41040000
		len=sizeof(uploaddata);
		DMA_SetCurrDataCounter(DMA1_Channel2, len); //Lora需<=53
		DMA_Cmd(DMA1_Channel2, ENABLE);
//		LED0=!LED0;
	}
}

//void vUploadTask(void *param)		//一般方式
//{
//	while(1)
//	{
//		vTaskDelay(500);
//		if(!LORA_AUX)
//		{
//			LoRa_SendData(upload_buf); //发送一次数据
//			LED0=!LED0;
//		}
//	}	
//}
