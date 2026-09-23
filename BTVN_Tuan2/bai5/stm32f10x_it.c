#include "stm32f10x_it.h"

extern char rx_buffer[];
extern volatile uint8_t rx_index;
extern volatile uint8_t rx_flag;

/* Hàm gửi 1 ký tự qua UART */
static void USART1_SendChar(char c)
{
    USART_SendData(USART1, c);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        char data = (char)USART_ReceiveData(USART1);

        /* Bỏ qua các ký tự xuống dòng thừa */
        if (data == '\r' || data == '\n')
        {
            return;
        }

        /* Echo back: Gửi lại chính ký tự vừa gõ lên màn hình PC để nhìn thấy chữ */
        USART1_SendChar(data);

        if (!rx_flag)
        {
            /* Khi nhận dấu ! -> Hoàn thành câu lệnh */
            if (data == '!')
            {
                rx_buffer[rx_index] = '\0';
                rx_flag = 1;
                /* Xuống dòng trên màn hình sau khi nhận xong lệnh ! */
                USART1_SendChar('\r');
                USART1_SendChar('\n');
            }
            else
            {
                if (rx_index < 63)
                {
                    rx_buffer[rx_index++] = data;
                }
            }
        }
    }
}
