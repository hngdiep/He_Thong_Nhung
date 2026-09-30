#ifndef __UART_DMA_H
#define __UART_DMA_H

#include "stm32f1xx.h"
#include <stdint.h>

void UART1_DMA_Init(void);
void UART1_DMA_Service(void);

uint8_t UART1_DMA_Send(char *data, uint16_t len);
uint8_t UART1_DMA_IsBusy(void);

#endif
