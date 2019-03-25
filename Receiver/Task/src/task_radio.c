#include "task_radio.h"
#include "task_key.h"
#include "24l01.h"
#include "led.h"
#include "delay.h"

/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/********************************************************************************	 
********************************************************************************/

/*���ͺͽ��ն�����Ϣ����*/
#define RADIOLINK_TX_QUEUE_SIZE 10
#define RADIOLINK_RX_QUEUE_SIZE 10

xTaskHandle radiolinkTaskHandle;
static xQueueHandle ctrlQueue;
static xQueueHandle fdbkQueue;
static xSemaphoreHandle nrfIT;
static bool isInit;
static bool connectStatus;
static u8 statusCount;
static u16 rxCount;
static u16 rxCountNum;
static TickType_t rxcountTime;

static ctrlData rx_p;
static fdbkData tx_p;

/*nrf�ⲿ�жϻص�����*/
static void nrf_interruptCallback(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(nrfIT, &xHigherPriorityTaskWoken);
//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //�����Ҫ�Ļ�����һ�������л�
}

/*�������ó�ʼ������ַ��ͨ�������ʣ�*/
static void radioInit(void)
{
	uint64_t addr = DEFAULT_ADDR;
	if (nrf_check() == SUCCESS)
	{
		nrfInit(PTX_MODE);
		nrf_setIterruptCallback(nrf_interruptCallback);
	}
	else
	{
		LED0 = 0;
		LED1 = 0;
		while (1);
	}
	LED0 = 1;
	LED1 = 1;
	nrf_setAddress(addr);
	nrf_setChannel(DEFAULT_CHANNEL);
	nrf_setDataRate(DEFAULT_DATARATE);
}

/*�������ӳ�ʼ��*/
void radiolinkInit(void)
{
	if (isInit)
		return;
	radioInit();

	ctrlQueue = xQueueCreate(RADIOLINK_TX_QUEUE_SIZE, sizeof(ctrlData));
	//	ASSERT(txQueue);
	fdbkQueue = xQueueCreate(RADIOLINK_RX_QUEUE_SIZE, sizeof(fdbkData));
	//	ASSERT(rxQueue);

	nrfIT = xSemaphoreCreateBinary();

	connectStatus = false;
	isInit = true;
}

bool radioReceiveCtrlPacket(ctrlData *p)
{
	return xQueueReceive(ctrlQueue, p, 0);
}
bool radioReceiveFdbkPacket(fdbkData *p)
{
	return xQueueReceive(fdbkQueue, p, 0);
}

/*������������*/
void vRadioTask(void *param)
{
	u8 rx_len;
	static u16 rx_r=0,rx_c=0,rx_r_n=0,rx_c_n=0;
	u8 upload=1;
	while (1)
	{
		vTaskDelay(500);
		nrf_txPacket((u8*)&tx_p,sizeof(tx_p));
		LED0=!LED0;
		xSemaphoreTake(nrfIT, 20);
		nrfEvent_e status = nrf_checkEventandRxPacket((u8 *)&rx_p, &rx_len);
		if (status == RX_DR) //�յ�ң������
		{
			LED1 = 0;
			statusCount = 0;
			rxCount++;
			connectStatus = true;
			if (rx_len == 6)
			{
				rx_r++;
				xQueueSend(ctrlQueue, &rx_p, portMAX_DELAY);
			}
//			if (rx_len == 32)
//			{
//				rx_c++;
//				xQueueSend(fdbkQueue, &rx_p, portMAX_DELAY);
//			}
		}
		else
		{
			if (++statusCount > 10) //����x����Ӧ����ͨѶʧ��
			{
				LED1 = 1;
				statusCount = 0;
				connectStatus = false;
			}
		}

		//1000ms���մ���
		if (xTaskGetTickCount() >= rxcountTime + 1000)
		{
			rxcountTime = xTaskGetTickCount();
			rxCountNum = rxCount;
			rxCount = 0;
			rx_r_n=rx_r;
			rx_c_n=rx_c;
			rx_r=0;
			rx_c=0;
		}
	}
}

/*��ȡ��������״̬*/
bool radioConnectStatus(void)
{
	return connectStatus;
}
/*��ȡ����ÿ����մ���*/
u16 getRxCount(void)
{
	return rxCountNum;
}
/*ʹ��radiolink*/
void radioEnable(FunctionalState state)
{
	if (state == ENABLE)
		vTaskResume(radiolinkTaskHandle);
	else
		vTaskSuspend(radiolinkTaskHandle);
}
