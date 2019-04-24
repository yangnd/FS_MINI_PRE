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
extern fdbkData carFdbkData1, carFdbkData2;

void vCanSendTask(void *param)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
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
		if (carCtrlData.ControlMode == THROTTLE)
			canTX.Mode = 0x20;
		else if (carCtrlData.ControlMode == TORQUE)
			canTX.Mode = 0x22;
		else if (carCtrlData.ControlMode == SPEED)
			canTX.Mode = 0x02;
		if ((carCtrlData.MotoSel == FRONT_ONLY) || (carCtrlData.MotoSel == FRONT_REAR))
		{
			Can_Send_Msg(MOTO_F_CANID1, (u8 *)&canTX, 8);
		}
		if ((carCtrlData.MotoSel == REAR_ONLY) || (carCtrlData.MotoSel == FRONT_REAR))
		{
			Can_Send_Msg(MOTO_R_CANID1, (u8 *)&canTX, 8);
		}
		vTaskDelayUntil(&xLastWakeTime, 50);
	}
}
void vCanReceiveTask(void *param)
{
	carFdbkData1.MotoId = MOTO_FRONT;
	carFdbkData2.MotoId = MOTO_REAR;
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
				carFdbkData1.Gear = uCanRxBuf[0];
				carFdbkData1.ThrottleL = uCanRxBuf[1];
				carFdbkData1.ThrottleH = uCanRxBuf[2];
				carFdbkData1.MotoMode = uCanRxBuf[3];
				carFdbkData1.RPML = uCanRxBuf[4];
				carFdbkData1.RPMH = uCanRxBuf[5];
				carFdbkData1.MotoTemp = uCanRxBuf[6] - 40;
				carFdbkData1.DriverTemp = uCanRxBuf[7] - 40;
				break;
			case MOTO_F_CANID3:
				carFdbkData1.VoltL = uCanRxBuf[0];
				carFdbkData1.VoltH = uCanRxBuf[1];
				carFdbkData1.CurrentL = uCanRxBuf[2];
				carFdbkData1.CurrentH = uCanRxBuf[3];
				carFdbkData1.DistanceL = uCanRxBuf[4];
				carFdbkData1.DistanceH = uCanRxBuf[5];
				carFdbkData1.ErrCodeL = uCanRxBuf[6];
				carFdbkData1.ErrCodeH = uCanRxBuf[7];
				break;
			case MOTO_F_CANID4:
				carFdbkData1.TorqueCtrlL = uCanRxBuf[0];
				carFdbkData1.TorqueCtrlH = uCanRxBuf[1];
				carFdbkData1.RPMCtrlL = uCanRxBuf[2];
				carFdbkData1.RPMCtrlH = uCanRxBuf[3];
				carFdbkData1.TorqueL = uCanRxBuf[4];
				carFdbkData1.TorqueH = uCanRxBuf[5];
				carFdbkData1.CanReserved1 = uCanRxBuf[6];
				carFdbkData1.CanReserved2 = uCanRxBuf[7];
				break;
			//Front电机反馈信息打包
			case MOTO_R_CANID2:
				carFdbkData2.Gear = uCanRxBuf[0];
				carFdbkData2.ThrottleL = uCanRxBuf[1];
				carFdbkData2.ThrottleH = uCanRxBuf[2];
				carFdbkData2.MotoMode = uCanRxBuf[3];
				carFdbkData2.RPML = uCanRxBuf[4];
				carFdbkData2.RPMH = uCanRxBuf[5];
				carFdbkData2.MotoTemp = uCanRxBuf[6] - 40;
				carFdbkData2.DriverTemp = uCanRxBuf[7] - 40;
				break;
			case MOTO_R_CANID3:
				carFdbkData2.VoltL = uCanRxBuf[0];
				carFdbkData2.VoltH = uCanRxBuf[1];
				carFdbkData2.CurrentL = uCanRxBuf[2];
				carFdbkData2.CurrentH = uCanRxBuf[3];
				carFdbkData2.DistanceL = uCanRxBuf[4];
				carFdbkData2.DistanceH = uCanRxBuf[5];
				carFdbkData2.ErrCodeL = uCanRxBuf[6];
				carFdbkData2.ErrCodeH = uCanRxBuf[7];
				break;
			case MOTO_R_CANID4:
				carFdbkData2.TorqueCtrlL = uCanRxBuf[0];
				carFdbkData2.TorqueCtrlH = uCanRxBuf[1];
				carFdbkData2.RPMCtrlL = uCanRxBuf[2];
				carFdbkData2.RPMCtrlH = uCanRxBuf[3];
				carFdbkData2.TorqueL = uCanRxBuf[4];
				carFdbkData2.TorqueH = uCanRxBuf[5];
				carFdbkData2.CanReserved1 = uCanRxBuf[6];
				carFdbkData2.CanReserved2 = uCanRxBuf[7];
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
		uCarRPM = (carFdbkData1.RPMH << 8) + carFdbkData1.RPML;
	}
	else if (carCtrlData.MotoSel == REAR_ONLY)
	{
		uCarRPM = (carFdbkData2.RPMH << 8) + carFdbkData2.RPML;
	}
	else if (carCtrlData.MotoSel == FRONT_REAR)
	{
		uCarRPM = ((carFdbkData1.RPMH << 8) + carFdbkData1.RPML + (carFdbkData2.RPMH << 8) + carFdbkData2.RPML) / 2;
	}

	return uCarRPM;
}
