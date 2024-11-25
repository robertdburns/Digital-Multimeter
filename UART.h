#ifndef INCUART_H
#define INCUART_H
#include "main.h"

#include <stdlib.h>

#define BAUDRATE (24000000 / 115200)
#define VScale 0.81							// voltage needed to be scaled by 0.81 to be correct value
void UART_init(void);
void UART_format(uint16_t freq, uint16_t min, uint16_t max, uint32_t RMS, uint32_t avg);
void UART_print(char *piggy);
void USART_ESC_Code(char *esc_code);

char* numtoStrV(uint16_t value);
char* numtoStrF(uint16_t value);


#endif /* INC_UART_H */
