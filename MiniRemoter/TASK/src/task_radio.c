#include "task_radio.h"
#include "led.h"
#include "24l01.h"
#include "oled.h"
//#include "config_param.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/********************************************************************************	 
********************************************************************************/

/*发送和接收队列信息个数*/
#define  RADIOLINK_TX_QUEUE_SIZE  10
#define  RADIOLINK_RX_QUEUE_SIZE  10

xTaskHandle radiolinkTaskHandle;
static xQueueHandle  txQueue;
static xQueueHandle  rxQueue;
static xSemaphoreHandle nrfIT;
static bool isInit;
static bool connectStatus;
static ctrlData tx_p;
static fdbkData rx_p;
static u8 statusCount;
static u16 txCount;
static u16 txCountNum;
static u16 failRxCount;
static u16 failReceiveNum;
static TickType_t failRxcountTime;

/*nrf外部中断回调函数*/
static void nrf_interruptCallback(void)
{
	portBASE_TYPE  xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(nrfIT, &xHigherPriorityTaskWoken);
//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
}

/*无线配置初始化（地址、通道、速率）*/
static void radioInit(void)
{
	uint64_t addr = DEFAULT_ADDR;
	if(nrf_check() == SUCCESS)
	{
		nrfInit(PTX_MODE);
		nrf_setIterruptCallback(nrf_interruptCallback);
	}
	else
	{
		oledInit();
		oled_showString(0,0,(u8*)"NRF24L01 CHECK FAIL !",6,12);
		oled_refreshGram();
		while(1);
	}
	nrf_setAddress(addr);
	nrf_setChannel(DEFAULT_CHANNEL);
	nrf_setDataRate(DEFAULT_DATARATE);
}

/*无线连接初始化*/
void radiolinkInit(void)
{
	if (isInit) return;
	radioInit();
	
	txQueue = xQueueCreate(RADIOLINK_TX_QUEUE_SIZE, sizeof(ctrlData));
	ASSERT(txQueue);
	rxQueue = xQueueCreate(RADIOLINK_RX_QUEUE_SIZE, sizeof(fdbkData));
	ASSERT(rxQueue);
	
	nrfIT = xSemaphoreCreateBinary();
	
	connectStatus = false;
	isInit = true;
}
/*无线发送ctrlData Packet*/
bool radioSendPacket(const ctrlData *p)
{
	ASSERT(p);
	return xQueueSend(txQueue, p, 0);
}
bool radioSendPacketBlocking(const ctrlData *p)
{
	ASSERT(p);
	return xQueueSend(txQueue, p, 100);//portMAX_DELAY
}

/*无线接收fbData Packet*/
bool radioReceivePacket(fdbkData *p)
{
	ASSERT(p);
	return xQueueReceive(rxQueue, p, 0);
}
bool radioReceivePacketBlocking(fdbkData *p)
{
	ASSERT(p);
	return xQueueReceive(rxQueue, p, portMAX_DELAY);
}
/*无线连接任务*/
void vRadioTask(void* param)
{
	u8 rx_len;
	
	while(1)
	{
//		if(xQueueReceive(txQueue, &sendData, 0) == pdPASS)
//		{
//			nrf_txPacket((u8*)&sendData,sizeof(sendData));
//			txCount++;
//		}	
		xQueueReceive(txQueue, &tx_p, 0);
		nrf_txPacket((u8*)&tx_p,sizeof(tx_p));
		txCount++;
				
		xSemaphoreTake(nrfIT,1000);
//		LED_BLUE=!LED_BLUE;
		nrfEvent_e status = nrf_checkEventandRxPacket((u8*)&rx_p, &rx_len);
		if(status == RX_DR)//发送成功
		{	
			LED_BLUE = 0;
			LED_RED  = 1;
			statusCount = 0;
			connectStatus = true;
			if(rx_len <= 32)
			{
				xQueueSend(rxQueue, &rx_p, portMAX_DELAY);
			}
		}
		else if(status == MAX_RT)//发送失败
		{
			LED_BLUE = 1;
			LED_RED  = 0;
			failRxCount++;
			if(++statusCount > 10)//连续10次无应答则通讯失败
			{
				statusCount = 0;
				connectStatus = false;
			}
		}
		
		/*1000ms统计一次发送次数和收发失败次数*/
		if( xTaskGetTickCount()>=failRxcountTime+1000)
		{
			failRxcountTime = xTaskGetTickCount();
			failReceiveNum = failRxCount;
			txCountNum=txCount;
			failRxCount = 0;
			txCount=0;
			
		}
		
	}
}

/*获取丢包个数*/
u16 radioFailRxcount(void)
{
	return failReceiveNum;
}
/*获取发包个数*/
u16 radioTxcount(void)
{
	return txCountNum;
}


/*获取无线连接状态*/
bool radioConnectStatus(void)
{
	return connectStatus;
}

/*使能radiolink*/
void radioEnable(FunctionalState state)
{
	if(state == ENABLE)
		vTaskResume(radiolinkTaskHandle);
	else
		vTaskSuspend(radiolinkTaskHandle);
}

ctrlData getCMD(void)
{
	return tx_p;
}










