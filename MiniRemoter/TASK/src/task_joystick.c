#include <stdlib.h>
#include "task_joystick.h"
#include "adc.h"


//摇杆中间软件死区值（ADC值）
#define MID_DB_RH			150
#define MID_DB_THROTTLE		150
#define MID_DB_LH			150
#define MID_DB_BREAK		150


//摇杆上下量程死区值（ADC值）
#define DB_RANGE			10

//获取摇杆方向时定义在中间的范围值（ADC值）
#define DIR_MID_RH		800
#define DIR_MID_THROTTLE			800
#define DIR_MID_LH		800
#define DIR_MID_BREAK		800


static bool isInit;

/*去除死区函数*/
int deadband(int value, const int threshold)
{
	if (abs(value) < threshold)
	{
		value = 0;
	}
	else if (value > 0)
	{
		value -= threshold;
	}
	else if (value < 0)
	{
		value += threshold;
	}
	return value;
}

/*摇杆初始化*/
void joystickInit(void)
{
	if(isInit) return;
	Adc_Init();
	isInit = true;
}

/*获取摇杆ADC值*/
void getCtrlDataADCValue(uJoyStick *adcValue)
{
	adcValue->RH=getAdcValue(ADC_RH);
	adcValue->Throttle = getAdcValue(ADC_THROTTLE);
	adcValue->LH=getAdcValue(ADC_LH);
	adcValue->Break = getAdcValue(ADC_BREAK);	
}

/*ADC值转换成控制数据百分比*/
void ADCtoCtrlDataPercent(fJoyStick *percent)
{
	s16 adcValue;
	//RH
	adcValue=getAdcValue(ADC_RH)-2048;		//减去中间值
	adcValue=deadband(adcValue,MID_DB_RH);
	if(adcValue>=0)
		percent->RH=(float)adcValue/(4095-2048);
	else
		percent->RH=0;
	//Throttle
	adcValue=getAdcValue(ADC_THROTTLE)-2048;		//减去中间值
	adcValue=deadband(adcValue,MID_DB_THROTTLE);
	if(adcValue>=0)
		percent->Throttle=(float)adcValue/(4095-2048);
	else
		percent->Throttle=0;
	//LH
	adcValue=getAdcValue(ADC_LH)-2048;		//减去中间值
	adcValue=deadband(adcValue,MID_DB_RH);
	if(adcValue>=0)
		percent->LH=(float)adcValue/(4095-2048);
	else
		percent->LH=0;
	//Break
	adcValue=getAdcValue(ADC_BREAK)-2048;		//减去中间值
	adcValue=deadband(adcValue,MID_DB_BREAK);
	if(adcValue>=0)
		percent->Break=(float)adcValue/(4095-2048);
	else
		percent->Break=0;
}
/*获取摇杆1方向*/
/*mode:0,不支持连续按;1,支持连续按*/
enum dir_e getJoystick1Dir(u8 mode)
{
	enum dir_e ret=CENTER;
	uJoyStick adcValue;
	static bool havebackToCenter = true;
	
	getCtrlDataADCValue(&adcValue);
	if(mode) havebackToCenter = true;
	if(havebackToCenter == true)//摇杆回到过中间位置
	{
		if(adcValue.Break > (2048+DIR_MID_BREAK))
			ret = FORWARD;
		else if(adcValue.Break < (2048-DIR_MID_BREAK))
			ret = BACK;
		
		if(ret==BACK && adcValue.LH>(2048+DIR_MID_LH))
			ret = BACK_RIGHT;
		else if(ret==BACK && adcValue.LH<(2048-DIR_MID_LH))
			ret = BACK_LEFT;
		else if(adcValue.LH > (2048+DIR_MID_LH))
			ret = RIGHT;
		else if(adcValue.LH < (2048-DIR_MID_LH))
			ret = LEFT;
		
		havebackToCenter = false;//摇杆离开了中间位置
		if(ret == CENTER)//摇杆依然在中间位置
			havebackToCenter = true;
	}
	else if( adcValue.Break >= (2048-DIR_MID_BREAK) &&
			 adcValue.Break <= (2048+DIR_MID_BREAK) &&
			 adcValue.LH >= (2048-DIR_MID_LH) &&
			 adcValue.LH <= (2048+DIR_MID_LH) )//摇杆离开了中间位置，现在查询摇杆是否回中
	{
		havebackToCenter = true;
		ret = CENTER;
	}
	
