#include <string.h>
#include "task_fdbkdata.h"
#include "task_ctrldata.h"
#include "task_beep.h"
#include "task_radio.h"
#include "config_param.h"
#include "trim_ui.h"
#include "main_ui.h"
#include "match_ui.h"


/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
/********************************************************************************	 
 * All rights reserved
********************************************************************************/


fdbkData carFdbkData,fdbkMotoF,fdbkMotoR;
u8 uCarBreak;
float fCarBattery;
/*返回数据解析*/
static void dataAnalyze(fdbkData *p)
{
//	if(p->MotoId==MOTO_FRONT)
//	{
//		uMoto_F_Gear=p->Gear;
//		uMoto_F_Throttle=p->ThrottleL+(p->ThrottleH<<8);
//		uMoto_F_RPM=p->RPMCtrlL+(p->RPMCtrlH<<8);
//	}
//	else if(p->MotoId==MOTO_REAR)
//	{
//		uMoto_R_Gear=p->Gear;
//		uMoto_R_Throttle=p->ThrottleL+(p->ThrottleH<<8);
//		uMoto_R_RPM=p->RPMCtrlL+(p->RPMCtrlH<<8);
//	}
	uCarBreak=p->Break;
	fCarBattery=(float)(p->VoltL+(p->VoltH<<8))/10;
}
/*无线连接数据处理任务*/
void vFdbkdataTask(void *param) 
{
	
	while(1)
	{
		radioReceivePacketBlocking(&carFdbkData);
		dataAnalyze(&carFdbkData);
		if(carFdbkData.MotoId==MOTO_FRONT) fdbkMotoF=carFdbkData;
		else if(carFdbkData.MotoId==MOTO_REAR) fdbkMotoR=carFdbkData;
		vTaskDelay(1);
	}
}

motoData getMotoData(enum motoID motoid)
{
	motoData motodata;
	if(motoid==MOTO_FRONT)
	{
		motodata.Gear=(enum motoGear)fdbkMotoF.Gear;
		motodata.Throttle=(fdbkMotoF.ThrottleH<<8)+fdbkMotoF.ThrottleL;
		if(fdbkMotoF.MotoMode==0x20) motodata.Mode='T';
		else if(fdbkMotoF.MotoMode==0x22) motodata.Mode='Q';
		else if(fdbkMotoF.MotoMode==0x02) motodata.Mode='S';
		motodata.RPM=(fdbkMotoF.RPMH<<8)+fdbkMotoF.RPML;
		motodata.MotoTemp=fdbkMotoF.MotoTemp;
		motodata.DriverTemp=fdbkMotoF.DriverTemp;
		motodata.Voltage=(float)((fdbkMotoF.VoltH<<8)+fdbkMotoF.VoltL)*0.1;
		motodata.Current=(float)((fdbkMotoF.CurrentH<<8)+fdbkMotoF.CurrentL)*0.1;
		motodata.Distance =(float)((fdbkMotoF.DistanceH<<8)+fdbkMotoF.DistanceL)*0.1;
		motodata.ErrorCode=(fdbkMotoF.ErrCodeH<<8)+fdbkMotoF.ErrCodeL;
		motodata.TorqueCtrl=(float)((fdbkMotoF.TorqueCtrlH<<8)+fdbkMotoF.TorqueCtrlL)*0.1;
		motodata.RPMCtrl=(fdbkMotoF.RPMCtrlH<<8)+fdbkMotoF.RPMCtrlL;
		motodata.Torque=(float)((fdbkMotoF.TorqueH<<8)+fdbkMotoF.TorqueL)*0.1;
	}
	else if(motoid==MOTO_REAR)
	{
		motodata.Gear=(enum motoGear)fdbkMotoR.Gear;
		motodata.Throttle=(fdbkMotoR.ThrottleH<<8)+fdbkMotoR.ThrottleL;
		if(fdbkMotoR.MotoMode==0x20) motodata.Mode='T';
		else if(fdbkMotoR.MotoMode==0x22) motodata.Mode='Q';
		else if(fdbkMotoR.MotoMode==0x02) motodata.Mode='S';
		motodata.RPM=(fdbkMotoR.RPMH<<8)+fdbkMotoR.RPML;
		motodata.MotoTemp=fdbkMotoR.MotoTemp;
		motodata.DriverTemp=fdbkMotoR.DriverTemp;
		motodata.Voltage=(float)((fdbkMotoR.VoltH<<8)+fdbkMotoR.VoltL)*0.1;
		motodata.Current=(float)((fdbkMotoR.CurrentH<<8)+fdbkMotoR.CurrentL)*0.1;
		motodata.Distance =(float)((fdbkMotoR.DistanceH<<8)+fdbkMotoR.DistanceL)*0.1;
		motodata.ErrorCode=(fdbkMotoR.ErrCodeH<<8)+fdbkMotoR.ErrCodeL;
		motodata.TorqueCtrl=(float)((fdbkMotoR.TorqueCtrlH<<8)+fdbkMotoR.TorqueCtrlL)*0.1;
		motodata.RPMCtrl=(fdbkMotoR.RPMCtrlH<<8)+fdbkMotoR.RPMCtrlL;
		motodata.Torque=(float)((fdbkMotoR.TorqueH<<8)+fdbkMotoR.TorqueL)*0.1;
	}
	return motodata;
}

