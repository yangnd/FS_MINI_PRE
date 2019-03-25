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
static u16 rx_crc,cal_crc;
static u8 rxlen;
static u8 uRail;
static u8 railState;
static u8 keyState;
static xSemaphoreHandle railrxIT;

/*RS485�ⲿ�жϻص�����*/
static void rs485_interruptCallback(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(railrxIT, &xHigherPriorityTaskWoken);
//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //�����Ҫ�Ļ�����һ�������л�
}
void RailCb_Init(void)
{
	railrxIT = xSemaphoreCreateBinary();
	Rail_setIterruptCallback(rs485_interruptCallback);
}
void vChangeRailTask(void *param)
{
	portBASE_TYPE state;
	static u8 step=0;
	static u8 preach=0;
	static u8 change_en=1;
	static u8 SCount=0;
	static u8 modbusCount=0;
	
	while(1)
	{
		vTaskDelay(50);
		
		keyState=getKeyState();
		if(keyState==KEY0_SHORT_PRESS)	railState=LEFTRAIL;			//�����Ӵ��ڹ��״̬
		else if(keyState==KEY1_SHORT_PRESS)	railState=RIGHTRAIL;
		
		uRail=getRail();
		preach=0;
		SCount=0;
		while((uRail==1)&&(railState==RIGHTRAIL)&&change_en)//��ת270��
		{
			switch(step)
			{
				case 0://ѡ���ڲ�λ��
					rs485txbuf[0]=0x02;
					rs485txbuf[1]=0x06;
					rs485txbuf[2]=0x00;
					rs485txbuf[3]=0x47;		//Pn071
					rs485txbuf[4]=0x7F;		//�ڲ�λ��0
					rs485txbuf[5]=0xFF;		//��λ
					ModbusWriteSReg(rs485txbuf,8);
					state=xSemaphoreTake(railrxIT, MODBUS_TIME);
					if(state==pdTRUE)
					{
						cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
						rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
						if(cal_crc==rx_crc)
						{
							//д�ɹ���������һ��
							step++;
							modbusCount=0;
						}
						else
						{
							//У����󣬷���ִ�б�����
						}
					}
					else
					{
						modbusCount++;
					}
					break;
				case 1://ʹ���ŷ����
					rs485txbuf[0]=0x02;
					rs485txbuf[1]=0x06;
					rs485txbuf[2]=0x00;
					rs485txbuf[3]=0x46;	//Pn070
					rs485txbuf[4]=0x7F;
					rs485txbuf[5]=0xFE;	//Sonʹ��������
					ModbusWriteSReg(rs485txbuf,8);
					state=xSemaphoreTake(railrxIT, MODBUS_TIME);
					if(state==pdTRUE)
					{
						cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
						rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
						if(cal_crc==rx_crc)
						{
							//д�ɹ���������һ��
							step++;
							modbusCount=0;
						}
						else
						{
							//У����󣬷���ִ�б�����
						}
					}
					else
					{
						modbusCount++;
					}
					break;
				case 2://�����ڲ�λ�ã����ת��
					rs485txbuf[0]=0x02;
					rs485txbuf[1]=0x06;
					rs485txbuf[2]=0x00;
					rs485txbuf[3]=0x47;	//Pn071
					rs485txbuf[4]=0x7B;	//�ڲ�λ��0,����
					rs485txbuf[5]=0xFF;
					ModbusWriteSReg(rs485txbuf,8);
					state=xSemaphoreTake(railrxIT, MODBUS_TIME);
					if(state==pdTRUE)
					{
						cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
						rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
						if(cal_crc==rx_crc)
						{
							//д�ɹ���������һ��
							step++;
							modbusCount=0;
							vTaskDelay(500);							
						}
						else
						{
							//У����󣬷���ִ�б�����
						}
					}
					else
					{
						modbusCount++;
					}
					break;
				case 3://��ȡDn018���ж�bit3-Preach
					rs485txbuf[0]=0x02;
					rs485txbuf[1]=0x03;	//��
					rs485txbuf[2]=0x01;
					rs485txbuf[3]=0x82;	//Dn018
					rs485txbuf[4]=0x00;	//��1���Ĵ���
					rs485txbuf[5]=0x01;
					ModbusWriteSReg(rs485txbuf,8);
					state=xSemaphoreTake(railrxIT, MODBUS_TIME);
					if(state==pdTRUE)
					{
						SCount++;
						cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
						rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
						if(cal_crc==rx_crc)
						{
							if(RS485_RX_BUF[4]&0x08)//ȡBit3 Preach,Bit λΪ 0����ʾ����Ϊ ON ״̬��Ϊ 1 ���� OFF ״̬
							{
								//λ��ƫ�����ƫ���
								preach=0;
							}
							else
							{
								//����ָ��λ�ã�������һ��
								preach=1;
								step++;
								modbusCount=0;
							}
							
						}
						else
						{
							//У�����
						}
					}
					else
					{
						modbusCount++;
					}
					break;
				case 4://ʧ���ŷ����
					rs485txbuf[0]=0x02;
					rs485txbuf[1]=0x06;
					rs485txbuf[2]=0x00;
					rs485txbuf[3]=0x46;	//Pn070
					rs485txbuf[4]=0x7F;
					rs485txbuf[5]=0xFF;	//Sonʧ��������
					ModbusWriteSReg(rs485txbuf,8);
					state=xSemaphoreTake(railrxIT, MODBUS_TIME);
					if(state==pdTRUE)
					{
						cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
						rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
						if(cal_crc==rx_crc)
						{
							//���ָ�����
							if(preach)
							{
								railState=LEFTRAIL;
								preach=0;//���״̬
							}
							else
							{
								change_en=0;//ָ����ɵ�λ��ƫ��ʱ����ֹ���
							}
							step=0;//��λ����
							modbusCount=0;
						}
						else
						{
							//У����󣬷���ִ�б�����
						}
					}
					else
					{
						modbusCount++;
					}
					break;
			}		
		}
		while((uRail==2)&&(railState==LEFTRAIL)&&change_en)//��ת270��
		{
			switch(step)
			{
				case 0://ѡ���ڲ�λ��
					rs485txbuf[0]=0x02;
					rs485txbuf[1]=0x06;
					rs485txbuf[2]=0x00;
					rs485txbuf[3]=0x47;		//Pn071
					rs485txbuf[4]=0x7E;		//�ڲ�λ��1
					rs485txbuf[5]=0xFF;		//��λ
					ModbusWriteSReg(rs485txbuf,8);
					state=xSemaphoreTake(railrxIT, MODBUS_TIME);
					if(state==pdTRUE)
					{
						cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
						rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
						if(cal_crc==rx_crc)
						{
							//д�ɹ���������һ��
							step++;
							modbusCount=0;
						}
						else
						{
							//У����󣬷���ִ�б�����
						}
					}
					else
					{
						modbusCount++;
					}
					break;
				case 1://ʹ���ŷ����
					rs485txbuf[0]=0x02;
					rs485txbuf[1]=0x06;
					rs485txbuf[2]=0x00;
					rs485txbuf[3]=0x46;	//Pn070
					rs485txbuf[4]=0x7F;
					rs485txbuf[5]=0xFE;	//Sonʹ��������
					ModbusWriteSReg(rs485txbuf,8);
					state=xSemaphoreTake(railrxIT, MODBUS_TIME);
					if(state==pdTRUE)
					{
						cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
						rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
						if(cal_crc==rx_crc)
						{
							//д�ɹ���������һ��
							step++;
							modbusCount=0;
						}
						else
						{
							//У����󣬷���ִ�б�����
						}
					}
					else
					{
						modbusCount++;
					}
					break;
				case 2://�����ڲ�λ�ã����ת��
					rs485txbuf[0]=0x02;
					rs485txbuf[1]=0x06;
					rs485txbuf[2]=0x00;
					rs485txbuf[3]=0x47;	//Pn071
					rs485txbuf[4]=0x7A;	//�ڲ�λ��1,����
					rs485txbuf[5]=0xFF;
					ModbusWriteSReg(rs485txbuf,8);
					state=xSemaphoreTake(railrxIT, MODBUS_TIME);
					if(state==pdTRUE)
					{
						cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
						rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
						if(cal_crc==rx_crc)
						{
							//д�ɹ���������һ��
							step++;
							modbusCount=0;
							vTaskDelay(500);
						}
						else
						{
							//У����󣬷���ִ�б�����
						}
					}
					else
					{
						modbusCount++;
					}
					break;
				case 3://��ȡDn018���ж�bit3-Preach
					rs485txbuf[0]=0x02;
					rs485txbuf[1]=0x03;	//��
					rs485txbuf[2]=0x01;
					rs485txbuf[3]=0x82;	//Dn018
					rs485txbuf[4]=0x00;	//��1���Ĵ���
					rs485txbuf[5]=0x01;
					ModbusWriteSReg(rs485txbuf,8);
					state=xSemaphoreTake(railrxIT, MODBUS_TIME);
					if(state==pdTRUE)
					{
						SCount++;
						cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
						rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
						if(cal_crc==rx_crc)
						{
							if(RS485_RX_BUF[4]&0x08)//ȡBit3 Preach,Bit λΪ 0����ʾ����Ϊ ON ״̬��Ϊ 1 ���� OFF ״̬
							{
								//λ��ƫ�����ƫ���
								preach=0;
							}
							else
							{
								//����ָ��λ�ã�������һ��
								preach=1;
								step++;
								modbusCount=0;
							}
							
						}
						else
						{
							//У�����
						}
					}
					else
					{
						modbusCount++;
					}
					break;
				case 4://ʧ���ŷ����
					rs485txbuf[0]=0x02;
					rs485txbuf[1]=0x06;
					rs485txbuf[2]=0x00;
					rs485txbuf[3]=0x46;	//Pn070
					rs485txbuf[4]=0x7F;
					rs485txbuf[5]=0xFF;	//Sonʧ��������
					ModbusWriteSReg(rs485txbuf,8);
					state=xSemaphoreTake(railrxIT, MODBUS_TIME);
					if(state==pdTRUE)
					{
						cal_crc=crc_chk(RS485_RX_BUF,RS485_RX_CNT-2);
						rx_crc=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)+RS485_RX_BUF[RS485_RX_CNT-2];
						if(cal_crc==rx_crc)
						{
							//���ָ�����
							if(preach)
							{
								railState=RIGHTRAIL;
								preach=0;//���״̬
							}
							else
							{
								change_en=0;//ָ����ɵ�λ��ƫ��ʱ����ֹ���
							}
							step=0;//��λ����
							modbusCount=0;
						}
						else
						{
							//У����󣬷���ִ�б�����
						}
					}
					else
					{
						modbusCount++;
					}
					break;
			}		
		}
	}
}
