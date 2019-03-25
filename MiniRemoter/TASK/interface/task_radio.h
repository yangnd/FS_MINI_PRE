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
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 * ALIENTEK MiniFly_Remotor
 * ����ͨ����������	
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2018/6/1
 * �汾��V1.0
 * ��Ȩ���У�����ؾ���
 * Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
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
