#ifndef __RS485_H
#define __RS485_H			 
#include "sys.h"	 								  
//////////////////////////////////////////////////////////////////////////////////	 
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
	  		  	
extern u8 RS485_RX_BUF[64]; 		//���ջ���,���64���ֽ�
extern u8 RS485_RX_CNT;   			//���յ������ݳ���

//ģʽ����
#define RS485_TX_EN		PDout(7)	//485ģʽ����.0,����;1,����.
//����봮���жϽ��գ��벻Ҫע�����º궨��
#define EN_USART2_RX 	1			//0,������;1,����.

#define MODBUS_TIME 100				//Modbus��ʱʱ��




void RS485_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);
u8 RS485_Receive_Data(u8 *buf,u8 *len);
u16 crc_chk(u8 *data,u8 length);
void ModbusWriteSReg(u8 *txbuf, u8 txlen);
/*����RS485�жϻص�����*/
void Brake_setIterruptCallback(void (*cb)(void));
void Rail_setIterruptCallback(void (*cb)(void));

#endif
