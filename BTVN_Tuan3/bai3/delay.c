#include "delay.h"

void Delay_ms(uint32_t ms)
{
    volatile uint32_t i;

    while (ms--)
    {
        for (i = 0; i < 800; i++)
        {
            __NOP();
        }
    }
}
