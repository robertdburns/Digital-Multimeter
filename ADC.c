#include "ADC.h"
#define us20 50000
void calculation_function(uint16_t array[]);

uint16_t read_values[SIZE];
uint16_t place = 0;

void ADC1_2_IRQHandler(void) {
    if (ADC1 -> ISR & ADC_ISR_EOC) { // maybe maybe not?

        // read value, convert the value to uint16_t (16 bits wide)
        // - reading from DR sets EOC to 1 which enables another conversion
        // - (18.7.1)
        uint16_t adcVal = (ADC1 -> DR);

        // save to array[place];
        read_values[place] = CALIBRATE(adcVal);

        // increment place value
        place += 1;

        // check the count value = 2500
        if (place >= SIZE) {
            // calculate values and reset count
            calculation_function(read_values);
            place = 0;
        }
    }
}

void ADC_init() {
    // pin PA0 channel 5
    // configure the clock, p2 - 607

    RCC -> AHB2ENR |= (RCC_AHB2ENR_ADCEN | RCC_AHB2ENR_GPIOAEN);
    ADC123_COMMON -> CCR = (1 << ADC_CCR_CKMODE_Pos);

    // power on ADC and voltage regulator
    ADC1 -> CR &= ~(ADC_CR_DEEPPWD); // deep power down 0
    ADC1 -> CR |= (ADC_CR_ADVREGEN); // volt regulator 1

    // wait for 20us
    for (int i = 0; i < us20; i++);

    // enable the differential select
    // need to be 0 on channel 5 to enable single-ended mode
    ADC1 -> DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);

    // calibrate
    // ensure that ADC is disabled and single-ended calibration
    ADC1 -> CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF);

    // start calibration
    ADC1 -> CR |= (ADC_CR_ADCAL); // calibration in process
    while (ADC1 -> CR & ADC_CR_ADCAL); // waits till 0 - done calibration

    // enable ADC
    // clear ADC ready with a 1
    ADC1 -> ISR |= (ADC_ISR_ADRDY);
    ADC1 -> CR |= (ADC_CR_ADEN);
    while (!(ADC1 -> ISR & ADC_ISR_ADRDY));

    // configure sequence
    // single channel 5, conversion size of 1
    ADC1 -> SQR1 = (5 << ADC_SQR1_SQ1_Pos);

    // Configure 12-bit resolution, right align, single conversion mode (18.7.4)
    ADC1 -> CFGR = 0; // everything needs to 0 or we don't care

    // 2.5 clock selection
    ADC1 -> SMPR1 = 0;

    // configure interrupts
    ADC1 -> IER |= ADC_IER_EOCIE;
    NVIC_EnableIRQ(ADC1_2_IRQn);
    __enable_irq();

    // configure GPIO for PA0 analog
    // mode = 11; PUPD = 0
    GPIOA -> MODER |= (3 << GPIO_MODER_MODE0_Pos);
    GPIOA -> PUPDR &= ~(GPIO_PUPDR_PUPD0);
    GPIOA -> ASCR |= 1; // configure analog switch control register
}
