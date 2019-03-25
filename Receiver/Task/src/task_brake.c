#include "task_brake.h"
#include "task_ctrldata.h"
#include "task_can.h"
#include "brake.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

#define PI 3.14159265358979323846264338327950288419716939937510 //���峣��PI
//#define DM 0.4													//����ֱ��
#define DELTA 1.0												//���ٶȲ���ֵ
#define AMAX 10.0												//�����ٶ�10r/s2,ÿ��ת�ٽ���600r/min
#define BRAKETIME 100
u8 uBrake;														//ɲ���ź�
u16 uRPM,uLastRPM;												//����ת��
s16 sDeltaRPM,sBrake;
float fDecTarget,fDecNow;

void vBrakeTask(void *param)
{
	BRAKE_EN = 1;
	BRAKE_IN1 = 0; 
	BRAKE_IN2 = 0;
	while (1)
	{	
		vTaskDelay(BRAKETIME);
		uBrake = getBrake();
		fDecTarget = uBrake*AMAX/100;				//�����趨���ٶ�ֵΪɲ���źŵ�ʮ��֮һ
		uRPM = getRPM();									//ת��
		fDecNow=((float)uRPM-uLastRPM)/(60*BRAKETIME/1000);
		uLastRPM=uRPM;
		
		if (uBrake>5)						//�յ�ɲ���ź�
		{
			BRAKE_EN = 1; 					//ʹ��ɲ��
			if (uRPM > 0)
			{
				
				if ((-fDecNow) <fDecTarget-DELTA) //ʵ�ʼ��ٶ����趨���ٶ�֮������趨��ֵ
				{
					BRAKE_IN2 = 0; 				//����ɲ��
					BRAKE_IN1 = 1;
				}
//				else if ((-fDecNow) >fDecTarget+DELTA) 	//ʵ�ʼ��ٶ����趨���ٶ�֮����趨��ֵ
//				{
//					BRAKE_IN1 = 0; 			//�ɿ�ɲ��
//					BRAKE_IN2 = 1;
//				}
				else if ((fDecTarget-DELTA <=( -fDecNow) ) && ((-fDecNow )<= fDecTarget+DELTA)) //ʵ�ʼ��ٶ����趨���ٶ�֮�����趨��ֵ��Χ��
				{
					BRAKE_IN1 = 0; 				//����ɲ��λ��
					BRAKE_IN2 = 0;
				}
			}
		}
		else
		{
			BRAKE_IN1=0;
			BRAKE_IN2=1;
		}
	}
}
