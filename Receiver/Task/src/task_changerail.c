#include "task_changerail.h"
#include "task_ctrldata.h"
#include "task_key.h"
#include "rs485.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static u8 rs485txbuf[8]; 
static u8 rs485rxbuf[8];
static u8 rxlen;
static u8 uRail;
static u8 railState;
static u8 keyState;
static xSemaphoreHandle rs485rxIT;

/*RS485�ⲿ�жϻص�����*/
static void rs485_interruptCallback(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(rs485rxIT, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //�����Ҫ�Ļ�����һ�������л�
}
void Modbus_Init(void)
{
	RS485_Init(9600);
	rs485rxIT = xSemaphoreCreateBinary();
	rs485_setIterruptCallback(rs485_interruptCallback);
}
void vChangeRailTask(void *param)
{
	keyState=getKeyState();
	if(keyState==KEY0_SHORT_PRESS)	railState=LEFTRAIL;
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	while(1)
	{
		vTaskDelay(50);
		
		keyState=getKeyState();
		if(keyState==KEY0_SHORT_PRESS)	railState=LEFTRAIL;			//�����Ӵ��ڹ��״̬
		else if(keyState==KEY1_SHORT_PRESS)	railState=RIGTHRAIL;
		
		uRail=getRail();
		if((uRail==2)&&(railState==LEFTRAIL))//��ת270��
		{
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x47;		//Pn071
			rs485txbuf[4]=0x7F;		//�ڲ�λ��0
			rs485txbuf[5]=0xFF;		//��λ
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x46;	//Pn070
			rs485txbuf[4]=0x7F;
			rs485txbuf[5]=0xFE;	//Sonʹ��������
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x47;	//Pn071
			rs485txbuf[4]=0x7B;	//�ڲ�λ��0,����
			rs485txbuf[5]=0xFF;
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
//			rs485txbuf[2]=0x00;
//			rs485txbuf[3]=0x46;	//Pn070
//			rs485txbuf[4]=0x7F;
//			rs485txbuf[5]=0xFF;	//Sonʧ��������
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			railState=RIGTHRAIL;
		}
		else if((uRail==1)&&(railState==RIGTHRAIL))//��ת270��
		{
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x47;	//Pn071
			rs485txbuf[4]=0x7E;	//�ڲ�λ��1
			rs485txbuf[5]=0xFF;
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x46;	//Pn070
			rs485txbuf[4]=0x7F;
			rs485txbuf[5]=0xFE;	//Sonʹ��������
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x47;	//Pn071
			rs485txbuf[4]=0x7A;	//�ڲ�λ��1,����
			rs485txbuf[5]=0xFF;
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
//			rs485txbuf[2]=0x00;
//			rs485txbuf[3]=0x46;	//Pn070
//			rs485txbuf[4]=0x7F;
//			rs485txbuf[5]=0xFF;	//Sonʧ��������
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			railState=LEFTRAIL;
		}
	}
}
