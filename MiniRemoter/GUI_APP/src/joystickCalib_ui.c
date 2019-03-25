#include <string.h>
#include <stdio.h>
#include "joystickCalib_ui.h"
#include "task_key.h"
#include "display.h"
#include "task_joystick.h"
#include "text.h"
#include "config_param.h"
#include "oled.h"

/********************************************************************************	 
 * All rights reserved
********************************************************************************/

const unsigned char* stepLanguage1[3]={
{"1.摇动两个摇杆一圈"},
{"1.Rock two joysticks   a circle"},
{"1.搖動兩個搖桿一圈"},
};

const unsigned char* stepLanguage2[3]={
{"2.按KEY_R下一步"},
{"2.Press KEY_R next"},
{"2.按KEY_R下一步"},
};

const unsigned char* stepLanguage3[3]={
{"3.保持摇杆回中"},
{"3.Keep joysticks on    center"},
{"3.保持搖桿回中"},
};

const unsigned char* stepLanguage4[3]={
{"4.按KEY_R完成"},
{"4.Press KEY_R finish"},
{"4.按KEY_R完成"},
};

/*摇杆校准界面*/
void joystickCalib_ui(void)
{
	static u8 steps;
	char str[30];
	static s16 max[4],min[4] = {1000,1000,1000,1000};
	u8 y;
	joystickParam_t jsParam;
	uJoyStick adcValue;
	
	getCtrlDataADCValue(&adcValue);
	if(steps==0)
	{
		oled_fill(0,0,127,38,0);
		y = 14;
		if(configParam.language == ENGLISH) 
			y = 26;
		show_str(0,0,stepLanguage1[configParam.language],12,12,1);
		show_str(0,y,stepLanguage2[configParam.language],12,12,1);
					
		//roll
		max[0] = MAX(max[0],adcValue.RH);
		min[0] = MIN(min[0],adcValue.RH);	
		//pitch
		max[1] = MAX(max[1],adcValue.Throttle);
		min[1] = MIN(min[1],adcValue.Throttle);
		//yaw
		max[2] = MAX(max[2],adcValue.LH);
		min[2] = MIN(min[2],adcValue.LH);
		//thrust
		max[3] = MAX(max[3],adcValue.Break);
		min[3] = MIN(min[3],adcValue.Break);
	}
	if(steps==1)
	{
		oled_fill(0,0,127,38,0);
		y = 14;
		if(configParam.language == ENGLISH) 
			y = 26;
		show_str(0,0,stepLanguage3[configParam.language],12,12,1);
		show_str(0,y,stepLanguage4[configParam.language],12,12,1);
		
		jsParam.RH.mid = adcValue.RH;
		jsParam.Throttle.mid = adcValue.Throttle;
		jsParam.LH.mid = adcValue.LH;
		jsParam.Break.mid = adcValue.Break;
		
		jsParam.RH.range_pos = max[0] - jsParam.RH.mid;
		jsParam.RH.range_neg = jsParam.RH.mid - min[0];	
		
		jsParam.Throttle.range_pos = max[1] - jsParam.Throttle.mid;
		jsParam.Throttle.range_neg = jsParam.Throttle.mid - min[1];
		
		jsParam.LH.range_pos = max[2] - jsParam.LH.mid;
		jsParam.LH.range_neg = jsParam.LH.mid - min[2];
		
		jsParam.Break.range_pos = max[3] - jsParam.Break.mid;
		jsParam.Break.range_neg = jsParam.Break.mid - min[3];
	}
	
	memset(str,0,22);
	sprintf(str,"BRK:%4d    THR:%4d",adcValue.Break,adcValue.Throttle);
	oled_showString(0,44,(u8*)str,6,8);
	memset(str,0,22);
	sprintf(str,"LH:%4d    RH:%4d",adcValue.LH,adcValue.RH);
	oled_showString(0,54,(u8*)str,6,8);	
	
	u8 keyState = getKeyState();
	if(keyState == KEY_R_SHORT_PRESS)/*短按KEY_R进入下一步*/
	{
		steps++;
		if(steps==2)
		{
			steps=0;
			configParam.jsParam = jsParam;
			writeConfigParamToFlash();
			for(u8 i=0; i<4; i++)
			{
				min[i] = 1000;
				max[i] = 1000;
			}
			setShow_ui(MAIN_UI);
		}
	}
	else if(keyState == KEY_L_SHORT_PRESS)/*短按KEY_L退出校准*/
	{
		steps=0;
		setShow_ui(MAIN_UI);
	}
}
