#include "task_brake_servo.h"
#include "rs485.h"
#include "task_ctrldata.h"
#include "task_can.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define PI 3.141592654				  //定义常量PI
#define DM 0.375					  //轮子直径
#define KM 3.285714					  //速比
#define DELTA 1.0					  //减速度差阈值
#define AMAX 10.0					  //最大减速度10m/s2
#define BRAKETIME 10				  //刹车计算时间
#define MAXSTEP 100					  //刹车行程总步数
static u8 uBrake = 0, uLastBrake = 0; //刹车信号
static s16 sTorque = 0, sLastTorque = 0;
static s16 sSpeed = -2500; //送刹车转速
u16 uRPM, uLastRPM;		   //定义转速
s16 sDeltaRPM, sBrake;
float fDecTarget, fDecNow;
static u8 rs485txbuf[8];
static u16 rx_crc, cal_crc;
static s8 servo_step = 0; //标记伺服位置
static s32 pulseCount;	  //刹车位置与伺服当前位置差
static s16 pulseE4;
static s16 pulseE0;
static xSemaphoreHandle brakerxIT;
static u8 timeout_count = 0;

/*RS485外部中断回调函数*/
static void rs485_interruptCallback(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(brakerxIT, &xHigherPriorityTaskWoken);
	//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //如果需要的话进行一次任务切换
}
void BrakeCb_Init(void)
{
	brakerxIT = xSemaphoreCreateBinary();
	Brake_setIterruptCallback(rs485_interruptCallback);
}
#if 0
void vBrakeServoTask(void *param)	//转矩、速度模式
{
	portBASE_TYPE state;
	TickType_t xWakeTime,xLastWakeTime,xDeltaTime;
	BrakeCb_Init();
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0x02;	//Pn002：重新上电才有效，预先设置
	rs485txbuf[4]=0x00;
	rs485txbuf[5]=0x05;	//速度/转矩模式
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0xA8;	//Pn168
	rs485txbuf[4]=0x00;
	rs485txbuf[5]=0x01;	//内部速度
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0xA9;	//Pn169内部速度1
	rs485txbuf[4]=(sSpeed>>8)&0xFF;	//高位
	rs485txbuf[5]=sSpeed&0xFF;	//低位 保持500r/min
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0xCC;	//Pn204
	rs485txbuf[4]=0x00;
	rs485txbuf[5]=0x01;	//内部转矩1
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0xC8;		//Pn200	内部转矩1
	rs485txbuf[4]=0x00;		//高位 默认0
	rs485txbuf[5]=0x00;		//低位 默认0
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0x44;		//Pn068
	rs485txbuf[4]=0x3F;		//高位：Cmode,TR2,TR1,Sp3,Sp2,Sp1由通信控制
	rs485txbuf[5]=0x01;		//低位：Son由通信控制
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);	
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0x46;		//Pn070 
	rs485txbuf[4]=0x7F;		//高位 Cmode:1(OFF)速度模式；TR2，TR1:1(OFF)内部转矩；1,Sp3,Sp2,Sp1:1(OFF)内部速度1
	rs485txbuf[5]=0xB2;		//低位 Son:0（ON）使能驱动器	
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	
	while(1)
	{
		vTaskDelay(BRAKETIME);
		uBrake = getBrake();
		
		/*ABS功能*/
		xWakeTime = xTaskGetTickCount();	
		uRPM = getRPM();							//转速
		xDeltaTime=xWakeTime-xLastWakeTime;
		fDecNow=PI*DM*(uRPM-uLastRPM)/(60*KM*xDeltaTime/1000);	//单位m/s2
		xLastWakeTime=xWakeTime;
		uLastRPM=uRPM;
		if(fDecNow<-2.5)	//轮子减速度大于10m/s2
		{
			uBrake=0;
		}
		
		sTorque=uBrake+47;
		if((uBrake<uLastBrake)||(uBrake==0))
		{
			/*速度模式松开刹车*/
			rs485txbuf[0]=0x01;
			rs485txbuf[1]=0x06;
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x46;		//Pn070 
			rs485txbuf[4]=0x7F;		//高位 Cmode:1(OFF)速度模式；TR2，TR1:1(OFF)内部转矩1；Sp3,Sp2,Sp1:1(OFF)内部速度1
			rs485txbuf[5]=0xB2;		//低位 Son:0（ON）使能驱动器	
			ModbusWriteSReg(rs485txbuf,8);
			state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
			vTaskDelay(20);
		}
		else
		{
			/*转矩模式刹车*/
			while(1)
			{
//				rs485txbuf[0]=0x01;
//				rs485txbuf[1]=0x06;
//				rs485txbuf[2]=0x00;
//				rs485txbuf[3]=0x46;		//Pn070 
//				rs485txbuf[4]=0x5F;		//高位 Cmode:0(ON)转矩模式；TR2，TR1:1(OFF)内部转矩1；Sp3,Sp2,Sp1:1(OFF)内部速度1
//				rs485txbuf[5]=0xB3;		//低位 Son:1（OFF）失能驱动器	
//				ModbusWriteSReg(rs485txbuf,8);
//				state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
				rs485txbuf[0]=0x01;
				rs485txbuf[1]=0x06;
				rs485txbuf[2]=0x00;
				rs485txbuf[3]=0xC8;		//Pn200	内部转矩1
				rs485txbuf[4]=(sTorque>>8)&0xFF;		//高位 
				rs485txbuf[5]=sTorque&0xFF;		//低位 
				ModbusWriteSReg(rs485txbuf,8);
				state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
				rs485txbuf[0]=0x01;
				rs485txbuf[1]=0x06;
				rs485txbuf[2]=0x00;
				rs485txbuf[3]=0x46;		//Pn070 
				rs485txbuf[4]=0x5F;		//高位 Cmode:0(ON)转矩模式；TR2，TR1:1(OFF)内部转矩1；Sp3,Sp2,Sp1:1(OFF)内部速度1
				rs485txbuf[5]=0xB2;		//低位 Son:0（ON）使能驱动器	
				ModbusWriteSReg(rs485txbuf,8);
				state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if(state==pdTRUE)
				{
					cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
					rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
					if(cal_crc==rx_crc)
					{
						//写成功，进行下一步
						timeout_count=0;
						break;
					}
					else
					{
						//校验错误(忽略：电机驱动器收到了正确的数据，并且有响应，485传输线路问题导致crc错误)
						timeout_count=0;
						break;
					}
				}
				else
				{
					//超时未收到反馈，重复发送
					timeout_count++;
					if(timeout_count>100)						
					{
						timeout_count=0;
						break;
					}
				}		
			}
//			rs485txbuf[0]=0x01;
//			rs485txbuf[1]=0x03;	//读
//			rs485txbuf[2]=0x01;
//			rs485txbuf[3]=0x82;	//Dn018
//			rs485txbuf[4]=0x00;	//读1个寄存器
//			rs485txbuf[5]=0x01;
//			ModbusWriteSReg(rs485txbuf,8);
//			state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
//			if(state==pdTRUE)
//			{
//				cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
//				rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
//				if(cal_crc==rx_crc)
//				{
//					if(RS485_RX_BUF[3]&0x20)//取Bit13 TCMDreach,Bit 位为 0，表示功能为 ON 状态，为 1 则是 OFF 状态
//					{
//						//转矩没有到达设定指令值
//					}
//					else
//					{
//						//转矩到达设定指令值
//					}
//					
//				}
//				else
//				{
//					//校验错误
//				}
//			}
//			else
//			{
//				//超时
//			}
		}
		
		uLastBrake=uBrake;	
	}
}
#endif

