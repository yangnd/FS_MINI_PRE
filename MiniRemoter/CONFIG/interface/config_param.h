#ifndef __CONFIG_PARAM_H
#define __CONFIG_PARAM_H
#include <stdbool.h>
#include "sys.h"
#include "24l01.h"
#include "task_ctrldata.h"
#include "task_joystick.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly
 * 配置参数驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

#define BOOTLOADER_SIZE		(9*1024)		/*9K bootloader*/
#define CONFIG_PARAM_SIZE	(127*1024)		/*128K用于保存参数*/

#define CONFIG_PARAM_ADDR 	(FLASH_BASE + CONFIG_PARAM_SIZE)/*配置参数保存地址*/	
#define FIRMWARE_START_ADDR (FLASH_BASE + BOOTLOADER_SIZE)

/* 默认配置参数 */
#define  VERSION	10		/*表示版本为V1.0*/
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

//车辆配置
typedef struct{
	enum motoSel motosel;
	enum controlMode controlmode;
	u8 maxthrottle;
}carConfig_t;

/*无线配置结构*/
typedef struct{
	u8 channel;		
	enum nrfRate dataRate;
	u32 addressHigh;/*通信地址高4字节*/
	u32 addressLow;	/*通信地址低4字节*/
}radioConfig_t;

/*保存参数结构*/
typedef struct{
	u8 version;				/*软件版本号*/
	enum language language;	/*显示语言*/
	radioConfig_t radio;	/*无线配置参数*/
	carConfig_t car;	/*飞行配置参数*/
	joystickParam_t jsParam;/*摇杆校准参数*/
	u8 cksum;				/*校验*/
} configParam_t;


extern configParam_t configParam;


void configParamInit(void);
void configParamTask(void* param);
void writeConfigParamToFlash(void);
void configParamReset(void);

#endif

