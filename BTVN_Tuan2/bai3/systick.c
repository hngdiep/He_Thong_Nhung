#include "systick.h"

void SysTick_Init(void)
{
    /*
     * CPU = 8 MHz
     *
     * 8,000,000 tick = 1 giay
     */
    SysTick->LOAD = 8000000 - 1;

    SysTick->VAL = 0;

    /*
     * Bit 2 = CPU clock
     * Bit 1 = 0 -> KHONG dung interrupt
     * Bit 0 = Enable
     */
    SysTick->CTRL =
          (1 << 2)
        | (1 << 0);
}
