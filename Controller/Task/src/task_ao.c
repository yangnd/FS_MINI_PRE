/**
 * @file task_ao.c
 * @author Yang Nandong (yangnd89@qq.com)
 * @brief 油门信号控制模拟量输出，转把实测输出范围0.88~3.58V；
 *          485AO模块0~5000对应0~5V
 * @version 0.1
 * @date 2020-11-27
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "task_ao.h"
#include "rs485.h"
#include "task_ctrldata.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern ctrlData carCtrlData;
static xSemaphoreHandle aoIt;
static void AO_interruptCallback(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(aoIt, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //如果需要的话进行一次任务切换
}
void AOCb_Init(void)
{
    aoIt = xSemaphoreCreateBinary();
    AO_setInterruptCallback(AO_interruptCallback);
}

void vAOTask(void *param)
{
    portBASE_TYPE state;
    u16 aoVal;
    u8 throttle = 0;
    u8 failCount = 0;
	AOCb_Init();
    while (1)
    {
        throttle = carCtrlData.Throttle;
        aoVal = 880 + throttle * 10;
        ModbusWriteSingleReg(0x03, 0x00, aoVal);
        state = xSemaphoreTake(aoIt, 50);
        if (pdTRUE == state)
        {
            failCount = 0;
        }
        else
        {
            failCount++;
        }
        if(failCount>10)
        {

        }
        
    }
}
