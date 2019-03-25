#ifndef __TASK_KEY_H
#define __TASK_KEY_H	 
#include "sys.h"
#include "key.h"

/********************************************************************************	 
 * All rights reserved
********************************************************************************/
#define PRESSED		0
#define RELEASED	1
//°´¼ü×´Ì¬
#define KEY_L_SHORT_PRESS	1	
#define KEY_L_LONG_PRESS	2
#define KEY_R_SHORT_PRESS	3	
#define KEY_R_LONG_PRESS	4	
#define KEY_J1_SHORT_PRESS	5	
#define KEY_J1_LONG_PRESS	6
#define KEY_J2_SHORT_PRESS	7
#define KEY_J2_LONG_PRESS	8

void keyTask(void* param);
u8 getKeyState(void);

#endif



