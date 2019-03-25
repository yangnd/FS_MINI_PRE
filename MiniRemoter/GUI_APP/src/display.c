#include "display.h"
#include "main_ui.h"
#include "motof_ui.h"
#include "motor_ui.h"
#include "setMaxThrottle_ui.h"
#include "debug_ui.h"
#include "menu_ui.h"
#include "match_ui.h"
#include "joystickCalib_ui.h"
#include "menuL1_item.h"
#include "menuL2_item.h"
#include "reset_ui.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************************************	 
 * All rights reserved
********************************************************************************/

static enum ui_e show_ui = MAIN_UI;

/*设置显示界面*/
void setShow_ui(enum ui_e ui)
{
	show_ui = ui;
	GUI_ClearSCR();
}

/*显示任务*/
void displayTask(void* param)
{
	while(1)
	{
		vTaskDelay(50);
		switch(show_ui)
		{
			case MAIN_UI:
				main_ui();
				break;
			case MOTOF_UI:
				motof_ui();
				break;
			case MOTOR_UI:
				motor_ui();
				break;
			case SETMAXTHROTTLE_UI:
				setMaxThrottle_ui();
				break;
			case MENU_UI:
				Menu_Run();
				break;
			case DEBUG_UI:
				debug_ui();
				break;
			case JOYSTICK_CALIB_UI:
				joystickCalib_ui();
				break;
//			case MATCH_UI:
//				match_ui();
//				break;
			case RESET_UI:
				reset_ui();
				break;
			default:break;
		}
		GUI_Refresh();
	}
}

/*界面显示初始化*/
void displayInit(void)
{
	//菜单初始化
	mainMenuInit();
	motoselMenuInit();
	motomodeMenuInit();
	languageMenuInit();
}








