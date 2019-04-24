#include <stdio.h>
#include <string.h>
#include "main_ui.h"
#include "motof_ui.h"
#include "motor_ui.h"
#include "display.h"
#include "font.h"
#include "task_joystick.h"
#include "task_key.h"
#include "task_beep.h"
#include "config_param.h"
#include "oled.h"
#include "task_fdbkdata.h"
#include "task_radio.h"
#include "adc.h"
#include "menuL1_item.h"
#include "text.h"
#include "task_ctrldata.h"
#include "gui_basic.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
/********************************************************************************	 
 * All rights reserved
********************************************************************************/

#define  REMOTER_LOW_POWER  	3.25f
#define  REMOTER_FULL_POWER		4.15f
#define  CAR_LOW_POWER  	60.0f
#define  CAR_FULL_POWER		84.0f

static bool RCLock = true;
static bool trimFlag = false;
static bool isCARSelfTestPass = true;

static bool RCLowPower = false;
static bool RCLowPowerOld = false;
static bool CARLowPower = false;
static bool CARLowPowerOld = false;
static bool isMFCanFly = false;
static bool isMFCanFlyOld = false;

static s8 batPercent;
static TickType_t dispalyBatTime;
static TickType_t getMFMsgTime;
static u8 uRail;
enum dir_e joystick1,joystick2;
char str[22];
static motoData motoF_Data,motoR_Data;
extern float fCarBattery;;//返回的电量
ctrlData ctrlCMD;
u8 uGear=0;
char cGear,cGearF,cGearR;
u8 uThrF,uThrR;
u8 uRPMF,uRPMR;
//extern u8 rssi;//返回的无线信号值


static void drive(void)
{
	oled_fill(0,24,127,63,0);	
	/*显示选中电机，模式*/
	GUI_Circle(10,10,8,1);
	if(configParam.car.motosel == FRONT_ONLY)
	{
		oled_showChar(4,6,'F',6,8,1);
		oled_showChar(10,6,'O',6,8,1);
	}
	else if(configParam.car.motosel == REAR_ONLY)
	{
		oled_showChar(4,6,'R',6,8,1);
		oled_showChar(10,6,'O',6,8,1);
	}
	else 
	{
		oled_showChar(4,6,'F',6,8,1);
		oled_showChar(10,6,'R',6,8,1);
	}
	GUI_Circle(30,10,8,1);
	if(configParam.car.controlmode == THROTTLE)
		oled_showChar(27,4,'T',6,12,1);
	else if(configParam.car.controlmode == TORQUE)
		oled_showChar(27,4,'t',6,12,1);
	else if(configParam.car.controlmode == SPEED)
		oled_showChar(27,4,'S',6,12,1);
	GUI_Circle(50,10,8,1);
	ctrlCMD=getCMD();
	switch(ctrlCMD.Gear)
	{
		case NONE:
			cGear='N';
			break;
		case DRIVE:
			cGear='D';
			break;
		case REVERSE:
			cGear='R';
			break;
		case LOW:
			cGear='L';
			break;
	}
	oled_showChar(47,4,cGear,6,12,1);
	memset(str,0,22);
	sprintf(str,"%3d",ctrlCMD.Throttle);
	oled_showString(64,4,(u8*)str,6,12);
	GUI_Line(64,16,82,16,1);
	if(RCLock==true)
		oled_showPicture(49,43-15,bmp_lock[RCLock],30,30);
	else
	{
		//moto F反馈
		motoF_Data=getMotoData(MOTO_FRONT);
		memset(str,0,22);
		switch(motoF_Data.Gear)
		{
			case NONE:
				cGearF='N';
				break;
			case DRIVE:
				cGearF='D';
				break;
			case REVERSE:
				cGearF='R';
				break;
			case LOW:
				cGearF='L';
				break;
		}
		sprintf(str,"GEAR:%c",cGearF);
		oled_showString(0,22,(u8*)str,6,12);
		memset(str,0,22);
		sprintf(str,"THR:%3d",motoF_Data.Throttle);
		oled_showString(0,36,(u8*)str,6,12);
		memset(str,0,22);
		sprintf(str,"RPM:%4d", motoF_Data.RPM);
		oled_showString(0,50,(u8*)str,6,12);
		//moto R反馈
		motoR_Data=getMotoData(MOTO_REAR);
		memset(str,0,22);
		switch(motoR_Data.Gear)
		{
			case NONE:
				cGearR='N';
				break;
			case DRIVE:
				cGearR='D';
				break;
			case REVERSE:
				cGearR='R';
				break;
			case LOW:
				cGearR='L';
				break;
		}
		sprintf(str,"GEAR:%c",cGearR);
		oled_showString(64,22,(u8*)str,6,12);
		memset(str,0,22);
		sprintf(str,"THR:%3d",motoR_Data.Throttle);
		oled_showString(64,36,(u8*)str,6,12);
		memset(str,0,22);
		sprintf(str,"RPM:%4d",motoR_Data.RPM);
		oled_showString(64,50,(u8*)str,6,12);
	}
	/*按键处理*/
	u8 keyState = getKeyState();
	switch(keyState)
	{
		case KEY_J1_LONG_PRESS:	/*遥控器解锁*/
			RCLock=false;
			break;
		case KEY_J1_SHORT_PRESS:
			break;
		case KEY_J2_LONG_PRESS:	//进入菜单界面
			if(RCLock == true)
			{
				setShow_ui(MENU_UI);
			}
			break;
		case KEY_J2_SHORT_PRESS:	/*遥控器加锁*/
			RCLock=true;
			break;
		case KEY_L_LONG_PRESS:
			break;
		case KEY_L_SHORT_PRESS:	//切换到左轨道
			if( RCLock==false)
			{
				uRail=0x01;
			}
			break;
		case KEY_R_LONG_PRESS:	//进入调试界面
			if( RCLock==true)
			{
				setShow_ui(DEBUG_UI);
			}
			break;
		case KEY_R_SHORT_PRESS:	//切换到右轨道
			if( RCLock==false)
			{
				uRail=0x02;
			}
			break;
		default:
			break;
	}
//	joystick1=getJoystick1DirC();
//	joystick2=getJoystick2DirC();
//	joystick10=getJoystick1DirD();
//	joystick20=getJoystick2DirD();
	joystick1=getJoystick1Dir(0);
	joystick2=getJoystick2Dir(0);
	/*遥控器解锁、加锁*/
//	if(joystick1==BACK_LEFT && joystick2==BACK_RIGHT)
//	{
//		RCLock = false;
//	}
//	if(joystick1==BACK_RIGHT && joystick2==BACK_LEFT)
//	{
//		RCLock = true;
//	}	
	if(joystick1==RIGHT)
	{
		setShow_ui(MOTOF_UI);
	}
	else if(joystick1==LEFT)
	{
		setShow_ui(MOTOR_UI);
	}
	if((joystick2==RIGHT)&&(RCLock==true))
	{
		uGear++;
		if(uGear>5) uGear=0;
	}		
}

