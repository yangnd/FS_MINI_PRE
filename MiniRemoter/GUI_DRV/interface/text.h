#ifndef __TEXT_H
#define __TEXT_H 
#include <stdint.h>
//////////////////////////////////////////////////////////////////////////////////	 
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

/*�����ʾ���ֺ��ַ�*/
void show_str(uint16_t x,uint16_t y,const uint8_t*str,uint8_t f_w,uint8_t f_h,uint8_t mode);
/*���м���ʾ����ַ�*/
void show_str_mid(uint16_t x,uint16_t y,const uint8_t*str,uint8_t f_w,uint8_t f_h,uint8_t mode,uint16_t len);

#endif
