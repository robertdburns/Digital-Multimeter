#include "main.h"
#include "ADC.h"
#include "UART.h"
#include "math.h"

#define ARRVal 5
#define SYSCLK 24000000
#define PRESC    2400 -1
#define INTCLK (SYSCLK/ARRVal)
#define VScale 0.82
#define VMax 4095


uint8_t flag = 1;

void calculation_function(uint16_t array[]);
void TIM_init();

void SystemClock_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    ADC_init();
    UART_init();
    TIM_init();

    // clear the screen
    USART_ESC_Code("[2J");
    USART_ESC_Code("[H");
    USART_ESC_Code("[?25l");

    // start a conversion
    ADC1 -> CR |= ADC_CR_ADSTART;

    while (1)
    {

        }


}


void calculation_function(uint16_t array[])
{
    uint64_t sum = 0;
    uint64_t RMS = 0;
    uint16_t max = 0;										// start max low, increase from there
    uint16_t min = VMax;									// start min high, reduce from there

    uint16_t crossings = 0;

    for (int i = 0; i < SIZE; i++){
    	if (array[i] > VMax) {								// since int is unsigned, if it is negative it will
    		array[i] = 0;									// underflow and become big, this makes it = 0
    	}
        sum += array[i];									// add for sum
        RMS += ((uint64_t)array[i] * (uint64_t)array[i]);	// square for RMS

        if (array[i] > max) {								// update MAX
        	max = array[i];
        }

        if (array[i] < min) {								// update min
        	min = array[i];
        }
    }

    RMS = RMS / SIZE;										// divide RMS by n
    RMS = sqrt(RMS);										// square root to get final RMS
    uint16_t avg = (sum/SIZE);								// get avg = sum / n

    for(int i = 1; i < SIZE ;i++){							// get # times the val crosses the middle
        // crossing going down
        if (array[i - 1] > avg && array[i] <= avg){
            crossings += 1;
        }
        // crossing going up
        else if (array[i - 1] < avg && array[i] >= avg){
            crossings += 1;
        }
    }

    uint16_t freak = crossings/2;							// 2 crossings in each period, thus /2

    UART_format(freak, min, max, RMS*VScale, avg*VScale);	// scale appropriate values, then pass into functio
}

// init a timer
void TIM_init()
{
    // configure clk
    RCC -> APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    // set APR to total count
    TIM2 -> ARR = ARRVal-1;

    TIM2 -> PSC = PRESC;

    // enable update event interrupt for AAR, disable CCR
    TIM2 -> DIER |= (TIM_DIER_UIE );
    TIM2 -> DIER &= ~(TIM_DIER_CC1IE);

    // clear interrupt status and update event
    TIM2 -> SR &= ~(TIM_SR_UIF);

    // enable NVIC interrupts
    NVIC -> ISER[0] = (1 << (TIM2_IRQn & 0x1F));

    // enable global interrupts
    __enable_irq();

    // start the timer
    TIM2 -> CR1 |= TIM_CR1_CEN;
}

// Timer handler -> start conversion every 500us
void TIM2_IRQHandler(void)
{
    if (TIM2 -> SR & TIM_SR_UIF){
        if (flag){
            // start conversion
            ADC1 -> CR |= (ADC_CR_ADSTART);
        }
        // clear flag
        TIM2 -> SR &= ~(TIM_SR_UIF);
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
