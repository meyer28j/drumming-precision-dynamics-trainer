// Name: Jacob Meyer
// SID: 200364255
// Start Date: 2024-03-23

#include "stm32f10x.h"
#include "main.h"

uint16_t bpm = 60; // beats per minute
uint16_t mspb = 1000; // milliseconds per beat; used with delay_ms()


void EXTI0_IRQHandler(void) {
	
	EXTI->PR |= EXTI_PR_PR0; // clear pending interrupt bit
	
	bpm += 1;
	mspb = bpm * 1000 / 60;
	LD2_TOGGLE();
	return;
}


void EXTI1_IRQHandler(void) {
	
	EXTI->PR |= EXTI_PR_PR1; // clear pending interrupt bit
	
	bpm -= 1;
	mspb = bpm * 1000 / 60;
	LD2_TOGGLE();
	return;
}



void initialize(void) {
	// enable clock for ports A, B, C
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN);
	
	
	// enable on-board LED
	enable_GPIO_output(GPIOA, 5);
		
	// enable on-board button
	enable_GPIO_input(GPIOC, 13);
	
	// enable breadboard button input on PA8 and PA9
	enable_GPIO_input(GPIOA, 8);
	enable_GPIO_input(GPIOA, 9);
	
	///////////////////////// 
	// ENABLE INTERRUPTS ON PORT B
	// for breadboard buttons to change BPM
	RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN);	// enable clock for AFIO
	
	// Initialize EXTI0 registers for PB8
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PB; // select port B as source for EXTI0 events
	EXTI->IMR |= EXTI_IMR_MR8; // unmask PB8 as the interrupt source
	EXTI->FTSR |= EXTI_FTSR_TR8; // enable falling edge of signal on PB8 as the trigger
	NVIC->ISER[0] |= NVIC_ISER_SETENA_6; // unmask EXTI0 as an interrupt source in the NVIC
	
	// Initialize EXTI1 registers for PB9
	AFIO->EXTICR[1] |= AFIO_EXTICR1_EXTI1_PB; // select port B as source for EXTI1 events
	EXTI->IMR |= EXTI_IMR_MR9; // unmask PB9 as the interrupt source
	EXTI->FTSR |= EXTI_FTSR_TR9; // enable falling edge of signal on PB9 as the trigger
	NVIC->ISER[1] |= NVIC_ISER_SETENA_6; // unmask EXTI1 as an interrupt source in the NVIC
	
	bpm = 60;
	mspb = 600;	

	return;
}


int main(void)
{
	SystemInit(); 	// 72 MHz system clock
	tim2_init(); 		// Delay function timer
	spi2_init(); 		// Serial peripheral interface
	max_init();  		// LED matrix
	
	initialize();
	
	while (1) {
		
		// TEST all matrices on, then all off
		for (int i = 0; i < MATRIX_COUNT; i++) {
			spi_start();
			spi_send(0xF01); // enable display test
			spi_stop();
			delay_ms(mspb / 4);
		}
		for (int i = 0; i < MATRIX_COUNT; i++) {
			spi_start();
			spi_send(0xF00); // enable display test
			spi_stop();
			delay_ms(mspb / 4);
		}
		
		bin_count();
		
		// enable and disable each matrix one at a time
		for (uint16_t matrix_num = 0; matrix_num < MATRIX_COUNT; matrix_num++) {
			update(matrix_num, ADDR_TEST, 1);
			delay_ms(mspb / 2);
			update(matrix_num, ADDR_TEST, 0);
			delay_ms(mspb / 2);
		}
		
		// do the same as above except backwards
		for (uint16_t matrix_num = MATRIX_COUNT; matrix_num > 0; matrix_num--) {
			update(matrix_num - 1, ADDR_TEST, 1);
			delay_ms(mspb / 2);
			update(matrix_num - 1, ADDR_TEST, 0);
			delay_ms(mspb / 2);
		}
		

		// sequentially activate bottom row LEDs
		for (uint16_t matrix = 1; matrix <= MATRIX_COUNT; matrix++) {
			spi_send_receive((matrix << 8) | 0xFF);
			delay_ms(mspb);
		}
		
		clear_all();
	}
}
