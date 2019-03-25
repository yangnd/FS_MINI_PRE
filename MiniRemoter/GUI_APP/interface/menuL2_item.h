#ifndef __MENUL2_ITEM_H
#define __MENUL2_ITEM_H
#include "sys.h"
#include "gui_menu.h"

/********************************************************************************	 
 * All rights reserved
********************************************************************************/

extern MenuItem_Typedef  motoselMenu[];
extern MenuItem_Typedef  motomodeMenu[];
extern MenuItem_Typedef  languageMenu[];

extern const u8 radio_selected_img[32];
extern const u8  radio_unselected_img[32];



void motoselMenuInit(void);
void motomodeMenuInit(void);
void languageMenuInit(void);
//void flySpeedMenuInit(void);
//void flipEnableMenuInit(void);
//void expModuleMenuInit(void);

#endif /*__MENUL2_ITEM_H*/

