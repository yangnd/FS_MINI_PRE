#include "menuL1_item.h"
#include "menu_ui.h"
#include "windows.h"
#include "scrollbar.h"
#include "task_joystick.h"
#include "task_key.h"
#include "display.h"
#include "OLED.h"
#include "config_param.h"
#include "24l01.h"
#include "menuL2_item.h"

/********************************************************************************	 
 * All rights reserved
********************************************************************************/

#define  MAIN_MENU_ITEMNUM    		7	//���˵�����

MenuItem_Typedef  mainMenu[MAIN_MENU_ITEMNUM];

/****** ��ָ��ťͼ�� ���� ����ʽ ˳��(��λ��ǰ) *****/
const u8 finger_img[32]={
0x07,0xF0,0x04,0x10,0x04,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x04,0x20,0x04,
0x41,0x54,0x4D,0x5C,0x75,0x70,0x05,0xC0,0x05,0x00,0x05,0x00,0x05,0x00,0x02,0x00,
};

const u8 mainMenuTitle[] = "FeiSuo";

const u8* mainMenuLanguage[3][MAIN_MENU_ITEMNUM]={
{
"1.���ѡ��",
"2.���ģʽ",
"3.�������",
"4.ҡ��У׼",
"5.����ѡ��",
"6.����",
"7.�˳�",
},
{
"1.Moto Select",
"2.Moto Mode",
"3.Max Throttle",
"4.Joystick Calib",
"5.Language",
"6.Reset",
"7.Exit",
},
{
"1.���ѡ��",
"2.���ģʽ",
"3.�������",
"4.ҡ��У׼",
"5.����ѡ��",
"6.����",
"7.�˳�",
},
};

void gotoSetMaxThrottle(void)
{
	exitMenu();
	setShow_ui(SETMAXTHROTTLE_UI);
}
void gotoJoystickCalibUI(void)
{
	exitMenu();
	setShow_ui(JOYSTICK_CALIB_UI);
}

void gotoMatchMiniFlyUI(void)
{
	exitMenu();
	setShow_ui(MATCH_UI);
}

void gotoResetUI(void)
{
	exitMenu();
	setShow_ui(RESET_UI);
}


/*��ʼ�����˵�*/
void mainMenuInit(void)
{	
	for(int i=0; i<MAIN_MENU_ITEMNUM; i++)
	{
		mainMenu[i].menuItemCount = MAIN_MENU_ITEMNUM;
		mainMenu[i].isSelect = false;
		mainMenu[i].icoSelected = finger_img;
		mainMenu[i].icoUnselected = NULL;
		mainMenu[i].title = mainMenuLanguage[configParam.language][i];
		mainMenu[i].Function = NULL;
		mainMenu[i].parentMenu = NULL;
		mainMenu[i].childrenMenu = NULL;
	}
	mainMenu[0].isSelect = true;
	mainMenu[0].Function = gotoNextMenu;
	mainMenu[0].childrenMenu = motoselMenu;

	mainMenu[1].Function = gotoNextMenu;
	mainMenu[1].childrenMenu = motomodeMenu;
	
	mainMenu[2].Function = gotoSetMaxThrottle;
	mainMenu[2].childrenMenu = NULL;	
	
	mainMenu[3].Function = gotoJoystickCalibUI;
	mainMenu[3].childrenMenu = NULL;
	
	mainMenu[4].Function = gotoNextMenu;
	mainMenu[4].childrenMenu = languageMenu;
	
	mainMenu[5].Function = gotoResetUI;
	mainMenu[5].childrenMenu = NULL;
	
	mainMenu[6].Function = exitMenu;
	mainMenu[6].childrenMenu = NULL;
}