	return ret;
}

/*获取摇杆2方向*/
/*mode:0,不支持连续按;1,支持连续按*/
enum dir_e getJoystick2Dir(u8 mode)
{
	enum dir_e ret = CENTER;
	uJoyStick adcValue;
	static bool havebackToCenter = true;
	
	getCtrlDataADCValue(&adcValue);
	if(mode) havebackToCenter = true;
	if(havebackToCenter == true)//摇杆回到过中间位置
	{	
		if(adcValue.Throttle > (2048+DIR_MID_THROTTLE))
			ret = FORWARD;
		else if(adcValue.Throttle < (2048-DIR_MID_THROTTLE))
			ret = BACK;
		
		if(ret==BACK && adcValue.RH>(2048+DIR_MID_RH))
			ret = BACK_RIGHT;
		else if(ret==BACK && adcValue.RH<(2048-DIR_MID_RH))
			ret = BACK_LEFT;
		else if(adcValue.RH>(2048+DIR_MID_RH))
			ret = RIGHT;
		else if(adcValue.RH<(2048-DIR_MID_RH))
			ret = LEFT;

		havebackToCenter = false;//摇杆离开了中间位置
		if(ret == CENTER)//摇杆依然在中间位置
			havebackToCenter = true;
	}
	else if( adcValue.Throttle >= (2048-DIR_MID_THROTTLE) &&
			 adcValue.Throttle <= (2048+DIR_MID_THROTTLE) &&
			 adcValue.RH >= (2048-DIR_MID_RH) &&
			 adcValue.RH <= (2048+DIR_MID_RH) 
		   )//摇杆离开了中间位置，现在查询摇杆是否回中
	{
		havebackToCenter = true;
		ret = CENTER;
	}
	
	return ret;
}

///*获取摇杆1方向*/
///*mode:0,不支持连续按;1,支持连续按*/
//enum dir_e getJoystick1DirD(void)
//{
//	enum dir_e ret=CENTER;
//	uJoyStick adcValue;
//	static bool havebackToCenter = true;
//	
//	getCtrlDataADCValue(&adcValue);
////	if(mode) havebackToCenter = true;
//	if(havebackToCenter == true)//摇杆回到过中间位置
//	{
//		if(adcValue.Break > (2048+DIR_MID_BREAK))
//			ret = FORWARD;
//		else if(adcValue.Break < (2048-DIR_MID_BREAK))
//			ret = BACK;
//		
//		if(ret==BACK && adcValue.LH>(2048+DIR_MID_LH))
//			ret = BACK_RIGHT;
//		else if(ret==BACK && adcValue.LH<(2048-DIR_MID_LH))
//			ret = BACK_LEFT;
//		else if(adcValue.LH > (2048+DIR_MID_LH))
//			ret = RIGHT;
//		else if(adcValue.LH < (2048-DIR_MID_LH))
//			ret = LEFT;
//		
//		havebackToCenter = false;//摇杆离开了中间位置
//		if(ret == CENTER)//摇杆依然在中间位置
//			havebackToCenter = true;
//	}
//	else if( adcValue.Break >= (2048-DIR_MID_BREAK) &&
//			 adcValue.Break <= (2048+DIR_MID_BREAK) &&
//			 adcValue.LH >= (2048-DIR_MID_LH) &&
//			 adcValue.LH <= (2048+DIR_MID_LH) )//摇杆离开了中间位置，现在查询摇杆是否回中
//	{
//		havebackToCenter = true;
//		ret = CENTER;
//	}
//	
//	return ret;
//}

///*获取摇杆2方向*/
///*mode:0,不支持连续按;1,支持连续按*/
//enum dir_e getJoystick2DirD(void)
//{
//	enum dir_e ret = CENTER;
//	uJoyStick adcValue;
//	static bool havebackToCenter = true;
//	
//	getCtrlDataADCValue(&adcValue);
////	if(mode) havebackToCenter = true;
//	if(havebackToCenter == true)//摇杆回到过中间位置
//	{	
//		if(adcValue.Throttle > (2048+DIR_MID_THROTTLE))
//			ret = FORWARD;
//		else if(adcValue.Throttle < (2048-DIR_MID_THROTTLE))
//			ret = BACK;
//		
//		if(ret==BACK && adcValue.RH>(2048+DIR_MID_RH))
//			ret = BACK_RIGHT;
//		else if(ret==BACK && adcValue.RH<(2048-DIR_MID_RH))
//			ret = BACK_LEFT;
//		else if(adcValue.RH>(2048+DIR_MID_RH))
//			ret = RIGHT;
//		else if(adcValue.RH<(2048-DIR_MID_RH))
//			ret = LEFT;

