#include "menuL2_item.h"
#include "menu_ui.h"
#include "windows.h"
#include "scrollbar.h"
#include "task_joystick.h"
#include "task_key.h"
#include "display.h"
#include "OLED.h"
#include "config_param.h"
#include "24l01.h"
#include "menuL1_item.h"
#include "task_fdbkdata.h"

/********************************************************************************	 
 * All rights reserved
********************************************************************************/

/*菜单项数*/
#define  MOTOSEL_MENU_NUM   3
#define  MOTOMODE_MENU_NUM    	3
#define  LANGUAGE_MENU_NUM    	3

MenuItem_Typedef  motoselMenu[MOTOSEL_MENU_NUM];
MenuItem_Typedef  motomodeMenu[MOTOMODE_MENU_NUM];
MenuItem_Typedef  languageMenu[LANGUAGE_MENU_NUM];

/****** 选中图标 阴码 逐列式 顺向(高位在前) *****/
const u8 radio_selected_img[32]={
0x00,0x00,0x03,0xC0,0x0C,0x30,0x10,0x08,0x11,0x88,0x23,0xC4,0x27,0xE4,0x27,0xE4,
0x23,0xC4,0x11,0x88,0x10,0x08,0x0C,0x30,0x03,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,
};

/****** 非选中图标 阴码 逐列式 顺向(高位在前) *****/
const u8  radio_unselected_img[32]={
0x00,0x00,0x03,0xC0,0x0C,0x30,0x10,0x08,0x10,0x08,0x20,0x04,0x20,0x04,0x20,0x04,
0x20,0x04,0x10,0x08,0x10,0x08,0x0C,0x30,0x03,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,
};

const u8* motoselLanguage[3][MOTOSEL_MENU_NUM]={
{
"电机A",
"电机B",
"电机A和B",
},
{
"Moto A",
"Moto B",
"Moto A and B",
},
{
"电机A",
"电机B",
"电机A和B",
},
};
const u8* motomodeLanguage[3][MOTOMODE_MENU_NUM]={
{
"油门控制模式",
"转矩控制模式",
"转速控制模式",
},
{
"Throttle Control Mode",
"Torque Control Mode",
"Speed Control Mode",
},
{
"油门控制模式",
"转矩控制模式",
"转速控制模式",
},
};

const u8* languageChoiceLanguage[3][LANGUAGE_MENU_NUM]={
{
"简体中文",
"English",
"繁体中文"
},
{
"简体中文",
"English",
"繁体中文"
},
{
"简体中文",
"English",
"繁体中文"
}	
};


static void selMotoF(void)
{
	exitMenu();
	configParam.car.motosel = FRONT_ONLY;
}

static void selMotoR(void)
{
	exitMenu();
	configParam.car.motosel = REAR_ONLY;
}

static void selMotoFR(void)
{
	exitMenu();
	configParam.car.motosel = FRONT_REAR;
}

/*初始化电机选择菜单*/
void motoselMenuInit(void)
{	
	for(int i=0; i<MOTOSEL_MENU_NUM; i++)
	{
		motoselMenu[i].menuItemCount = MOTOSEL_MENU_NUM;
		motoselMenu[i].isSelect = false;
		motoselMenu[i].icoSelected = radio_selected_img;
		motoselMenu[i].icoUnselected = radio_unselected_img;
		motoselMenu[i].title = motoselLanguage[configParam.language][i];
		motoselMenu[i].parentMenu = mainMenu;
		motoselMenu[i].childrenMenu = NULL;
	}
	motoselMenu[0].Function = selMotoF;
	motoselMenu[1].Function = selMotoR;
	motoselMenu[2].Function = selMotoFR;
	switch(configParam.car.motosel)
	{
		case FRONT_ONLY:
			motoselMenu[0].isSelect = true;
			break;
		case REAR_ONLY:
			motoselMenu[1].isSelect = true;
			break;
		case FRONT_REAR:
			motoselMenu[2].isSelect = true;
			break;
	}
}

static void motoModeThrottle(void)
{
	exitMenu();
	configParam.car.controlmode = THROTTLE;
}
static void motoModeTorque(void)
{
	exitMenu();
	configParam.car.controlmode = TORQUE;
}
static void motoModeSpeed(void)
{
	exitMenu();
	configParam.car.controlmode = SPEED;
}

/*初始化电机模式菜单*/
void motomodeMenuInit(void)
{	
	for(int i=0; i<MOTOMODE_MENU_NUM; i++)
	{
		motomodeMenu[i].menuItemCount = MOTOMODE_MENU_NUM;
		motomodeMenu[i].isSelect = false;
		motomodeMenu[i].icoSelected = radio_selected_img;
		motomodeMenu[i].icoUnselected = radio_unselected_img;
		motomodeMenu[i].title = motomodeLanguage[configParam.language][i];
		motomodeMenu[i].Function = NULL;
		motomodeMenu[i].parentMenu = mainMenu;
		motomodeMenu[i].childrenMenu = NULL;
	}
	motomodeMenu[0].Function = motoModeThrottle;
	motomodeMenu[1].Function = motoModeTorque;
	motomodeMenu[2].Function = motoModeSpeed;
	switch(configParam.car.controlmode)
	{
		case THROTTLE:
			motomodeMenu[0].isSelect = true;
			break;
		case TORQUE:
			motomodeMenu[1].isSelect = true;
			break;
		case SPEED:
			motomodeMenu[2].isSelect = true;
			break;
	}
}

static void setLanguageSimpleChinese(void)
{
	exitMenu();
	configParam.language = SIMPLE_CHINESE;
	mainMenuInit();
	motoselMenuInit();
	motomodeMenuInit();
	languageMenuInit();
}

static void setLanguageEnglish(void)
{
	exitMenu();
	configParam.language = ENGLISH;
	mainMenuInit();
	motoselMenuInit();
	motomodeMenuInit();
	languageMenuInit();
}

static void setLanguageComplexChinese(void)
{
	exitMenu();
	configParam.language = COMPLEX_CHINESE;
	mainMenuInit();
	motoselMenuInit();
	motomodeMenuInit();
	languageMenuInit();
}

/*初始化语言选择菜单*/
void languageMenuInit(void)
{	
	for(int i=0; i<LANGUAGE_MENU_NUM; i++)
	{
		languageMenu[i].menuItemCount = LANGUAGE_MENU_NUM;
		languageMenu[i].isSelect = false;
		languageMenu[i].icoSelected = radio_selected_img;
		languageMenu[i].icoUnselected = radio_unselected_img;
		languageMenu[i].title = languageChoiceLanguage[configParam.language][i];
		languageMenu[i].parentMenu = mainMenu;
		languageMenu[i].childrenMenu = NULL;
	}
	languageMenu[0].Function = setLanguageSimpleChinese;
	languageMenu[1].Function = setLanguageEnglish;
	languageMenu[2].Function = setLanguageComplexChinese;
	
	switch(configParam.language)
	{
		case SIMPLE_CHINESE:
			languageMenu[0].isSelect = true;
			break;
		case ENGLISH:
			languageMenu[1].isSelect = true;
			break;
		case COMPLEX_CHINESE:
			languageMenu[2].isSelect = true;
	}
}

