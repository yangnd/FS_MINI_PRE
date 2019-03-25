#ifndef __TASK_FDBKDATA_H
#define __TASK_FDBKDATA_H
#include "sys.h"
#include "config_param.h"
#include <stdbool.h>
/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 飞控通讯协议格式代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/


#define MAX_DATA_SIZE 32

/*通讯反馈数据结构*/
typedef struct
{
	u8 CarStatus;
	u8 MotoSel;
	u8 MotoId;
	u8 BreakMode;
	u8 Break;
	u8 Reserve1;
	u8 Reserve2;
	u8 Reserve3;
	//CAN ID=10F81X9A
	u8 Gear;
	u8 ThrottleL;
	u8 ThrottleH;
	u8 MotoMode;
	u8 RPML;
	u8 RPMH;
	u8 MotoTemp;
	u8 DriverTemp;
	//CAN ID=10F81X8D
	u8 VoltL;
	u8 VoltH;
	u8 CurrentL;
	u8 CurrentH;
	u8 DistanceL;
	u8 DistanceH;
	u8 ErrCodeL;
	u8 ErrCodeH;
	//CAN ID=10F81X7B
	u8 TorqueCtrlL;
	u8 TorqueCtrlH;
	u8 RPMCtrlL;
	u8 RPMCtrlH;
	u8 TorqueL;
	u8 TorqueH;
	u8 CanReserved1;
	u8 CanReserved2;
}fdbkData;

//电机数据
typedef struct
{
	enum motoGear Gear;
	u8 Throttle;
	u8 Mode;
	u16 RPM;
	u8 MotoTemp;
	u8 DriverTemp;
	float Voltage;
	float Current;
	float Distance;
	u8 ErrorCode;
	float TorqueCtrl;
	u16 RPMCtrl;
	float Torque;
}motoData;


void vFdbkdataTask(void *param);

motoData getMotoData(enum motoID motoid);

#endif /*FeedbackData_H*/

