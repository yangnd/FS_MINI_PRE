#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define  ADC_BAT		0
#define  ADC_RH		1
#define  ADC_THROTTLE	2
#define  ADC_LH		3
#define  ADC_BREAK		4

//��ʼ��ADC��ʹ��DMA����
void Adc_Init(void);
void ADC_Filter(uint16_t* adc_val);	//ADC��ֵ�˲�
uint16_t getAdcValue(uint8_t axis);

#endif 
