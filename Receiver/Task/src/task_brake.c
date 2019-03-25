#include "task_brake.h"
#include "task_ctrldata.h"
#include "task_can.h"
#include "brake.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

#define PI 3.14159265358979323846264338327950288419716939937510 //定义常量PI
//#define DM 0.4													//轮子直径
#define DELTA 1.0												//减速度差阈值
#define AMAX 10.0												//最大减速度10r/s2,每秒转速降低600r/min
#define BRAKETIME 100
u8 uBrake;														//刹车信号
u16 uRPM,uLastRPM;												//定义转速
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
		fDecTarget = uBrake*AMAX/100;				//定义设定减速度值为刹车信号的十分之一
		uRPM = getRPM();									//转速
		fDecNow=((float)uRPM-uLastRPM)/(60*BRAKETIME/1000);
		uLastRPM=uRPM;
		
		if (uBrake>5)						//收到刹车信号
		{
			BRAKE_EN = 1; 					//使能刹车
			if (uRPM > 0)
			{
				
				if ((-fDecNow) <fDecTarget-DELTA) //实际减速度与设定减速度之差低于设定阈值
				{
					BRAKE_IN2 = 0; 				//继续刹车
					BRAKE_IN1 = 1;
				}
//				else if ((-fDecNow) >fDecTarget+DELTA) 	//实际减速度与设定减速度之差超出设定阈值
//				{
//					BRAKE_IN1 = 0; 			//松开刹车
//					BRAKE_IN2 = 1;
//				}
				else if ((fDecTarget-DELTA <=( -fDecNow) ) && ((-fDecNow )<= fDecTarget+DELTA)) //实际减速度与设定减速度之差在设定阈值范围内
				{
					BRAKE_IN1 = 0; 				//保持刹车位置
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
