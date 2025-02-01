#ifndef DEFINES_H
#define DEFINES_H

#include "sdkconfig.h"

/** Button defines */
#define BUTTON_NUM 3
#if 0
#define BUTTON_A CONFIG_BUTTON_A
#define BUTTON_B CONFIG_BUTTON_B
#define BUTTON_C CONFIG_BUTTON_C
#else
#define BUTTON_A 39
#define BUTTON_B 38
#define BUTTON_C 37

#define BUTTON_A_INDEX 0u
#define BUTTON_B_INDEX 1u
#define BUTTON_C_INDEX 2u

#endif

/** Menu defines */
#define MENU_ITEM_NUM 4

#endif // DEFINES_H
