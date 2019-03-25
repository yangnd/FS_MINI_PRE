#include <stdlib.h>
#include "task_joystick.h"
#include "adc.h"


//ҡ���м��������ֵ��ADCֵ��
#define MID_DB_RH			150
#define MID_DB_THROTTLE		150
#define MID_DB_LH			150
#define MID_DB_BREAK		150


//ҡ��������������ֵ��ADCֵ��
#define DB_RANGE			10

//��ȡҡ�˷���ʱ�������м�ķ�Χֵ��ADCֵ��
#define DIR_MID_RH		800
#define DIR_MID_THROTTLE			800
#define DIR_MID_LH		800
#define DIR_MID_BREAK		800


static bool isInit;

/*ȥ����������*/
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

/*ҡ�˳�ʼ��*/
void joystickInit(void)
{
	if(isInit) return;
	Adc_Init();
	isInit = true;
}

/*��ȡҡ��ADCֵ*/
void getCtrlDataADCValue(uJoyStick *adcValue)
{
	adcValue->RH=getAdcValue(ADC_RH);
	adcValue->Throttle = getAdcValue(ADC_THROTTLE);
	adcValue->LH=getAdcValue(ADC_LH);
	adcValue->Break = getAdcValue(ADC_BREAK);	
}

/*ADCֵת���ɿ������ݰٷֱ�*/
void ADCtoCtrlDataPercent(fJoyStick *percent)
{
	s16 adcValue;
	//RH
	adcValue=getAdcValue(ADC_RH)-2048;		//��ȥ�м�ֵ
	adcValue=deadband(adcValue,MID_DB_RH);
	if(adcValue>=0)
		percent->RH=(float)adcValue/(4095-2048);
	else
		percent->RH=0;
	//Throttle
	adcValue=getAdcValue(ADC_THROTTLE)-2048;		//��ȥ�м�ֵ
	adcValue=deadband(adcValue,MID_DB_THROTTLE);
	if(adcValue>=0)
		percent->Throttle=(float)adcValue/(4095-2048);
	else
		percent->Throttle=0;
	//LH
	adcValue=getAdcValue(ADC_LH)-2048;		//��ȥ�м�ֵ
	adcValue=deadband(adcValue,MID_DB_RH);
	if(adcValue>=0)
		percent->LH=(float)adcValue/(4095-2048);
	else
		percent->LH=0;
	//Break
	adcValue=getAdcValue(ADC_BREAK)-2048;		//��ȥ�м�ֵ
	adcValue=deadband(adcValue,MID_DB_BREAK);
	if(adcValue>=0)
		percent->Break=(float)adcValue/(4095-2048);
	else
		percent->Break=0;
}
/*��ȡҡ��1����*/
/*mode:0,��֧��������;1,֧��������*/
enum dir_e getJoystick1Dir(u8 mode)
{
	enum dir_e ret=CENTER;
	uJoyStick adcValue;
	static bool havebackToCenter = true;
	
	getCtrlDataADCValue(&adcValue);
	if(mode) havebackToCenter = true;
	if(havebackToCenter == true)//ҡ�˻ص����м�λ��
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
		
		havebackToCenter = false;//ҡ���뿪���м�λ��
		if(ret == CENTER)//ҡ����Ȼ���м�λ��
			havebackToCenter = true;
	}
	else if( adcValue.Break >= (2048-DIR_MID_BREAK) &&
			 adcValue.Break <= (2048+DIR_MID_BREAK) &&
			 adcValue.LH >= (2048-DIR_MID_LH) &&
			 adcValue.LH <= (2048+DIR_MID_LH) )//ҡ���뿪���м�λ�ã����ڲ�ѯҡ���Ƿ����
	{
		havebackToCenter = true;
		ret = CENTER;
	}
	
	return ret;
}

/*��ȡҡ��2����*/
/*mode:0,��֧��������;1,֧��������*/
enum dir_e getJoystick2Dir(u8 mode)
{
	enum dir_e ret = CENTER;
	uJoyStick adcValue;
	static bool havebackToCenter = true;
	
	getCtrlDataADCValue(&adcValue);
	if(mode) havebackToCenter = true;
	if(havebackToCenter == true)//ҡ�˻ص����м�λ��
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

		havebackToCenter = false;//ҡ���뿪���м�λ��
		if(ret == CENTER)//ҡ����Ȼ���м�λ��
			havebackToCenter = true;
	}
	else if( adcValue.Throttle >= (2048-DIR_MID_THROTTLE) &&
			 adcValue.Throttle <= (2048+DIR_MID_THROTTLE) &&
			 adcValue.RH >= (2048-DIR_MID_RH) &&
			 adcValue.RH <= (2048+DIR_MID_RH) 
		   )//ҡ���뿪���м�λ�ã����ڲ�ѯҡ���Ƿ����
	{
		havebackToCenter = true;
		ret = CENTER;
	}
	
	return ret;
}

