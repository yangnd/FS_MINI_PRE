#include "task_usart.h"
#include "task_ctrldata.h"
#include "task_radio.h"
#include "led.h"
#include "usart.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

u8 USART_TX_BUF[64];
extern u8 rx_buf[100];
extern u16 rx_len;

//u8 *data;//数据起始地址，用于计算 CRC 值
//u8 length; //数据长度
//返回 unsigned integer 类型的 CRC 值。
u16 crc_chk(u8 *data, u8 length)
{
	u8 j;
	u16 crc_reg = 0xFFFF;
	while (length--)
	{
		crc_reg ^= *data++;
		for (j = 0; j < 8; j++)
		{
			if (crc_reg & 0x01)
			{
				crc_reg = (crc_reg >> 1) ^ 0xA001;
			}
			else
			{
				crc_reg = crc_reg >> 1;
			}
		}
	}
	return crc_reg;
}
void usart_send_frame(u8 sel,u16 frame_count) 
{
	u16 len;
	u8 i=0;
	u16 crc;
	USART_TX_BUF[0]=0x22;
	USART_TX_BUF[1]=0x33;
	USART_TX_BUF[2]=0x44;
	USART_TX_BUF[3]=0x55;
	USART_TX_BUF[4]=frame_count>>8;
	USART_TX_BUF[5]=frame_count&0xFF;
	if(sel==0)
	{
		USART_TX_BUF[6]=0xAA;
	}
	else
	{
		USART_TX_BUF[6]=0xBB;
	}
	len=rx_len;
	USART_TX_BUF[7]=len;
	for(i=0;i<len;i++)
		USART_TX_BUF[8+i]=rx_buf[i];
	USART_TX_BUF[8+len]=0x00;
	crc=crc_chk(USART_TX_BUF,9+len);
	USART_TX_BUF[9+len]=crc>>8;
	USART_TX_BUF[10+len]=crc&0xFF;
	USART_ClearFlag( USART1, USART_FLAG_TC );
//	for(i=0;i<9+len;i++)	//不含crc：9+len，含crc：11+len
//	{
//		USART_SendData(USART1, *(USART_TX_BUF+i));//向串口1发送数据
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//	}
	DMA_SetCurrDataCounter(DMA1_Channel4, 9+len); 
    DMA_Cmd(DMA1_Channel4, ENABLE);
}

void vUsartTask(void *param)
{
	static u16 frame_count=0;
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	DMA1_4_Init(USART_TX_BUF);
	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime,100);
		LED1=!LED1;
		usart_send_frame(0,frame_count);
		frame_count++;
//		printf("\n\r");
//		printf("\n\r");
	}
}





