#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"

// Sử dụng chân PA4 làm CS (NSS SPI1)
#define MAX7219_CS_PORT GPIOA
#define MAX7219_CS_PIN  GPIO_Pin_4

void SPI1_Init_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    // Bật Clock cho GPIOA và SPI1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    // Cấu hình PA5 (SCK) và PA7 (MOSI)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Cấu hình PA4 (CS)
    GPIO_InitStructure.GPIO_Pin = MAX7219_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MAX7219_CS_PORT, &GPIO_InitStructure);
    
    GPIO_SetBits(MAX7219_CS_PORT, MAX7219_CS_PIN);

    // Cấu hình thông số SPI1
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
}

void MAX7219_Write(uint8_t address, uint8_t data) {
    uint16_t sendData = (address << 8) | data;

    GPIO_ResetBits(MAX7219_CS_PORT, MAX7219_CS_PIN);

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, sendData);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);

    GPIO_SetBits(MAX7219_CS_PORT, MAX7219_CS_PIN);
}

void MAX7219_Clear(void) {
    for (uint8_t i = 1; i <= 8; i++) {
        MAX7219_Write(i, 0x00);
    }
}

void MAX7219_Init(void) {
    MAX7219_Write(0x09, 0x00); // Decode mode: None
    MAX7219_Write(0x0A, 0x03); // Độ sáng trung bình
    MAX7219_Write(0x0B, 0x07); // Quét cả 8 hàng
    MAX7219_Write(0x0C, 0x01); // Bật nguồn hoạt động (Normal Operation)
    MAX7219_Write(0x0F, 0x00); // Tắt chế độ test
    MAX7219_Clear();            // Xóa sạch màn hình ban đầu
}

void Delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 4000; i++) {
        __NOP();
    }
}

int main(void) {
    SPI1_Init_Config();
    MAX7219_Init();

    while (1) {
        // Lặp qua từng hàng từ trên xuống dưới (Hàng 1 -> Hàng 8)
        for (uint8_t row = 1; row <= 8; row++) {
            
            // Lặp từng điểm LED trên hàng đó từ trái sang phải (Bit 7 -> Bit 0)
            for (int col = 7; col >= 0; col--) {
                MAX7219_Clear();                   // Xóa các LED cũ
                MAX7219_Write(row, (1 << col));    // Sáng 1 điểm LED tại vị trí (row, col)
                Delay_ms(100);                     // Tốc độ chuyển điểm LED (100ms)
            }
        }
    }
}
