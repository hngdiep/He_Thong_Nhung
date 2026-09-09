#include <stdint.h>

#define SRAM_START  0x20000000U
#define SRAM_SIZE   (20U * 1024U) // 20KB RAM
#define SRAM_END    ((SRAM_START) + (SRAM_SIZE))
#define STACK_START SRAM_END

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

int main(void);

void Reset_Handler(void) {
    // Sao chép vùng .data từ Flash sang SRAM
    uint32_t size = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint8_t *pDst = (uint8_t*)&_sdata;
    uint8_t *pSrc = (uint8_t*)&_etext;
    for (uint32_t i = 0; i < size; i++) {
        *pDst++ = *pSrc++;
    }

    // Khởi tạo vùng .bss thành 0
    size = (uint32_t)&_ebss - (uint32_t)&_sbss;
    pDst = (uint8_t*)&_sbss;
    for (uint32_t i = 0; i < size; i++) {
        *pDst++ = 0;
    }

    // Nhảy vào hàm main
    main();

    while (1);
}

void Default_Handler(void) {
    while (1);
}

// Bảng Vector ngắt tối thiểu
__attribute__((section(".isr_vector")))
const uint32_t isr_vector[] = {
    STACK_START,
    (uint32_t)&Reset_Handler,
    (uint32_t)&Default_Handler, // NMI
    (uint32_t)&Default_Handler  // HardFault
};
