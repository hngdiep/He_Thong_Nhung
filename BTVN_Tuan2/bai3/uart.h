#ifndef UART_H
#define UART_H

#include "stm32f1xx.h"
#include <stdint.h>

void UART1_Init(void);

void UART1_SendChar(char c);
void UART1_SendString(const char *str);
void UART1_SendNumber(uint32_t number);

#endif
