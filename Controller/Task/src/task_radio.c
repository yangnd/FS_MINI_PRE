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

/*发送和接收队列信息个数*/
#define RADIOLINK_TX_QUEUE_SIZE 10
#define RADIOLINK_RX_QUEUE_SIZE 10

xTaskHandle radiolinkTaskHandle;
static xQueueHandle txQueue;
static xQueueHandle rxQueue;
static xSemaphoreHandle nrfIT;
static bool isInit;
static bool connectStatus;
static u8 statusCount;
static u16 rxCount;
static u16 rxCountNum;
static TickType_t rxcountTime;

static ctrlData rx_p;
static fdbkData tx_p;
extern ctrlData carCtrlData;

/*nrf外部中断回调函数*/
static void nrf_interruptCallback(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(nrfIT, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //如果需要的话进行一次任务切换
}

/*无线配置初始化（地址、通道、速率）*/
static void radioInit(void)
{
	uint64_t addr = DEFAULT_ADDR;
	if (nrf_check() == SUCCESS)
	{
		nrfInit(PRX_MODE);
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

/*无线连接初始化*/
void radiolinkInit(void)
{
	if (isInit)
		return;
	radioInit();

	txQueue = xQueueCreate(RADIOLINK_TX_QUEUE_SIZE, sizeof(fdbkData));
	//	ASSERT(txQueue);
	rxQueue = xQueueCreate(RADIOLINK_RX_QUEUE_SIZE, sizeof(ctrlData));
	//	ASSERT(rxQueue);

	nrfIT = xSemaphoreCreateBinary();

	connectStatus = false;
	isInit = true;
}
/*无线发送fdbkData Packet*/
bool radioSendPacket(const fdbkData *p)
{
	//	ASSERT(p);
	return xQueueSend(txQueue, p, 0);
}
bool radioSendPacketBlocking(const fdbkData *p)
{
	//	ASSERT(p);
	return xQueueSend(txQueue, p, 100); //portMAX_DELAY
}

/*无线接收ctrlData Packet*/
bool radioReceivePacket(ctrlData *p)
{
	//	ASSERT(p);
	return xQueueReceive(rxQueue, p, 0);
}
bool radioReceivePacketBlocking(ctrlData *p)
{
	//	ASSERT(p);
	return xQueueReceive(rxQueue, p, portMAX_DELAY);
}
/*无线连接任务*/
void vRadioTask(void *param)
{
	u8 rx_len;
	nrf_txPacket_AP((u8 *)&tx_p, sizeof(tx_p));
//	nrf_txPacket_AP((u8 *)&tx_p, 0);
	while (1)
	{
		xSemaphoreTake(nrfIT, 20);
//		vTaskDelay(1);			//等待ACK payloa发送完毕
		nrfEvent_e status = nrf_checkEventandRxPacket((u8 *)&rx_p, &rx_len);
		if ((status == RX_DR)||(status == TX_DS))		//收到遥控数据
		{
			LED1 = 0;
			statusCount = 0;
			rxCount++;
			connectStatus = true;
			if (rx_len <= 32)
			{
				xQueueSend(rxQueue, &rx_p, portMAX_DELAY);
			}
			xQueueReceive(txQueue, &tx_p, 0);
			nrf_txPacket_AP((u8 *)&tx_p, sizeof(tx_p));
//			nrf_txPacket_AP((u8 *)&tx_p, 0);
		}
		else
		{
			if (++statusCount > 10) //连续x次无应答则通讯失败
			{
				LED1 = 1;
				statusCount = 0;
				connectStatus = false;
			}
		}

		//1000ms接收次数
		if (xTaskGetTickCount() >= rxcountTime + 1000)
		{
			rxcountTime = xTaskGetTickCount();
			rxCountNum = rxCount;
			rxCount = 0;
			if (connectStatus == true)
				LED1 = !LED1;
		}
	}
}

/*获取无线连接状态*/
bool radioConnectStatus(void)
{
	return connectStatus;
}
/*获取无线每秒接收次数*/
u16 getRxCount(void)
{
	return rxCountNum;
}
/*使能radiolink*/
void radioEnable(FunctionalState state)
{
	if (state == ENABLE)
		vTaskResume(radiolinkTaskHandle);
	else
		vTaskSuspend(radiolinkTaskHandle);
}
