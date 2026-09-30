#ifndef __BUTTON_H
#define __BUTTON_H

#include "stm32f1xx.h"
#include <stdint.h>

void Button_Init(void);
uint8_t Button_IsPressed(void);

#endif
