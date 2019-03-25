#ifndef __RADIO_LINK_H
#define __RADIO_LINK_H
#include <stdbool.h>
#include "sys.h"
#include "task_ctrldata.h"
#include "task_fdbkdata.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"


/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 无线通信驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/


extern xTaskHandle radiolinkTaskHandle;

void radiolinkInit(void);
bool radioSendPacket(const ctrlData *p);
bool radioSendPacketBlocking(const ctrlData *p);
bool radioReceivePacket(fdbkData *p);
bool radioReceivePacketBlocking(fdbkData *p);
void vRadioTask(void* param);
u16 radioFailRxcount(void);
u16 radioTxcount(void);
bool radioConnectStatus(void);
void radioEnable(FunctionalState state);
ctrlData getCMD(void);

#endif /*RADIO_LINK_H*/
