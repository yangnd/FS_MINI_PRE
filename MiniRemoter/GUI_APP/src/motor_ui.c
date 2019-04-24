#include "display.h"
#include "motor_ui.h"
#include "main_ui.h"
#include "gui_basic.h"
#include "task_joystick.h"
#include "task_fdbkdata.h"
#include "task_radio.h"
#include "task_key.h"
#include "config_param.h"
#include <string.h>
#include <stdio.h>
#include "oled.h"

extern ctrlData ctrlCMD;
motoData motoR_Data;
//G档位 T油门 M模式 R转速 Tm电机温度 Td驱动器温度
//V电压 C电流 D里程 E错误代码
//TQc转矩指令 Rc转速指令 TQ转矩

void motor_ui(void)
{
	static u8 page=0;
	char str[22];
	enum dir_e joystick1,joystick2;
	oled_fill(0,24,127,63,0);
//	joystick1=getJoystick1DirD();
//	joystick2=getJoystick2DirD();
	joystick1=getJoystick1Dir(0);
	joystick2=getJoystick2Dir(0);
	if(joystick2==RIGHT) 
	{
		page++;
		oled_fill(0,16,127,63,0);
	}
	else if(joystick2==LEFT) 
	{
		page--;
		oled_fill(0,16,127,63,0);
	}
	
	motoR_Data=getMotoData(MOTO_REAR);
	
	GUI_Circle(8,8,8,1);
	if(getRCLock()==false)	oled_showChar(5,2,'U',6,12,1);
	else oled_showChar(5,2,'L',6,12,1);
	memset(str,0,22);
	sprintf(str,"MOTO_R");
	oled_showString(34,0,(u8*)str,6,12);
	ctrlCMD=getCMD();
	memset(str,0,22);
	sprintf(str,"%3d",ctrlCMD.Throttle);
	oled_showString(76,0,(u8*)str,6,12);
	GUI_Line(76,14,94,14,1);
	if(page%3==0)
	{
		memset(str,0,22);
		switch(motoR_Data.Gear)
		{
			case NONE:
				sprintf(str,"Gear: N");
				break;
			case DRIVE:
				sprintf(str,"Gear: D");
				break;
			case REVERSE:
				sprintf(str,"Gear: R");
				break;
			case LOW:
				sprintf(str,"Gear: L");
				break;
		}
		oled_showString(0,16,(u8*)str,6,12);
		
		memset(str,0,22);
		sprintf(str,"Thrt:%d",motoR_Data.Throttle);
		oled_showString(0,32,(u8*)str,6,12);
		
		memset(str,0,22);
		sprintf(str,"Mode: %c",motoR_Data.Mode);
		oled_showString(0,48,(u8*)str,6,12);
		
		memset(str,0,22);
		sprintf(str,"RPM:%d",motoR_Data.RPM);
		oled_showString(64,16,(u8*)str,6,12);
		
		memset(str,0,22);
		sprintf(str,"MotoT:%d",motoR_Data.MotoTemp);
		oled_showString(64,32,(u8*)str,6,12);
		
		memset(str,0,22);
		sprintf(str,"DrvT:%d",motoR_Data.DriverTemp);
		oled_showString(64,48,(u8*)str,6,12);
	}
	else if(page%3==1)
	{
		memset(str,0,22);
		sprintf(str,"Volt:%.1f",motoR_Data.Voltage);
		oled_showString(0,16,(u8*)str,6,12);
		
		memset(str,0,22);
		sprintf(str,"Curr:%.1f",motoR_Data.Current);
		oled_showString(0,32,(u8*)str,6,12);
		
		memset(str,0,22);
		sprintf(str,"Dist:%.1f",motoR_Data.Distance);
		oled_showString(0,48,(u8*)str,6,12);
		
		memset(str,0,22);
		sprintf(str,"ErrC:%d",motoR_Data.ErrorCode);
		oled_showString(64,16,(u8*)str,6,12);
	}
	else if(page%3==2)
	{
		memset(str,0,22);
		sprintf(str,"TrqC:%.1f",motoR_Data.TorqueCtrl);
		oled_showString(0,16,(u8*)str,6,12);
		
		memset(str,0,22);
		sprintf(str,"RPMC:%d",motoR_Data.RPMCtrl);
		oled_showString(0,32,(u8*)str,6,12);
		
		memset(str,0,22);
		sprintf(str,"Trq:%.1f",motoR_Data.Torque);
		oled_showString(0,48,(u8*)str,6,12);
	}
	
	if(joystick1==RIGHT)
	{
		getKeyState();	//清除按键状态
		setShow_ui(MAIN_UI);
	}
	else if(joystick1==LEFT)
	{
		setShow_ui(MOTOF_UI);
	}	
}
