#ifndef __TASK_CAN_H
#define __TASK_CAN_H
#include "sys.h"

//MOTO F CAN ID
#define MOTO_F_CANID1	0x10F8E3F3
#define MOTO_F_CANID2	0x10F8139A
#define MOTO_F_CANID3	0x10F8138D
#define MOTO_F_CANID4	0x10F8137B
//MOTO R CAN ID
#define MOTO_R_CANID1	0x10F8E4F3
#define MOTO_R_CANID2	0x10F8149A
#define MOTO_R_CANID3	0x10F8148D
#define MOTO_R_CANID4	0x10F8147B

//Moto CAN控制数据结构
typedef struct
{
	u8 Gear;
	u8 ThrottleL;
	u8 ThrottleH;
	u8 Mode;
	u8 TorqueL;
	u8 TorqueH;
	u8 SpeedOrBreakL;
	u8 SpeedOrBreakH;
}canData;

//void vCAN_FilterSet(u32 ExtId, u32 mask);
void vCanSendTask(void *param);
void vCanReceiveTask(void *param);
u16 getRPM(void);

#endif
