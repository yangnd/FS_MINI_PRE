#ifndef __TASK_KEY_H
#define __TASK_KEY_H
#include "sys.h"

//按键状态
#define KEY0 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)  //读取按键0
#define KEY1 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)  //读取按键1
#define WK_UP GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) //读取按键3(WK_UP)

#define KEY0_SHORT_PRESS 1
#define KEY0_LONG_PRESS 2
#define KEY1_SHORT_PRESS 3
#define KEY1_LONG_PRESS 4
#define WKUP_SHORT_PRESS 5
#define WKUP_LONG_PRESS 6

#define LONG_PRESS_COUNT 500 /*判断为长按时间（ms）*/

void vKeyTask(void *param);
u8 getKeyState(void);

#endif
