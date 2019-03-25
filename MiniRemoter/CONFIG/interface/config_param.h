#ifndef __CONFIG_PARAM_H
#define __CONFIG_PARAM_H
#include <stdbool.h>
#include "sys.h"
#include "24l01.h"
#include "task_ctrldata.h"
#include "task_joystick.h"

/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 * ALIENTEK MiniFly
 * ���ò�����������	
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2018/6/1
 * �汾��V1.0
 * ��Ȩ���У�����ؾ���
 * Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
********************************************************************************/

#define BOOTLOADER_SIZE		(9*1024)		/*9K bootloader*/
#define CONFIG_PARAM_SIZE	(127*1024)		/*128K���ڱ������*/

#define CONFIG_PARAM_ADDR 	(FLASH_BASE + CONFIG_PARAM_SIZE)/*���ò��������ַ*/	
#define FIRMWARE_START_ADDR (FLASH_BASE + BOOTLOADER_SIZE)

/* Ĭ�����ò��� */
#define  VERSION	10		/*��ʾ�汾ΪV1.0*/
#define  DISPLAY_LANGUAGE	SIMPLE_CHINESE

#define  RADIO_CHANNEL 		40
#define  RADIO_DATARATE 	DATA_RATE_2M
#define  RADIO_ADDRESS 		0x0110104334ULL

enum motoID
{
	MOTO_FRONT,
	MOTO_REAR,
};
enum motoSel
{
	FRONT_ONLY,
	REAR_ONLY,
	FRONT_REAR,
};
enum controlMode
{
	THROTTLE,
	TORQUE,
	SPEED,
};
enum motoGear
{
	NONE,
	DRIVE,
	REVERSE,
	LOW,
};
enum language
{
	SIMPLE_CHINESE,
	ENGLISH,
	COMPLEX_CHINESE,
};

//��������
typedef struct{
	enum motoSel motosel;
	enum controlMode controlmode;
	u8 maxthrottle;
}carConfig_t;

/*�������ýṹ*/
typedef struct{
	u8 channel;		
	enum nrfRate dataRate;
	u32 addressHigh;/*ͨ�ŵ�ַ��4�ֽ�*/
	u32 addressLow;	/*ͨ�ŵ�ַ��4�ֽ�*/
}radioConfig_t;

/*��������ṹ*/
typedef struct{
	u8 version;				/*����汾��*/
	enum language language;	/*��ʾ����*/
	radioConfig_t radio;	/*�������ò���*/
	carConfig_t car;	/*�������ò���*/
	joystickParam_t jsParam;/*ҡ��У׼����*/
	u8 cksum;				/*У��*/
} configParam_t;


extern configParam_t configParam;


void configParamInit(void);
void configParamTask(void* param);
void writeConfigParamToFlash(void);
void configParamReset(void);

#endif

