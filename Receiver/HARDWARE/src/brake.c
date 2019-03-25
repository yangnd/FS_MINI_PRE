#include "brake.h"

//初始化PF0/2/4为输出口.并使能这时钟
//BRAKE IO初始化
void BRAKE_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE); //使能PB,PE端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4; //端口配置0/2/4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                   //IO口速度为50MHz
    GPIO_Init(GPIOF, &GPIO_InitStructure);                              //根据设定参数初始化GPIOF.0/2/4
    BRAKE_EN = 0;
    BRAKE_IN1 = 0;
    BRAKE_IN2 = 0;
}