///*��ȡҡ��1����*/
///*mode:0,��֧��������;1,֧��������*/
//enum dir_e getJoystick1DirD(void)
//{
//	enum dir_e ret=CENTER;
//	uJoyStick adcValue;
//	static bool havebackToCenter = true;
//	
//	getCtrlDataADCValue(&adcValue);
////	if(mode) havebackToCenter = true;
//	if(havebackToCenter == true)//ҡ�˻ص����м�λ��
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
//		havebackToCenter = false;//ҡ���뿪���м�λ��
//		if(ret == CENTER)//ҡ����Ȼ���м�λ��
//			havebackToCenter = true;
//	}
//	else if( adcValue.Break >= (2048-DIR_MID_BREAK) &&
//			 adcValue.Break <= (2048+DIR_MID_BREAK) &&
//			 adcValue.LH >= (2048-DIR_MID_LH) &&
//			 adcValue.LH <= (2048+DIR_MID_LH) )//ҡ���뿪���м�λ�ã����ڲ�ѯҡ���Ƿ����
//	{
//		havebackToCenter = true;
//		ret = CENTER;
//	}
//	
//	return ret;
//}

///*��ȡҡ��2����*/
///*mode:0,��֧��������;1,֧��������*/
//enum dir_e getJoystick2DirD(void)
//{
//	enum dir_e ret = CENTER;
//	uJoyStick adcValue;
//	static bool havebackToCenter = true;
//	
//	getCtrlDataADCValue(&adcValue);
////	if(mode) havebackToCenter = true;
//	if(havebackToCenter == true)//ҡ�˻ص����м�λ��
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

//		havebackToCenter = false;//ҡ���뿪���м�λ��
//		if(ret == CENTER)//ҡ����Ȼ���м�λ��
//			havebackToCenter = true;
//	}
//	else if( adcValue.Throttle >= (2048-DIR_MID_THROTTLE) &&
//			 adcValue.Throttle <= (2048+DIR_MID_THROTTLE) &&
//			 adcValue.RH >= (2048-DIR_MID_RH) &&
//			 adcValue.RH <= (2048+DIR_MID_RH) 
//		   )//ҡ���뿪���м�λ�ã����ڲ�ѯҡ���Ƿ����
//	{
//		havebackToCenter = true;
//		ret = CENTER;
//	}
//	
//	return ret;
//}

///*��ȡҡ��1����*/
///*֧��������*/
//enum dir_e getJoystick1DirC(void)
//{
//	enum dir_e ret=CENTER;
//	uJoyStick adcValue;
//	static bool havebackToCenter = true;
//	
//	getCtrlDataADCValue(&adcValue);
//	havebackToCenter = true;
//	if(havebackToCenter == true)//ҡ�˻ص����м�λ��
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
//		havebackToCenter = false;//ҡ���뿪���м�λ��
//		if(ret == CENTER)//ҡ����Ȼ���м�λ��
//			havebackToCenter = true;
//	}
//	else if( adcValue.Break >= (2048-DIR_MID_BREAK) &&
//			 adcValue.Break <= (2048+DIR_MID_BREAK) &&
//			 adcValue.LH >= (2048-DIR_MID_LH) &&
//			 adcValue.LH <= (2048+DIR_MID_LH) )//ҡ���뿪���м�λ�ã����ڲ�ѯҡ���Ƿ����
//	{
//		havebackToCenter = true;
//		ret = CENTER;
//	}
//	
//	return ret;
//}

///*��ȡҡ��2����*/
///*֧��������*/
//enum dir_e getJoystick2DirC(void)
//{
//	enum dir_e ret = CENTER;
//	uJoyStick adcValue;
//	static bool havebackToCenter = true;
//	
//	getCtrlDataADCValue(&adcValue);
//	havebackToCenter = true;
//	if(havebackToCenter == true)//ҡ�˻ص����м�λ��
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

//		havebackToCenter = false;//ҡ���뿪���м�λ��
//		if(ret == CENTER)//ҡ����Ȼ���м�λ��
//			havebackToCenter = true;
//	}
//	else if( adcValue.Throttle >= (2048-DIR_MID_THROTTLE) &&
//			 adcValue.Throttle <= (2048+DIR_MID_THROTTLE) &&
//			 adcValue.RH >= (2048-DIR_MID_RH) &&
//			 adcValue.RH <= (2048+DIR_MID_RH) 
//		   )//ҡ���뿪���м�λ�ã����ڲ�ѯҡ���Ƿ����
//	{
//		havebackToCenter = true;
//		ret = CENTER;
//	}
//	
//	return ret;
//}
