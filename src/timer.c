#include "stm32f10x.h"
#include "timer.h"


/*****************************************************************************************************************/
/* TIMER 3 : Timer 3 is used for checking entry conditions for calibration mode							                     */
/*****************************************************************************************************************/
void tim3_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->CR1 = 0;
	TIM3->PSC = 7200 - 1; // 72MHz / 7200 = 10kHz
	TIM3->ARR = 0xA000 - 1; // with 10kHz prescaler, generates interrupt every 100ms
	TIM3->CR1 |= TIM_CR1_OPM; // stop when event fires
	TIM3->DIER |= TIM_DIER_UIE; // enable update interrupt
	NVIC->ISER[0] |= NVIC_ISER_SETENA_29; // enable TIM3 interrupt in NVIC
	TIM3->SR &= ~TIM_SR_UIF; // clear update interrupt flag
}


void start_calibration_timer(void) {
	TIM3->CNT = 0; // reset timer
	TIM3->CR1 |= TIM_CR1_CEN; // start timer
	return;
}


/*****************************************************************************************************************/
/* TIMER 2 : Timer 2 is used for generating delay times and is not available for other tasks                     */
/*****************************************************************************************************************/
void tim2_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->CR1 = 0;	//ensure timer is off
	TIM2->PSC = 72 - 1; //PSC adds 1 
	TIM2->ARR	= 0xffff - 1;
	TIM2->CR1 |= TIM_CR1_CEN;
	while(!(TIM2->SR & TIM_SR_UIF)); //Wait for the counter to generate its first update event
}


void delay_us(uint16_t delay_time_us)
{
	TIM2->CNT = 0;
	while (TIM2->CNT <= delay_time_us)
	{}
	
}

void delay_ms(uint16_t delay_time_ms)
{
	for (uint16_t i=0; i<delay_time_ms; i++)
	{
		delay_us (1000); // delay of 1 ms
	}
}

