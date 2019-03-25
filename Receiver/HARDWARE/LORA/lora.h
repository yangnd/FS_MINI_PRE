#ifndef _LORA_H_
#define _LORA_H_

#include "sys.h"
#include "lora_cfg.h"

/************************************************
FS 2018-08-25
************************************************/

#define LORA_AUX  PAin(4)    //LORA模块状态引脚
#define LORA_MD0  PAout(15)  //LORA模块控制引脚

extern _LoRa_CFG LoRa_CFG;
extern u8 Lora_mode;

u8 LoRa_Init(void);
void Aux_Int(u8 mode);
u8* lora_check_cmd(u8 *str);
u8 lora_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
void LoRa_Set(void);
void LoRa_SendData(u8 *txbuf);
u8 LoRa_ReceData(u8 *rxbuf,u16 *len);
void lora_setIterruptCallback(void (*cb)(void));


#endif

