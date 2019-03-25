#include "task_brake_servo.h"
#include "rs485.h"
#include "task_ctrldata.h"
#include "task_can.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define PI 3.141592654											//���峣��PI
#define DM 0.375												//����ֱ��
#define KM 3.285714												//�ٱ�
#define DELTA 1.0												//���ٶȲ���ֵ
#define AMAX 10.0												//�����ٶ�10m/s2
#define BRAKETIME 10											//ɲ������ʱ��
#define MAXSTEP 100												//ɲ���г��ܲ���
static u8 uBrake=0,uLastBrake=0;								//ɲ���ź�
static s16 sTorque=0,sLastTorque=0;
u16 uRPM,uLastRPM;												//����ת��
s16 sDeltaRPM,sBrake;
float fDecTarget,fDecNow;
static u8 rs485txbuf[8];
static u16 rx_crc,cal_crc;
static s8 servo_step=0;			//����ŷ�λ��
static s32 pulseCount;			//ɲ��λ�����ŷ���ǰλ�ò�
static s16 pulseE4;
static s16 pulseE0;
static xSemaphoreHandle brakerxIT;

/*RS485�ⲿ�жϻص�����*/
static void rs485_interruptCallback(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(brakerxIT, &xHigherPriorityTaskWoken);
//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //�����Ҫ�Ļ�����һ�������л�
}
void BrakeCb_Init(void)
{
	brakerxIT = xSemaphoreCreateBinary();
	Brake_setIterruptCallback(rs485_interruptCallback);
}
void vBrakeServoTask(void *param)	//ת�ء��ٶ�ģʽ
{
	portBASE_TYPE state;
	TickType_t xWakeTime,xLastWakeTime,xDeltaTime;
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0x02;	//Pn002�������ϵ����Ч��Ԥ������
	rs485txbuf[4]=0x00;
	rs485txbuf[5]=0x05;	//�ٶ�/ת��ģʽ
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0xA8;	//Pn168
	rs485txbuf[4]=0x00;
	rs485txbuf[5]=0x01;	//�ڲ��ٶ�
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0xA9;	//Pn169�ڲ��ٶ�1
	rs485txbuf[4]=0x03;	//��λ
	rs485txbuf[5]=0xE8;	//��λ ����500r/min
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0xCC;	//Pn204
	rs485txbuf[4]=0x00;
	rs485txbuf[5]=0x01;	//�ڲ�ת��1
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0xC8;		//Pn200	�ڲ�ת��1
	rs485txbuf[4]=0x00;		//��λ Ĭ��0
	rs485txbuf[5]=0x00;		//��λ Ĭ��0
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0x44;		//Pn068
	rs485txbuf[4]=0x3F;		//��λ��Cmode,TR2,TR1,Sp3,Sp2,Sp1��ͨ�ſ���
	rs485txbuf[5]=0x01;		//��λ��Son��ͨ�ſ���
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);	
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0x46;		//Pn070 
	rs485txbuf[4]=0x7F;		//��λ Cmode:1(OFF)�ٶ�ģʽ��TR2��TR1:1(OFF)�ڲ�ת�أ�1,Sp3,Sp2,Sp1:1(OFF)�ڲ��ٶ�1
	rs485txbuf[5]=0xB2;		//��λ Son:0��ON��ʹ��������	
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	
	while(1)
	{
		vTaskDelay(BRAKETIME);
		uBrake = getBrake();
		
		/*ABS����*/
		xWakeTime = xTaskGetTickCount();	
		uRPM = getRPM();							//ת��
		xDeltaTime=xWakeTime-xLastWakeTime;
		fDecNow=PI*DM*(uRPM-uLastRPM)/(60*KM*xDeltaTime/1000);	//��λm/s2
		xLastWakeTime=xWakeTime;
		uLastRPM=uRPM;
		if(fDecNow<-2.5)	//���Ӽ��ٶȴ���10m/s2
		{
			uBrake=0;
		}
		
		sTorque=-2*uBrake ;
		if((uBrake<uLastBrake)||(uBrake==0))
		{
			/*�ٶ�ģʽ�ɿ�ɲ��*/
			rs485txbuf[0]=0x01;
			rs485txbuf[1]=0x06;
			rs485txbuf[2]=0x00;
			rs485txbuf[3]=0x46;		//Pn070 
			rs485txbuf[4]=0x7F;		//��λ Cmode:1(OFF)�ٶ�ģʽ��TR2��TR1:1(OFF)�ڲ�ת��1��Sp3,Sp2,Sp1:1(OFF)�ڲ��ٶ�1
			rs485txbuf[5]=0xB2;		//��λ Son:0��ON��ʹ��������	
			ModbusWriteSReg(rs485txbuf,8);
			state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
			vTaskDelay(20);
		}
		else
		{
			/*ת��ģʽɲ��*/
			while(1)
			{
//				rs485txbuf[0]=0x01;
//				rs485txbuf[1]=0x06;
//				rs485txbuf[2]=0x00;
//				rs485txbuf[3]=0x46;		//Pn070 
//				rs485txbuf[4]=0x5F;		//��λ Cmode:0(ON)ת��ģʽ��TR2��TR1:1(OFF)�ڲ�ת��1��Sp3,Sp2,Sp1:1(OFF)�ڲ��ٶ�1
//				rs485txbuf[5]=0xB3;		//��λ Son:1��OFF��ʧ��������	
//				ModbusWriteSReg(rs485txbuf,8);
//				state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
				rs485txbuf[0]=0x01;
				rs485txbuf[1]=0x06;
				rs485txbuf[2]=0x00;
				rs485txbuf[3]=0xC8;		//Pn200	�ڲ�ת��1
				rs485txbuf[4]=(sTorque>>8)&0xFF;		//��λ 
				rs485txbuf[5]=sTorque&0xFF;		//��λ 
				ModbusWriteSReg(rs485txbuf,8);
				state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
				rs485txbuf[0]=0x01;
				rs485txbuf[1]=0x06;
				rs485txbuf[2]=0x00;
				rs485txbuf[3]=0x46;		//Pn070 
				rs485txbuf[4]=0x5F;		//��λ Cmode:0(ON)ת��ģʽ��TR2��TR1:1(OFF)�ڲ�ת��1��Sp3,Sp2,Sp1:1(OFF)�ڲ��ٶ�1
				rs485txbuf[5]=0xB2;		//��λ Son:0��ON��ʹ��������	
				ModbusWriteSReg(rs485txbuf,8);
				state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if(state==pdTRUE)
				{
					cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
					rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
					if(cal_crc==rx_crc)
					{
						//д�ɹ���������һ��
						break;
					}
					else
					{
						//У�����(���ԣ�����������յ�����ȷ�����ݣ���������Ӧ��485������·���⵼��crc����)
						break;
					}
				}
				else
				{
					//��ʱδ�յ��������ظ�����
				}		
			}
//			rs485txbuf[0]=0x01;
//			rs485txbuf[1]=0x03;	//��
//			rs485txbuf[2]=0x01;
//			rs485txbuf[3]=0x82;	//Dn018
//			rs485txbuf[4]=0x00;	//��1���Ĵ���
//			rs485txbuf[5]=0x01;
//			ModbusWriteSReg(rs485txbuf,8);
//			state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
//			if(state==pdTRUE)
//			{
//				cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
//				rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
//				if(cal_crc==rx_crc)
//				{
//					if(RS485_RX_BUF[3]&0x20)//ȡBit13 TCMDreach,Bit λΪ 0����ʾ����Ϊ ON ״̬��Ϊ 1 ���� OFF ״̬
//					{
//						//ת��û�е����趨ָ��ֵ
//					}
//					else
//					{
//						//ת�ص����趨ָ��ֵ
//					}
//					
//				}
//				else
//				{
//					//У�����
//				}
//			}
//			else
//			{
//				//��ʱ
//			}
		}
		
		uLastBrake=uBrake;	
	}
}
void vBrakeServoTask0(void *param)		//λ��ģʽ
{
	portBASE_TYPE state;
	TickType_t xWakeTime,xLastWakeTime,xDeltaTime;
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0x78;		//Pn120	�ڲ�λ��0����
	rs485txbuf[4]=0x00;		//��λ
	rs485txbuf[5]=0x00;		//��λ
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0x79;		//Pn121	�ڲ�λ��0������
	rs485txbuf[4]=0x00;		//��λ
	rs485txbuf[5]=0x00;		//��λ
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0x47;		//Pn071
	rs485txbuf[4]=0x7F;		//�ڲ�λ��0,��ȡ������
	rs485txbuf[5]=0xFF;		//��λ
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	rs485txbuf[0]=0x01;
	rs485txbuf[1]=0x06;
	rs485txbuf[2]=0x00;
	rs485txbuf[3]=0x46;	//Pn070
	rs485txbuf[4]=0x7F;
	rs485txbuf[5]=0xFE;	//Sonʹ��������
	ModbusWriteSReg(rs485txbuf,8);
	state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
	
	while (1)
	{	
		vTaskDelay(BRAKETIME);
		uBrake = getBrake();
		
		/*ABS����*/
		xWakeTime = xTaskGetTickCount();	
		uRPM = getRPM();							//ת��
		xDeltaTime=xWakeTime-xLastWakeTime;
		fDecNow=PI*DM*(uRPM-uLastRPM)/(60*KM*xDeltaTime/1000);	//��λm/s2
		xLastWakeTime=xWakeTime;
		uLastRPM=uRPM;
		if(fDecNow<-2.5)	//���Ӽ��ٶȴ���10m/s2
		{
			uBrake=0;
		}
		
		/*	���г�45000������
			ÿ��450����
		*/
		if(servo_step!=uBrake)
		{
			pulseCount=uBrake-servo_step;
			pulseCount=430*(uBrake-servo_step);		//ԭ��450
			pulseE4=pulseCount/10000;				//��λ
			pulseE0=pulseCount-10000*pulseE4;		//��λ
			while(1)
			{
				rs485txbuf[0]=0x01;
				rs485txbuf[1]=0x06;
				rs485txbuf[2]=0x00;
				rs485txbuf[3]=0x46;	//Pn070
				rs485txbuf[4]=0x7F;
				rs485txbuf[5]=0xFE;	//Sonʹ��������
				ModbusWriteSReg(rs485txbuf,8);
				state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if(state==pdTRUE)
				{
					cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
					rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
					if(cal_crc==rx_crc)
					{
						//д�ɹ���������һ��
					}
					else
					{
						//У���������
						break;
					}
				}
				else
				{
					//��ʱλ�յ�����������
					break;
				}				
				rs485txbuf[0]=0x01;
				rs485txbuf[1]=0x06;
				rs485txbuf[2]=0x00;
				rs485txbuf[3]=0x47;		//Pn071
				rs485txbuf[4]=0x7F;		//�ڲ�λ��0,��ȡ������
				rs485txbuf[5]=0xFF;		//��λ
				ModbusWriteSReg(rs485txbuf,8);
				state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if(state==pdTRUE)
				{
					cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
					rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
					if(cal_crc==rx_crc)
					{
						//д�ɹ���������һ��
					}
					else
					{
						//У���������
						break;
					}
				}
				else
				{
					//��ʱλ�յ�����������
					break;
				}
				rs485txbuf[0]=0x01;
				rs485txbuf[1]=0x06;
				rs485txbuf[2]=0x00;
				rs485txbuf[3]=0x78;		//Pn120	�ڲ�λ��0����
				rs485txbuf[4]=(pulseE4>>8)&0xFF;		//��λ
				rs485txbuf[5]=pulseE4&0xFF;		//��λ
//				rs485txbuf[4]=(pulseCount>>8)&0xFF;		//��λ
//				rs485txbuf[5]=pulseCount&0xFF;		//��λ
				ModbusWriteSReg(rs485txbuf,8);
				state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if(state==pdTRUE)
				{
 					cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
					rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
					if(cal_crc==rx_crc)
					{
						//д�ɹ���������һ��
					}
					else
					{
						//У���������
						break;
					}
				}
				else
				{
					//��ʱλ�յ�����������
					break;
				}
				rs485txbuf[0]=0x01;
				rs485txbuf[1]=0x06;
				rs485txbuf[2]=0x00;
				rs485txbuf[3]=0x79;		//Pn121	�ڲ�λ��0������
				rs485txbuf[4]=(pulseE0>>8)&0xFF;		//��λ
				rs485txbuf[5]=pulseE0&0xFF;		//��λ
				ModbusWriteSReg(rs485txbuf,8);
				state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if(state==pdTRUE)
				{
 					cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
					rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
					if(cal_crc==rx_crc)
					{
						//д�ɹ���������һ��
					}
					else
					{
						//У���������
						break;
					}
				}
				else
				{
					//��ʱλ�յ�����������
					break;
				}
				
				rs485txbuf[0]=0x01;
				rs485txbuf[1]=0x06;
				rs485txbuf[2]=0x00;
				rs485txbuf[3]=0x47;	//Pn071
				rs485txbuf[4]=0x7B;	//�ڲ�λ��0,����
				rs485txbuf[5]=0xFF;
				ModbusWriteSReg(rs485txbuf,8);
				state=xSemaphoreTake(brakerxIT, MODBUS_TIME);
				if(state==pdTRUE)
				{
					cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
					rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
					if(cal_crc==rx_crc)
					{
						//�ɹ�ִ�У�����
						servo_step=uBrake;		//����ŷ�λ��
						break;
					}
					else
					{
						//У���������
						break;
					}
				}
				else
				{
					//��ʱλ�յ�����������
					break;
				}
				
			}
		}
/*����2*/
//		if(servo_step<uBrake)
//		{
//			//����ɲ��
//			pulseCount=uBrake-servo_step;
//			servo_step=uBrake;
//			if(servo_step<=MAXSTEP)
//			{
//				rs485txbuf[0]=0x01;
//				rs485txbuf[1]=0x06;
//				rs485txbuf[2]=0x00;
//				rs485txbuf[3]=0x78;		//Pn120	�ڲ�λ��0����
//				rs485txbuf[4]=(pulseCount>>8)&0xFF;		//��λ
//				rs485txbuf[5]=pulseCount&0xFF;		//��λ
//				ModbusWriteSReg(rs485txbuf,8);
//				vTaskDelay(20);
//				rs485txbuf[0]=0x01;
//				rs485txbuf[1]=0x06;
//				rs485txbuf[2]=0x00;
//				rs485txbuf[3]=0x47;		//Pn071
//				rs485txbuf[4]=0x7F;		//�ڲ�λ��0
//				rs485txbuf[5]=0xFF;		//��λ
//				ModbusWriteSReg(rs485txbuf,8);
//				vTaskDelay(20);
//				rs485txbuf[0]=0x01;
//				rs485txbuf[1]=0x06;
//				rs485txbuf[2]=0x00;
//				rs485txbuf[3]=0x47;	//Pn071
//				rs485txbuf[4]=0x7B;	//�ڲ�λ��0,����
//				rs485txbuf[5]=0xFF;
//				ModbusWriteSReg(rs485txbuf,8);
//				vTaskDelay(20);
//			}
//			else
//			{
//				servo_step=MAXSTEP;
//			}	
//		}
//		else if(servo_step>uBrake)
//		{
//			//�ɿ�ɲ��
//			pulseCount=uBrake-servo_step;
//			servo_step=uBrake;
//			if(servo_step>=0)
//			{
//				rs485txbuf[0]=0x01;
//				rs485txbuf[1]=0x06;
//				rs485txbuf[2]=0x00;
//				rs485txbuf[3]=0x7A;		//Pn122	�ڲ�λ��1��λ
//				rs485txbuf[4]=(pulseCount>>8)&0xFF;		//��λ
//				rs485txbuf[5]=pulseCount&0xFF;		//��λ
//				ModbusWriteSReg(rs485txbuf,8);
//				vTaskDelay(20);
//				rs485txbuf[0]=0x01;
//				rs485txbuf[1]=0x06;
//				rs485txbuf[2]=0x00;
//				rs485txbuf[3]=0x47;		//Pn071
//				rs485txbuf[4]=0x7E;		//�ڲ�λ��1
//				rs485txbuf[5]=0xFF;		//��λ
//				ModbusWriteSReg(rs485txbuf,8);
//				vTaskDelay(20);
//				rs485txbuf[0]=0x01;
//				rs485txbuf[1]=0x06;
//				rs485txbuf[2]=0x00;
//				rs485txbuf[3]=0x47;	//Pn071
//				rs485txbuf[4]=0x7A;	//�ڲ�λ��1,����
//				rs485txbuf[5]=0xFF;
//				ModbusWriteSReg(rs485txbuf,8);
//				vTaskDelay(20);
//			}
//			else
//			{
//				servo_step=0;
//			}
//		}
/*����1*/		
//		if (uBrake>5)						//�յ�ɲ���ź�
//		{
//			//�ŷ�ʹ��
//			rs485txbuf[0]=0x01;
//			rs485txbuf[1]=0x06;
//			rs485txbuf[2]=0x00;
//			rs485txbuf[3]=0x46;	//Pn070
//			rs485txbuf[4]=0x7F;
//			rs485txbuf[5]=0xFE;	//Sonʹ��������
//			ModbusWriteSReg(rs485txbuf,8); 					
//			if (uRPM > 0)
//			{
//				
//				if ((-fDecNow) <fDecTarget-DELTA) //ʵ�ʼ��ٶ����趨���ٶ�֮������趨��ֵ
//				{
//					//����ɲ��
//					servo_step++;
//					if(servo_step<=MAXSTEP)
//					{
//						rs485txbuf[0]=0x01;
//						rs485txbuf[1]=0x06;
//						rs485txbuf[2]=0x00;
//						rs485txbuf[3]=0x47;		//Pn071
//						rs485txbuf[4]=0x7F;		//�ڲ�λ��0
//						rs485txbuf[5]=0xFF;		//��λ
//						ModbusWriteSReg(rs485txbuf,8);
//						rs485txbuf[0]=0x01;
//						rs485txbuf[1]=0x06;
//						rs485txbuf[2]=0x00;
//						rs485txbuf[3]=0x47;	//Pn071
//						rs485txbuf[4]=0x7B;	//�ڲ�λ��0,����
//						rs485txbuf[5]=0xFF;
//						ModbusWriteSReg(rs485txbuf,8);
//					}
//					else
//					{
//						servo_step=MAXSTEP;
//					}								
//				}
//				else if ((-fDecNow) >fDecTarget+DELTA) 	//ʵ�ʼ��ٶ����趨���ٶ�֮����趨��ֵ
//				{
//					//�ɿ�ɲ��
//					servo_step--;
//					if(servo_step>=0)
//					{
//						rs485txbuf[0]=0x01;
//						rs485txbuf[1]=0x06;
//						rs485txbuf[2]=0x00;
//						rs485txbuf[3]=0x47;		//Pn071
//						rs485txbuf[4]=0x7E;		//�ڲ�λ��1
//						rs485txbuf[5]=0xFF;		//��λ
//						ModbusWriteSReg(rs485txbuf,8);
//						rs485txbuf[0]=0x01;
//						rs485txbuf[1]=0x06;
//						rs485txbuf[2]=0x00;
//						rs485txbuf[3]=0x47;	//Pn071
//						rs485txbuf[4]=0x7A;	//�ڲ�λ��1,����
//						rs485txbuf[5]=0xFF;
//						ModbusWriteSReg(rs485txbuf,8);
//					}
//					else
//					{
//						servo_step=0;
//					}
//				}
//				else if ((fDecTarget-DELTA <=( -fDecNow) ) && ((-fDecNow )<= fDecTarget+DELTA)) //ʵ�ʼ��ٶ����趨���ٶ�֮�����趨��ֵ��Χ��
//				{
//					//����ɲ��λ��
//					
//				}
//			}
//		}
//		else
//		{
//			//�˻ص���ȫ�ɿ�λ��
//			while(servo_step)
//			{
//				servo_step--;
//				rs485txbuf[0]=0x01;
//				rs485txbuf[1]=0x06;
//				rs485txbuf[2]=0x00;
//				rs485txbuf[3]=0x47;		//Pn071
//				rs485txbuf[4]=0x7E;		//�ڲ�λ��1
//				rs485txbuf[5]=0xFF;		//��λ
//				ModbusWriteSReg(rs485txbuf,8);
//				rs485txbuf[0]=0x01;
//				rs485txbuf[1]=0x06;
//				rs485txbuf[2]=0x00;
//				rs485txbuf[3]=0x47;	//Pn071
//				rs485txbuf[4]=0x7A;	//�ڲ�λ��1,����
//				rs485txbuf[5]=0xFF;
//				ModbusWriteSReg(rs485txbuf,8);
//			}
//		}
	}
}

//��ȡ���ٶ�
float getDec(void)
{
	return fDecNow;
}
