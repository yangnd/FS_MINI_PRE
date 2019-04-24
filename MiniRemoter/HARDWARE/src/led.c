#include "led.h"

/********************************************************************************	 
 * All rights reserved
********************************************************************************/	   

/* LED初始化 */
void ledInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	/* 禁止JTA使能SWD口 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	/* 初始化LED_BLUE(PB3) LED_RED(PB7) */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_7;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* 关闭所有LED */
	GPIO_SetBits(GPIOB,GPIO_Pin_3);			
	GPIO_SetBits(GPIOB,GPIO_Pin_7);
}
 