//		havebackToCenter = false;//摇杆离开了中间位置
//		if(ret == CENTER)//摇杆依然在中间位置
//			havebackToCenter = true;
//	}
//	else if( adcValue.Throttle >= (2048-DIR_MID_THROTTLE) &&
//			 adcValue.Throttle <= (2048+DIR_MID_THROTTLE) &&
//			 adcValue.RH >= (2048-DIR_MID_RH) &&
//			 adcValue.RH <= (2048+DIR_MID_RH) 
//		   )//摇杆离开了中间位置，现在查询摇杆是否回中
//	{
//		havebackToCenter = true;
//		ret = CENTER;
//	}
//	
//	return ret;
//}

///*获取摇杆1方向*/
///*支持连续按*/
//enum dir_e getJoystick1DirC(void)
//{
//	enum dir_e ret=CENTER;
//	uJoyStick adcValue;
//	static bool havebackToCenter = true;
//	
//	getCtrlDataADCValue(&adcValue);
//	havebackToCenter = true;
//	if(havebackToCenter == true)//摇杆回到过中间位置
//	{
//		if(adcValue.Break > (2048+DIR_MID_BREAK))
//			ret = FORWARD;
//		else if(adcValue.Break < (2048-DIR_MID_BREAK))
//			ret = BACK;
//		
//		if(ret==BACK && adcValue.LH>(2048+DIR_MID_LH))
//			ret = BACK_RIGHT;
//		else if(ret==BACK && adcValue.LH<(2048-DIR_MID_LH))
//			ret = BACK_LEFT;
//		else if(adcValue.LH > (2048+DIR_MID_LH))
//			ret = RIGHT;
//		else if(adcValue.LH < (2048-DIR_MID_LH))
//			ret = LEFT;
//		
//		havebackToCenter = false;//摇杆离开了中间位置
//		if(ret == CENTER)//摇杆依然在中间位置
//			havebackToCenter = true;
//	}
//	else if( adcValue.Break >= (2048-DIR_MID_BREAK) &&
//			 adcValue.Break <= (2048+DIR_MID_BREAK) &&
//			 adcValue.LH >= (2048-DIR_MID_LH) &&
//			 adcValue.LH <= (2048+DIR_MID_LH) )//摇杆离开了中间位置，现在查询摇杆是否回中
//	{
//		havebackToCenter = true;
//		ret = CENTER;
//	}
//	
//	return ret;
//}

///*获取摇杆2方向*/
///*支持连续按*/
//enum dir_e getJoystick2DirC(void)
//{
//	enum dir_e ret = CENTER;
//	uJoyStick adcValue;
//	static bool havebackToCenter = true;
//	
//	getCtrlDataADCValue(&adcValue);
//	havebackToCenter = true;
//	if(havebackToCenter == true)//摇杆回到过中间位置
//	{	
//		if(adcValue.Throttle > (2048+DIR_MID_THROTTLE))
//			ret = FORWARD;
//		else if(adcValue.Throttle < (2048-DIR_MID_THROTTLE))
//			ret = BACK;
//		
//		if(ret==BACK && adcValue.RH>(2048+DIR_MID_RH))
//			ret = BACK_RIGHT;
//		else if(ret==BACK && adcValue.RH<(2048-DIR_MID_RH))
//			ret = BACK_LEFT;
//		else if(adcValue.RH>(2048+DIR_MID_RH))
//			ret = RIGHT;
//		else if(adcValue.RH<(2048-DIR_MID_RH))
//			ret = LEFT;

//		havebackToCenter = false;//摇杆离开了中间位置
//		if(ret == CENTER)//摇杆依然在中间位置
//			havebackToCenter = true;
//	}
//	else if( adcValue.Throttle >= (2048-DIR_MID_THROTTLE) &&
//			 adcValue.Throttle <= (2048+DIR_MID_THROTTLE) &&
//			 adcValue.RH >= (2048-DIR_MID_RH) &&
//			 adcValue.RH <= (2048+DIR_MID_RH) 
//		   )//摇杆离开了中间位置，现在查询摇杆是否回中
//	{
//		havebackToCenter = true;
//		ret = CENTER;
//	}
//	
//	return ret;
//}