void main_ui(void)
{
//	u8 rssi_level;
	static float remoterBat;
	
	/*电量百分比显示*/
	if(xTaskGetTickCount() >= dispalyBatTime+1000)/*遥控器电量1000ms更新一次*/
	{
		dispalyBatTime = xTaskGetTickCount();
		remoterBat = ((float)getAdcValue(ADC_BAT)/4095.0)*3*2;
		if(remoterBat < REMOTER_LOW_POWER)
			RCLowPower = true;
		else 
			RCLowPower = false;	
	}
	/*显示遥控器电量*/
	batPercent = (remoterBat-REMOTER_LOW_POWER)/(REMOTER_FULL_POWER-REMOTER_LOW_POWER)*100;
	batPercent = limit(batPercent, 0, 99);
	
	/*信号显示*/
//	if(rssi < 80) rssi_level = 5;/*信号值小80db,信号为5格*/
//	else if(rssi < 85) rssi_level = 4;
//	else if(rssi < 90) rssi_level = 3;
//	else if(rssi < 95) rssi_level = 2;
//	else if(rssi >= 95) rssi_level = 1;
//	if(radioinkConnectStatus() == false) rssi_level = 0;
		
	/*显示图片*/
//	oled_showPicture(0,0,bmp_rssi[rssi_level],24,22);
//	oled_showPicture(24,0,bmp_logo,80,22);
	oled_showPicture(106,4,bmp_battery[batPercent*12/99],10,16);
	oled_showChar(119,13,'%',6,8,1);
	oled_showNum(116,4,batPercent,2,6,8);
	
	/*四轴自检*/
	if(isCARSelfTestPass == true)
	{
		drive();/*自检成功可飞行*/
	}
	else/*自检出错显示出错传感器*/
	{
		
	}
	
	/*低电量报警*/
	if(RCLowPower!=RCLowPowerOld || CARLowPower!=CARLowPowerOld)
	{
		if(RCLowPower==true || CARLowPower==true)
			runBeepAcktion(LOWPOWER_BEEP);
		else 
			stopBeepAcktion(LOWPOWER_BEEP);
		RCLowPowerOld = RCLowPower;
		CARLowPowerOld = CARLowPower;
	}
	if(CARLowPower==true && radioConnectStatus()==false)
	{
		CARLowPower = false;
	}	
}

bool getRCLock(void)
{
	return RCLock;
}

u8 getRail(void)
{
	return uRail;
}
