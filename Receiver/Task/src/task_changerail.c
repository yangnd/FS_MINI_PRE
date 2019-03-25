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

/*RS485外部中断回调函数*/
static void rs485_interruptCallback(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(rs485rxIT, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //如果需要的话进行一次任务切换
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
		if(keyState==KEY0_SHORT_PRESS)	railState=LEFTRAIL;			//置轮子处于轨道状态
		else if(keyState==KEY1_SHORT_PRESS)	railState=RIGTHRAIL;
		
		uRail=getRail();
		if((uRail==2)&&(railState==LEFTRAIL))//正转270度
		{
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x47;		//Pn071
			rs485txbuf[4]=0x7F;		//内部位置0
			rs485txbuf[5]=0xFF;		//低位
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x46;	//Pn070
			rs485txbuf[4]=0x7F;
			rs485txbuf[5]=0xFE;	//Son使能驱动器
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x47;	//Pn071
			rs485txbuf[4]=0x7B;	//内部位置0,触发
			rs485txbuf[5]=0xFF;
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
//			rs485txbuf[2]=0x00;
//			rs485txbuf[3]=0x46;	//Pn070
//			rs485txbuf[4]=0x7F;
//			rs485txbuf[5]=0xFF;	//Son失能驱动器
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			railState=RIGTHRAIL;
		}
		else if((uRail==1)&&(railState==RIGTHRAIL))//反转270度
		{
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x47;	//Pn071
			rs485txbuf[4]=0x7E;	//内部位置1
			rs485txbuf[5]=0xFF;
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x46;	//Pn070
			rs485txbuf[4]=0x7F;
			rs485txbuf[5]=0xFE;	//Son使能驱动器
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x47;	//Pn071
			rs485txbuf[4]=0x7A;	//内部位置1,触发
			rs485txbuf[5]=0xFF;
			ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen);
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
//			rs485txbuf[2]=0x00;
//			rs485txbuf[3]=0x46;	//Pn070
//			rs485txbuf[4]=0x7F;
//			rs485txbuf[5]=0xFF;	//Son失能驱动器
//			while(ModbusWriteSReg(rs485txbuf,8,rs485rxbuf,rxlen));
			railState=LEFTRAIL;
		}
	}
}
