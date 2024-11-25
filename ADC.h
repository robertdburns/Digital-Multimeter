#ifndef INCADC_H
#define INCADC_H

#include "main.h"

// #defines for values for calibration function, numbers determined through testing for my ADC
#define MAXV    3300
#define SCALEH  4090
#define SCALEL  0
#define DACBIT  4096
#define BITVOLT (MAXV/DACBIT)
#define OFFSET  -20

#define CALIBRATE(val) (((val * (SCALEH - SCALEL)) / DACBIT) + OFFSET)

void ADC_init(void);

#endif /* INC_ADC_H */
