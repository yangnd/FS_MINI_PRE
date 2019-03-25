#ifndef __TASK_JOYSTICK_H
#define __TASK_JOYSTICK_H
#include "sys.h"
#include <stdbool.h>



#define MAX(a,b) a>=b? a:b
#define MIN(a,b) a<=b? a:b

enum dir_e
{
	CENTER,
	FORWARD,
	BACK,
	LEFT,
	RIGHT,
	BACK_LEFT,
	BACK_RIGHT,
};

//ҡ�˵��������̲���
struct range_s
{
	s16 range_pos;	//ҡ��������
	u16 mid;
	s16 range_neg;	//ҡ�˸�����
};
//ҡ�����̲���
typedef struct
{
	struct range_s RH;
	struct range_s Throttle;
	struct range_s LH;
	struct range_s Break;
}joystickParam_t;

typedef struct 
{
	u16 RH;
	u16 Throttle;
	u16 LH;
	u16 Break;
}uJoyStick;

typedef struct 
{
	float RH;
	float Throttle;
	float LH;
	float Break;
}fJoyStick;

void joystickInit(void);
void ADCtoCtrlDataPercent(fJoyStick *percent);
enum dir_e getJoystick1Dir(u8 mode);
enum dir_e getJoystick2Dir(u8 mode);
//enum dir_e getJoystick1DirD(void);
//enum dir_e getJoystick2DirD(void);
//enum dir_e getJoystick1DirC(void);
//enum dir_e getJoystick2DirC(void);

#endif /*__JOYSTICK_H*/
