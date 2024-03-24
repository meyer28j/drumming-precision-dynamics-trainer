#include "stm32f10x.h"
#include "timer.h"


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

void MCO_on(void)
{
    RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
	
		// Configure PA8 as AFIO ( Write 1011b into the configuration and mode bits )
    GPIOA->CRH |= GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8_1 | GPIO_CRH_MODE8_0 ;
    GPIOA->CRH &= ~GPIO_CRH_CNF8_0 ;

		RCC->CFGR |= RCC_CFGR_MCO_PLL; // select pll/2 as clock output
}

