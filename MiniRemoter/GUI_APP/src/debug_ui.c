#include <string.h>
#include <stdio.h>
#include "debug_ui.h"
#include "oled.h"
#include "task_key.h"
#include "display.h"
#include "task_joystick.h"
#include "task_ctrldata.h"
#include "task_radio.h"
#include "main_ui.h"

/********************************************************************************	 
 * All rights reserved
********************************************************************************/

void debug_ui(void)
{
	char str[22];
	fJoyStick percent;
	
	percent = getPercent();
	
	memset(str,0,22);
	sprintf(str,"FNUM/TNUM:%4d/%4d",radioFailRxcount(),radioTxcount());
	oled_showString(0,0,(u8*)str,6,12);
	memset(str,0,22);
	sprintf(str,"BRK:%3.2f    THR:%3.2f",percent.Break,percent.Throttle);
	oled_showString(0,13,(u8*)str,6,8);
	memset(str,0,22);
	sprintf(str,"LH:%3.2f    RH:%3.2f",percent.LH,percent.RH);
	oled_showString(0,22,(u8*)str,6,8);
	
	memset(str,0,22);
	sprintf(str," BACK <--- ");
	oled_showString(0,34,(u8*)str,6,12);
	memset(str,0,22);
	sprintf(str,"Gear:%3d   RPM:%4d",0,0);
	if(strlen(str)>21) str[21]='\0';
	oled_showString(0,47,(u8*)str,6,8);
	memset(str,0,22);
	sprintf(str,"BRK:%3d    THR:%3d",0,0);
	if(strlen(str)>21) str[21]='\0';
	oled_showString(0,56,(u8*)str,6,8);
	
	/*按键处理*/
	u8 keyState = getKeyState();
	if(keyState == KEY_L_SHORT_PRESS)/*紧急停机*/
	{
		//To Do
//		sendRmotorCmd(CMD_EMER_STOP, NULL);
	}
	else if(keyState==KEY_R_SHORT_PRESS)/*退出调试界面*/
	{
		setShow_ui(MAIN_UI);
	}
}

