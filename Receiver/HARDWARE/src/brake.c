#include "brake.h"

//��ʼ��PF0/2/4Ϊ�����.��ʹ����ʱ��
//BRAKE IO��ʼ��
void BRAKE_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE); //ʹ��PB,PE�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4; //�˿�����0/2/4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                   //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOF, &GPIO_InitStructure);                              //�����趨������ʼ��GPIOF.0/2/4
    BRAKE_EN = 0;
    BRAKE_IN1 = 0;
    BRAKE_IN2 = 0;
}
