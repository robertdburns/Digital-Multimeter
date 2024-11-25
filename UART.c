#include "UART.h"


void UART_init(void) {
    /* NUCLEO-L476
    * PA2 (TX)
    * PA3 (RX)
    *
    */

    // clock enable
    RCC -> APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    RCC -> AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // GPIO pins: PA2 and PA3
    GPIOA -> MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
    GPIOA -> MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1); // alt func
    GPIOA -> AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos) | (7 << GPIO_AFRL_AFSEL3_Pos);

    // USART config: TX, RX, USART
    USART2 -> BRR = BAUDRATE;
    USART2 -> CR1 = (USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);
}

void UART_format(uint16_t freq, uint16_t min, uint16_t max, uint32_t RMS, uint32_t avg) {

	USART_ESC_Code("[H");							// move cursor to upper left

	UART_print("==============================|DC|==============================");
	USART_ESC_Code("[H");							// move cursor to upper left
	USART_ESC_Code("[1B");							// move cursor down 1
	UART_print("AVG: ");
	char* avgStr = numtoStrV(avg);
	UART_print(avgStr);								// Print the converted string
	free(avgStr); 									// Free dynamically allocated memory

	USART_ESC_Code("[H");							// move cursor to upper left
	USART_ESC_Code("[2B");							// move cursor down 2
	USART_ESC_Code("[2k");
	USART_ESC_Code("[k");
	uint8_t numHashAVG = avg / 100 + 1;				// calculation for number of #
	for (int i = 0; i < numHashAVG; i++) {			// print # per 100mv
		UART_print("#");
	}
	for (int i = 0; i < 33 - numHashAVG; i++) {		// print remainder with _ (33 - num #)
		UART_print("_");
	}

	USART_ESC_Code("[H");							// move cursor to upper left
	USART_ESC_Code("[3B");							// move cursor down 3
	UART_print("|----|----|----|----|----|----|--|");

	USART_ESC_Code("[H");							// move cursor to upper left
	USART_ESC_Code("[4B");							// move cursor down 4
	UART_print("0   0.5   1   1.5   2   2.5   3 3.3");

	USART_ESC_Code("[H");							// move cursor to upper left
	USART_ESC_Code("[7B");							// move cursor down 5
	UART_print("==============================|AC|==============================");

	USART_ESC_Code("[H");							// move cursor to upper left
	USART_ESC_Code("[8B");							// move cursor down 6
	UART_print("RMS: ");
	char* rmsStr = numtoStrV(RMS);
	UART_print(rmsStr); 							// Print the converted string
	free(rmsStr); 									// Free dynamically allocated memory

	USART_ESC_Code("[H");							// move cursor to upper left
	USART_ESC_Code("[9B");							// move cursor down 7
	USART_ESC_Code("[2k");
	USART_ESC_Code("[0k");
	uint8_t numHashRMS = RMS / 100 + 1;				// calculation for number of #
	for (int i = 0; i < numHashRMS; i++) {			// print # per 100mv
		UART_print("#");
	}
	for (int i = 0; i < 33 - numHashRMS; i++) {		// print remainder with _ (33 - num #)
		UART_print("_");
	}


	USART_ESC_Code("[H");							// move cursor to upper left
	USART_ESC_Code("[10B");							// move cursor down 8
	UART_print("|----|----|----|----|----|----|--|");

	USART_ESC_Code("[H");							// move cursor to upper left
	USART_ESC_Code("[11B");							// move cursor down 9
	UART_print("0   0.5   1   1.5   2   2.5   3 3.3");

	USART_ESC_Code("[H");							// move cursor to upper left
	USART_ESC_Code("[12B");							// move cursor down 6
	UART_print("FREQ: ");
	char *freqS = numtoStrF(freq); 					// numtoStr should allocate memory
	UART_print(freqS); 								// Print the converted string
	free(freqS); 									// Free dynamically allocated memory
	UART_print("Hz");

	USART_ESC_Code("[H");							// move cursor to upper left
	USART_ESC_Code("[13B");							// move cursor down 6
	UART_print("Vpp: ");
	uint16_t vpp = (max - min) * VScale;
	char* vppStr = numtoStrV(vpp);
	UART_print(vppStr); 							// Print the converted string
	free(vppStr); 									// Free dynamically allocated memory

}

char* numtoStrV(uint16_t value) {					// for VOLTAGE printing
    // Dynamically allocate memory for the string (5 bytes: 4 digits + null terminator)
    char* list = (char*)malloc(5 * sizeof(char));
    if (list == NULL) {
        return NULL;								// If malloc fails, return NULL
    }

    int thous = value / 1000;
    list[4] = '\0'; 								// Null-terminate the string
    list[0] = '0' + thous; 							// Add thousands place

    list[1] = '.'; 									// Add hundreds place
    int huns = (value / 100) % 10;
    list[2] = '0' + huns;

    int tens = (value - (thous * 1000) - (huns * 100)) / 10;
    list[3] = '0' + tens; 							// Add ones place

    return list; 									// Return the dynamically allocated pointer
}

char* numtoStrF(uint16_t value) {					// for FREQUENCY printing
    // Dynamically allocate memory for the string (5 bytes: 4 digits + null terminator)
    char* list = (char*)malloc(5 * sizeof(char));
    if (list == NULL) {
        return NULL;	        					// If malloc fails, return NULL
    }

    int thous = value / 1000;
    list[4] = '\0'; 								// Null-terminate the string
    list[0] = '0' + thous; 							// Add thousands place

    int huns = (value - (thous * 1000)) / 100;
    list[1] = '0' + huns; 							// Add hundreds place

    int tens = (value - (thous * 1000) - (huns * 100)) / 10;
    list[2] = '0' + tens; 							// Add tens place

    int ones = (value - (thous * 1000) - (huns * 100) - (tens * 10));
    list[3] = '0' + ones; 							// Add ones place

    return list; 									// Return the dynamically allocated pointer
}




void UART_print(char *piggy) {
    while (*piggy) {
        // wait to send
        while (!(USART2 -> ISR & USART_ISR_TXE));
        USART2 -> TDR = *piggy++;
    }
}

void USART_ESC_Code(char *esc_code) {
    UART_print("\x1B"); // escape character \x1b
    UART_print(esc_code);
}
