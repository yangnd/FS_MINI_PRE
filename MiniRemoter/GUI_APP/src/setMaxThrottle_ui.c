#include "setMaxThrottle_ui.h"
#include "display.h"
#include "config_param.h"
#include "task_key.h"
#include <string.h>
#include <stdio.h>
#include "oled.h"

void setMaxThrottle_ui(void)
{
	s16 uMaxThrottle;
	enum dir_e joystick2;
	u8 keyState = getKeyState();
	char str[22];
	
	uMaxThrottle=configParam.car.maxthrottle;
	joystick2=getJoystick2Dir(0);
	if(joystick2==RIGHT) 
	{
		uMaxThrottle+=5;
	}
	else if(joystick2==LEFT) 
	{
		uMaxThrottle-=5;
	}
	if(uMaxThrottle>255) uMaxThrottle=255;
	else if(uMaxThrottle<0) uMaxThrottle=0;
	
	configParam.car.maxthrottle=uMaxThrottle;
	
	memset(str,0,22);
	sprintf(str,"Max THR");
	oled_showString(22,0,(u8*)str,12,24);
	oled_showNum(34,24,uMaxThrottle,4,12,24);
	memset(str,0,22);
	sprintf(str,"KEY_L");
	oled_showString(0,39,(u8*)str,6,12);
	memset(str,0,22);
	sprintf(str,"Exit");
	oled_showString(0,51,(u8*)str,6,12);
	sprintf(str,"KEY_R");
	oled_showString(97,39,(u8*)str,6,12);
	memset(str,0,22);
	sprintf(str,"Save&Exit");
	oled_showString(73,51,(u8*)str,6,12);
	
	if(keyState==KEY_R_SHORT_PRESS)	/*短按KEY_R保存并退出*/
	{
		writeConfigParamToFlash();
		setShow_ui(MAIN_UI);
	}
		else if(keyState == KEY_L_SHORT_PRESS)/*短按KEY_L退出*/
	{
		setShow_ui(MAIN_UI);
	}
	
	
}
