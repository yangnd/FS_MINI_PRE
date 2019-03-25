#include "rs485.h"
#include "delay.h"

static void (*interruptCb)(void) = 0;
/*����RS485�жϻص�����*/
void rs485_setIterruptCallback(void (*cb)(void))
{
	interruptCb = cb;
}

#ifdef EN_USART2_RX //���ʹ���˽���

//���ջ�����
u8 RS485_RX_BUF[64]; //���ջ���,���64���ֽ�.
//���յ������ݳ���
u8 RS485_RX_CNT = 0;

void USART2_IRQHandler(void)
{
	u8 res;

	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //���յ�����
	{

		res = USART_ReceiveData(USART2); //��ȡ���յ�������
		if (RS485_RX_CNT < 64)
		{
			RS485_RX_BUF[RS485_RX_CNT] = res; //��¼���յ���ֵ
			RS485_RX_CNT++;					  //������������1
		}
		if (interruptCb)
		{
			interruptCb();
		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}
#endif
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������
void RS485_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE); //ʹ��GPIOA,Dʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);						 //ʹ��USART2ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		 //PD7�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		//PA2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			  //PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, ENABLE);  //��λ����2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, DISABLE); //ֹͣ��λ

#ifdef EN_USART2_RX																	//���ʹ���˽���
	USART_InitStructure.USART_BaudRate = bound;										//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;						//9λ���ݳ���(ע�⣺������STM32���ݳ����ǰ�����żУ��λ֮��ĳ���)
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Odd;							///��żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure);
	; //��ʼ������

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		  //ʹ�ܴ���2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  //�����ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);							  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //�����ж�

	USART_Cmd(USART2, ENABLE); //ʹ�ܴ���

#endif

	RS485_TX_EN = 0; //Ĭ��Ϊ����ģʽ
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(u8 *buf, u8 len)
{
	u8 t;
	RS485_TX_EN = 1;		  //����Ϊ����ģʽ
	for (t = 0; t < len; t++) //ѭ����������
	{
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
			;
		USART_SendData(USART2, buf[t]);
	}

	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
		;
	RS485_RX_CNT = 0;
	RS485_TX_EN = 0; //����Ϊ����ģʽ
}
//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
u8 RS485_Receive_Data(u8 *buf, u8 *len)
{
	u8 rxlen = RS485_RX_CNT;
	u8 i = 0;
	u8 rxFlag = 1;
	*len = 0;							//Ĭ��Ϊ0
	delay_ms(10);						//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if (rxlen == RS485_RX_CNT && rxlen) //���յ�������,�ҽ��������
	{
		for (i = 0; i < rxlen; i++)
		{
			buf[i] = RS485_RX_BUF[i];
		}
		*len = RS485_RX_CNT; //��¼�������ݳ���
		RS485_RX_CNT = 0;	//����
		rxFlag = 0;			 //���ճɹ�
	}
	return rxFlag;
}

//u8 *data;//������ʼ��ַ�����ڼ��� CRC ֵ
//u8 length; //���ݳ���
//���� unsigned integer ���͵� CRC ֵ��
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

//���ܣ�д�����Ĵ���
//������txbuf�������ݣ�txlen���ͳ��ȣ�rxbuf���ճ��ȣ�rxlen���ճ���
//���أ�flag 0 ���ͳɹ���1 ����ʧ��
//ע�⣺�÷������ͳɹ�������У�����ʱ�����ظ����ͣ���α��⣿
u8 ModbusWriteSReg(u8 *txbuf, u8 txlen, u8 *rxbuf, u8 rxlen)
{
	//todo
	u16 crc_reg, rx_crc;
	u8 flag;
	crc_reg = crc_chk(txbuf, txlen - 2); //����CRC
	txbuf[txlen - 2] = crc_reg & 0xFF;
	txbuf[txlen - 1] = crc_reg >> 8;
	flag = 0; //flag:0 ���ͳɹ���1 ����ʧ��
	RS485_Send_Data(txbuf, txlen);
	delay_ms(20);
	RS485_Receive_Data(rxbuf, &rxlen);
	rx_crc = ((u16)rxbuf[rxlen - 1] << 8) + rxbuf[rxlen - 2];
	if ((rxbuf[2] == 0x86) || (rx_crc != crc_reg))
		flag = 1;
	return flag;
}

u8 ModbusReadMReg(u8 station,u16 address,u16 num,u8 *rxbuf,u8 *rxlen )
{
	return 0;
}
