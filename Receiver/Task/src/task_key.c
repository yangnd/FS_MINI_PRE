#include <stdbool.h>
#include "stm32f10x.h"
#include "task_key.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

static bool key0_pressed;
static bool key1_pressed;
static bool WK_UP_pressed;
static u8 uKeyState;
static u32 uPressedTime;

void vKeyTask(void *param)
{
	while (1)
	{
		vTaskDelay(20);
		if (key0_pressed == false && KEY0 == 0)
		{
			key0_pressed = true;
			uPressedTime = xTaskGetTickCount();
		}
		if (key1_pressed == false && KEY1 == 0)
		{
			key1_pressed = true;
			uPressedTime = xTaskGetTickCount();
		}
		if (WK_UP_pressed == false && WK_UP == 1)
		{
			WK_UP_pressed = true;
			uPressedTime = xTaskGetTickCount();
		}

		if (key0_pressed == true)
		{
			if (KEY0 == 1)
				key0_pressed = false;
			if ((xTaskGetTickCount() - uPressedTime) > LONG_PRESS_COUNT)
				uKeyState = KEY0_LONG_PRESS;
			else if (KEY0 == 1)
				uKeyState = KEY0_SHORT_PRESS;
		}
		if (key1_pressed == true)
		{
			if (KEY1 == 1)
				key1_pressed = false;
			if ((xTaskGetTickCount() - uPressedTime) > LONG_PRESS_COUNT)
				uKeyState = KEY1_LONG_PRESS;
			else if (KEY1 == 1)
				uKeyState = KEY1_SHORT_PRESS;
		}
		if (WK_UP_pressed == true)
		{
			if (WK_UP == 0)
				WK_UP_pressed = false;
			if ((xTaskGetTickCount() - uPressedTime) > LONG_PRESS_COUNT)
				uKeyState = WKUP_LONG_PRESS;
			else if (WK_UP == 0)
				uKeyState = WKUP_SHORT_PRESS;
		}
	}
}

//读取按键状态
u8 getKeyState(void)
{
	u8 temp;
	temp = uKeyState;
	uKeyState = 0; //读取按键之后清零
	return temp;
}
