#include <string.h>
#include "task_ctrldata.h"
#include "task_fdbkdata.h"
#include "task_radio.h"
#include "main_ui.h"
#include "trim_ui.h"
#include "config_param.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

static fJoyStick  percent;
extern u8 uGear;

u8 limit(u8 value,u8 min, u8 max)
{
	if(value > max)
	{
		value = max;
	}
	else if(value < min)
	{
		value = min;
	}
	return value;
}

/*发送控制命令任务*/
void vCtrldataTask(void* param)
{
	ctrlData carCtrlData;
	u8 maxThrottle;
	while(1)
	{
		vTaskDelay(10);
		maxThrottle=configParam.car.maxthrottle;
		carCtrlData.MotoSel=configParam.car.motosel;
		carCtrlData.ControlMode=configParam.car.controlmode;
		if(uGear%2==0)	carCtrlData.Gear=NONE;
		else if(uGear==1)	carCtrlData.Gear=DRIVE;
		else if(uGear==3)	carCtrlData.Gear=REVERSE;
		else if(uGear==5)	carCtrlData.Gear=LOW;
		ADCtoCtrlDataPercent(&percent);		
		//Break
		carCtrlData.Break= percent.Break*100;
		carCtrlData.Break=limit(carCtrlData.Break,0,100);
		if(getRCLock()==false)
		{
			//Throttle
			if(carCtrlData.Break!=0)	carCtrlData.Throttle=0;		//刹车屏蔽油门
			else
			{
				carCtrlData.Throttle=percent.Throttle*maxThrottle;
				carCtrlData.Throttle=limit(carCtrlData.Throttle,0,maxThrottle);
			}
			carCtrlData.Rail=getRail();
		}
		else 
		{
			carCtrlData.Throttle=0;	//锁定
		}
		/*发送控制数据*/
		radioSendPacket(&carCtrlData);
	}
}

fJoyStick getPercent(void)
{
	return  percent;
}


