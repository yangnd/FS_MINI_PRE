#ifndef __TASK_LORA_H
#define __TASK_LORA_H

/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

void lora_task_init(void);
void vLoraTask(void *param);

#endif