#if 1
void vBrakeServoTask(void *param) //位置模式
{
	portBASE_TYPE state;
	TickType_t xWakeTime, xLastWakeTime, xDeltaTime;
	BrakeCb_Init();
	rs485txbuf[0] = 0x01;
	rs485txbuf[1] = 0x06;
	rs485txbuf[2] = 0x00;
	rs485txbuf[3] = 0x44; //Pn068
	rs485txbuf[4] = 0x00; //高位：00
	rs485txbuf[5] = 0x01; //低位：Son由通信控制
	ModbusWriteSReg(rs485txbuf, 8);
	state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0] = 0x01;
	rs485txbuf[1] = 0x06;
	rs485txbuf[2] = 0x00;
	rs485txbuf[3] = 0x45; //Pn069
	rs485txbuf[4] = 0x07; //高位：ptriger/pos2/pos1由通信控制
	rs485txbuf[5] = 0x00; //低位：00
	ModbusWriteSReg(rs485txbuf, 8);
	state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0] = 0x01;
	rs485txbuf[1] = 0x06;
	rs485txbuf[2] = 0x00;
	rs485txbuf[3] = 0x75; //Pn117
	rs485txbuf[4] = 0x00; //高位：00
	rs485txbuf[5] = 0x01; //低位：01 位置指令源：内部位置指令
	ModbusWriteSReg(rs485txbuf, 8);
	state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0] = 0x01;
	rs485txbuf[1] = 0x06;
	rs485txbuf[2] = 0x00;
	rs485txbuf[3] = 0x78; //Pn120	内部位置0（万）
	rs485txbuf[4] = 0x00; //高位
	rs485txbuf[5] = 0x00; //低位
	ModbusWriteSReg(rs485txbuf, 8);
	state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0] = 0x01;
	rs485txbuf[1] = 0x06;
	rs485txbuf[2] = 0x00;
	rs485txbuf[3] = 0x79; //Pn121	内部位置0（个）
	rs485txbuf[4] = 0x00; //高位
	rs485txbuf[5] = 0x00; //低位
	ModbusWriteSReg(rs485txbuf, 8);
	state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0] = 0x01;
	rs485txbuf[1] = 0x06;
	rs485txbuf[2] = 0x00;
	rs485txbuf[3] = 0x80; //Pn128	内部位置指令0 运行速度
	rs485txbuf[4] = 0x0B; //高位
	rs485txbuf[5] = 0xB8; //低位	3000r/min
	ModbusWriteSReg(rs485txbuf, 8);
	state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0] = 0x01;
	rs485txbuf[1] = 0x06;
	rs485txbuf[2] = 0x00;
	rs485txbuf[3] = 0x47; //Pn071
	rs485txbuf[4] = 0x7F; //内部位置0,并取消触发
	rs485txbuf[5] = 0xFF; //低位
	ModbusWriteSReg(rs485txbuf, 8);
	state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0] = 0x01;
	rs485txbuf[1] = 0x06;
	rs485txbuf[2] = 0x00;
	rs485txbuf[3] = 0x46; //Pn070
	rs485txbuf[4] = 0x7F;
	rs485txbuf[5] = 0xFE; //Son使能驱动器
	ModbusWriteSReg(rs485txbuf, 8);
	state = xSemaphoreTake(brakerxIT, MODBUS_TIME);

	while (1)
	{
		vTaskDelay(BRAKETIME);
		uBrake = getBrake();

		/*ABS功能*/
		xWakeTime = xTaskGetTickCount();
		uRPM = getRPM(); //转速
		xDeltaTime = xWakeTime - xLastWakeTime;
		fDecNow = PI * DM * (uRPM - uLastRPM) / (60 * KM * xDeltaTime / 1000); //单位m/s2
		xLastWakeTime = xWakeTime;
		uLastRPM = uRPM;
		if (fDecNow < -2.5) //轮子减速度大于10m/s2
		{
			uBrake = 0;
		}

		/*	总行程45000个脉冲
			每步450脉冲
		*/
		if (servo_step != uBrake)
		{
			pulseCount = uBrake - servo_step;
			pulseCount = 4000 * (uBrake - servo_step); //原来4
			pulseE4 = pulseCount / 10000;			   //万位
			pulseE0 = pulseCount - 10000 * pulseE4;	   //个位
			while (1)
			{
				rs485txbuf[0] = 0x01;
				rs485txbuf[1] = 0x06;
				rs485txbuf[2] = 0x00;
				rs485txbuf[3] = 0x46; //Pn070
				rs485txbuf[4] = 0x7F;
				rs485txbuf[5] = 0xFE; //Son使能驱动器
				ModbusWriteSReg(rs485txbuf, 8);
				state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if (state == pdTRUE)
				{
					cal_crc = crc_chk(RS485_RX_BUF, RS485_RX_CNT - 2);
					rx_crc = ((u16)RS485_RX_BUF[RS485_RX_CNT - 1] << 8) + RS485_RX_BUF[RS485_RX_CNT - 2];
					if (cal_crc == rx_crc)
					{
						//写成功，进行下一步
					}
					else
					{
						//校验错误，跳出
						break;
					}
				}
				else
				{
					//超时位收到反馈，跳出
					break;
				}
				rs485txbuf[0] = 0x01;
				rs485txbuf[1] = 0x06;
				rs485txbuf[2] = 0x00;
				rs485txbuf[3] = 0x47; //Pn071
				rs485txbuf[4] = 0x7F; //内部位置0,并取消触发
				rs485txbuf[5] = 0xFF; //低位
				ModbusWriteSReg(rs485txbuf, 8);
				state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if (state == pdTRUE)
				{
					cal_crc = crc_chk(RS485_RX_BUF, RS485_RX_CNT - 2);
					rx_crc = ((u16)RS485_RX_BUF[RS485_RX_CNT - 1] << 8) + RS485_RX_BUF[RS485_RX_CNT - 2];
					if (cal_crc == rx_crc)
					{
						//写成功，进行下一步
					}
					else
					{
						//校验错误，跳出
						break;
					}
				}
				else
				{
					//超时位收到反馈，跳出
					break;
				}
				rs485txbuf[0] = 0x01;
				rs485txbuf[1] = 0x06;
				rs485txbuf[2] = 0x00;
				rs485txbuf[3] = 0x78;				   //Pn120	内部位置0（万）
				rs485txbuf[4] = (pulseE4 >> 8) & 0xFF; //高位
				rs485txbuf[5] = pulseE4 & 0xFF;		   //低位
				ModbusWriteSReg(rs485txbuf, 8);
				state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if (state == pdTRUE)
				{
					cal_crc = crc_chk(RS485_RX_BUF, RS485_RX_CNT - 2);
					rx_crc = ((u16)RS485_RX_BUF[RS485_RX_CNT - 1] << 8) + RS485_RX_BUF[RS485_RX_CNT - 2];
					if (cal_crc == rx_crc)
					{
						//写成功，进行下一步
					}
					else
					{
						//校验错误，跳出
						break;
					}
				}
				else
				{
					//超时位收到反馈，跳出
					break;
				}
				rs485txbuf[0] = 0x01;
				rs485txbuf[1] = 0x06;
				rs485txbuf[2] = 0x00;
				rs485txbuf[3] = 0x79;				   //Pn121	内部位置0（个）
				rs485txbuf[4] = (pulseE0 >> 8) & 0xFF; //高位
				rs485txbuf[5] = pulseE0 & 0xFF;		   //低位
				ModbusWriteSReg(rs485txbuf, 8);
				state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if (state == pdTRUE)
				{
					cal_crc = crc_chk(RS485_RX_BUF, RS485_RX_CNT - 2);
					rx_crc = ((u16)RS485_RX_BUF[RS485_RX_CNT - 1] << 8) + RS485_RX_BUF[RS485_RX_CNT - 2];
					if (cal_crc == rx_crc)
					{
						//写成功，进行下一步
					}
					else
					{
						//校验错误，跳出
						break;
					}
				}
				else
				{
					//超时位收到反馈，跳出
					break;
				}

				rs485txbuf[0] = 0x01;
				rs485txbuf[1] = 0x06;
				rs485txbuf[2] = 0x00;
				rs485txbuf[3] = 0x47; //Pn071
				rs485txbuf[4] = 0x7B; //内部位置0,触发
				rs485txbuf[5] = 0xFF;
				ModbusWriteSReg(rs485txbuf, 8);
				state = xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if (state == pdTRUE)
				{
					cal_crc = crc_chk(RS485_RX_BUF, RS485_RX_CNT - 2);
					rx_crc = ((u16)RS485_RX_BUF[RS485_RX_CNT - 1] << 8) + RS485_RX_BUF[RS485_RX_CNT - 2];
					if (cal_crc == rx_crc)
					{
						//成功执行，跳出
						servo_step = uBrake; //标记伺服位置
						break;
					}
					else
					{
						//校验错误，跳出
						break;
					}
				}
				else
				{
					//超时位收到反馈，跳出
					break;
				}
			}
		}
	}
}
#endif

//获取减速度
float getDec(void)
{
	return fDecNow;
}
