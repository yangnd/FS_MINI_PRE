#include "stm32f10x.h"
#include "task_can.h"
#include "task_radio.h"
#include "can.h"
#include "led.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

u32 uCanRxID;
u8 uCanRxBuf[8];
u8 uCanRxState;
//u8 uCanTxBuf[8] = {0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00}; //空挡，油门低字节，油门高字节，油门控制，转矩低字节，转矩高字节，转速低字节，转速高字节
static canData canTX =
	{
		.Gear = 0x00,
		.ThrottleL = 0x00,
		.ThrottleH = 0x00,
		.Mode = 0x20,
		.TorqueL = 0x00,
		.TorqueH = 0x00,
		.SpeedOrBreakL = 0x00,
		.SpeedOrBreakH = 0x00,
};
static bool connectStatus = false;
extern ctrlData carCtrlData;
extern fdbkData carFdbkDataF, carFdbkDataR;

void vCanSendTask(void *param)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	CAN_Mode_Init(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_9tq, 8, CAN_Mode_Normal); //CAN初始化正常模式,波特率250Kbps
	while (1)
	{
		connectStatus = radioConnectStatus();
		canTX.Gear = carCtrlData.Gear;
		if (connectStatus)
		{
			canTX.ThrottleL = carCtrlData.Throttle;
			canTX.ThrottleH = 0;
		}
		else
		{
			canTX.ThrottleL = 0;
			canTX.ThrottleH = 0;
		}
//		if (carCtrlData.ControlMode == THROTTLE)
//			canTX.Mode = 0x20;
//		else if (carCtrlData.ControlMode == TORQUE)
//			canTX.Mode = 0x22;
//		else if (carCtrlData.ControlMode == SPEED)
//			canTX.Mode = 0x02;
		if ((carCtrlData.MotoSel == FRONT_ONLY) || (carCtrlData.MotoSel == FRONT_REAR))
		{
			canTX.Mode = 0x28;		//前后电机方向相反
			Can_Send_Msg(MOTO_F_CANID1, (u8 *)&canTX, 8);
		}
		if ((carCtrlData.MotoSel == REAR_ONLY) || (carCtrlData.MotoSel == FRONT_REAR))
		{
			canTX.Mode = 0x20;		//前后电机方向相反
			Can_Send_Msg(MOTO_R_CANID1, (u8 *)&canTX, 8);
		}
		vTaskDelayUntil(&xLastWakeTime, 50);
	}
}
void vCanReceiveTask(void *param)
{
	carFdbkDataF.MotoId = MOTO_FRONT;
	carFdbkDataR.MotoId = MOTO_REAR;
	static u8 t = 0;
	while (1)
	{
		vTaskDelay(5);
		if (Can_Receive_Msg(uCanRxBuf, &uCanRxID))
		{
			//To Do
			uCanRxState = 1;
			t++;
			if (t == 60)
			{
				LED0 = !LED0;
				t = 0;
			}

			switch (uCanRxID)
			{
			//Front电机反馈信息打包
			case MOTO_F_CANID2:
				carFdbkDataF.Gear = uCanRxBuf[0];
				carFdbkDataF.ThrottleL = uCanRxBuf[1];
				carFdbkDataF.ThrottleH = uCanRxBuf[2];
				carFdbkDataF.MotoMode = uCanRxBuf[3];
				carFdbkDataF.RPML = uCanRxBuf[4];
				carFdbkDataF.RPMH = uCanRxBuf[5];
				carFdbkDataF.MotoTemp = uCanRxBuf[6] - 40;
				carFdbkDataF.DriverTemp = uCanRxBuf[7] - 40;
				break;
			case MOTO_F_CANID3:
				carFdbkDataF.VoltL = uCanRxBuf[0];
				carFdbkDataF.VoltH = uCanRxBuf[1];
				carFdbkDataF.CurrentL = uCanRxBuf[2];
				carFdbkDataF.CurrentH = uCanRxBuf[3];
				carFdbkDataF.DistanceL = uCanRxBuf[4];
				carFdbkDataF.DistanceH = uCanRxBuf[5];
				carFdbkDataF.ErrCodeL = uCanRxBuf[6];
				carFdbkDataF.ErrCodeH = uCanRxBuf[7];
				break;
			case MOTO_F_CANID4:
				carFdbkDataF.TorqueCtrlL = uCanRxBuf[0];
				carFdbkDataF.TorqueCtrlH = uCanRxBuf[1];
				carFdbkDataF.RPMCtrlL = uCanRxBuf[2];
				carFdbkDataF.RPMCtrlH = uCanRxBuf[3];
				carFdbkDataF.TorqueL = uCanRxBuf[4];
				carFdbkDataF.TorqueH = uCanRxBuf[5];
				carFdbkDataF.CanReserved1 = uCanRxBuf[6];
				carFdbkDataF.CanReserved2 = uCanRxBuf[7];
				break;
			//Front电机反馈信息打包
			case MOTO_R_CANID2:
				carFdbkDataR.Gear = uCanRxBuf[0];
				carFdbkDataR.ThrottleL = uCanRxBuf[1];
				carFdbkDataR.ThrottleH = uCanRxBuf[2];
				carFdbkDataR.MotoMode = uCanRxBuf[3];
				carFdbkDataR.RPML = uCanRxBuf[4];
				carFdbkDataR.RPMH = uCanRxBuf[5];
				carFdbkDataR.MotoTemp = uCanRxBuf[6] - 40;
				carFdbkDataR.DriverTemp = uCanRxBuf[7] - 40;
				break;
			case MOTO_R_CANID3:
				carFdbkDataR.VoltL = uCanRxBuf[0];
				carFdbkDataR.VoltH = uCanRxBuf[1];
				carFdbkDataR.CurrentL = uCanRxBuf[2];
				carFdbkDataR.CurrentH = uCanRxBuf[3];
				carFdbkDataR.DistanceL = uCanRxBuf[4];
				carFdbkDataR.DistanceH = uCanRxBuf[5];
				carFdbkDataR.ErrCodeL = uCanRxBuf[6];
				carFdbkDataR.ErrCodeH = uCanRxBuf[7];
				break;
			case MOTO_R_CANID4:
				carFdbkDataR.TorqueCtrlL = uCanRxBuf[0];
				carFdbkDataR.TorqueCtrlH = uCanRxBuf[1];
				carFdbkDataR.RPMCtrlL = uCanRxBuf[2];
				carFdbkDataR.RPMCtrlH = uCanRxBuf[3];
				carFdbkDataR.TorqueL = uCanRxBuf[4];
				carFdbkDataR.TorqueH = uCanRxBuf[5];
				carFdbkDataR.CanReserved1 = uCanRxBuf[6];
				carFdbkDataR.CanReserved2 = uCanRxBuf[7];
				break;
			default:
				break;
			}
		}
	}
}

u16 getRPM(void)
{
	u16 uCarRPM;
	if (carCtrlData.MotoSel == FRONT_ONLY)
	{
		uCarRPM = (carFdbkDataF.RPMH << 8) + carFdbkDataF.RPML;
	}
	else if (carCtrlData.MotoSel == REAR_ONLY)
	{
		uCarRPM = (carFdbkDataR.RPMH << 8) + carFdbkDataR.RPML;
	}
	else if (carCtrlData.MotoSel == FRONT_REAR)
	{
		uCarRPM = ((carFdbkDataF.RPMH << 8) + carFdbkDataF.RPML + (carFdbkDataR.RPMH << 8) + carFdbkDataR.RPML) / 2;
	}

	return uCarRPM;
}
