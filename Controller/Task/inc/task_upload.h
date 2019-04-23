#ifndef __TASK_UPLOAD_H
#define __TASK_UPLOAD_H

#include "task_ctrldata.h"
#include "task_fdbkdata.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

typedef struct
{
	ctrlData cData;	//6 bytes
	//Front Moto	//18 bytes
	u8 F_Gear;
	u8 F_ThrottleL;
	u8 F_ThrottleH;
	u8 F_MotoMode;
	u8 F_RPML;
	u8 F_RPMH;
	u8 F_MotoTemp;
	u8 F_DriverTemp;
	u8 F_VoltL;
	u8 F_VoltH;
	u8 F_CurrentL;
	u8 F_CurrentH;
	u8 F_DistanceL;
	u8 F_DistanceH;
	u8 F_ErrCodeL;
	u8 F_ErrCodeH;
	u8 F_TorqueL;
	u8 F_TorqueH;
	//Rear Moto		//18 bytes
	u8 R_Gear;
	u8 R_ThrottleL;
	u8 R_ThrottleH;
	u8 R_MotoMode;
	u8 R_RPML;
	u8 R_RPMH;
	u8 R_MotoTemp;
	u8 R_DriverTemp;
	u8 R_VoltL;
	u8 R_VoltH;
	u8 R_CurrentL;
	u8 R_CurrentH;
	u8 R_DistanceL;
	u8 R_DistanceH;
	u8 R_ErrCodeL;
	u8 R_ErrCodeH;
	u8 R_TorqueL;
	u8 R_TorqueH;
	//6+18+18=42, 加2 bytes补齐（对齐后的长度必须是成员中最大的对齐参数的整数倍）
	u8 res1;
	u8 res2;
	//加速度
	float fDec;	//4 bytes，共48 bytes
}uploadData;

void vUploadTask(void *param);
#endif 
