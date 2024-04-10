#include "stm32f10x.h"
#include <stdbool.h>

void tim3_init(void);
void start_calibration_timer(void);

/* Timer 2 provides delay time functions */
void tim2_init(void);
void delay_us(uint16_t);
void delay_ms(uint16_t);
void MCO_on(void);






