#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
/********************************************************************************	 
 * All rights reserved
********************************************************************************/   	 

#define KEY_L  		GPIO_Pin_11
#define KEY_R  		GPIO_Pin_15
#define KEY_S1   	GPIO_Pin_10
#define KEY_S2   	GPIO_Pin_8

#define READ_KEY_L()  	GPIO_ReadInputDataBit(GPIOB,KEY_L)	//读取左按键
#define READ_KEY_R() 	GPIO_ReadInputDataBit(GPIOC,KEY_R)	//读取右按键
#define READ_KEY_J1()  	GPIO_ReadInputDataBit(GPIOB,KEY_S1)	//读取摇杆1按键
#define READ_KEY_J2()  	GPIO_ReadInputDataBit(GPIOA,KEY_S2)	//读取摇杆2按键

//IO初始化
void keyInit(void);

 //按键扫描函数		
void KEY_Scan(void);

#endif



