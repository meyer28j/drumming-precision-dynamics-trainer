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
		
		//bin_count(mspb / 2);
		//test_all(mspb / 2);
		
		// turn on all matrices
		for (uint16_t m = 0; m < MATRIX_COUNT; m++) {
			update(m, ADDR_TEST, 0xFF); 
		}
		delay_ms(mspb);
		// turn off matrices 2 and 4
		update(1, ADDR_TEST, NO_OP);
		update(3, ADDR_TEST, NO_OP);
		delay_ms(mspb);
		// turn off matrices 1 and 3
		update(0, ADDR_TEST, NO_OP);
		update(2, ADDR_TEST, NO_OP);
		delay_ms(mspb);
		
		// sequentially activate all rows in all matrices
		for (uint16_t matrix_num = 0; matrix_num < MATRIX_COUNT; matrix_num++) {
			for (uint16_t row = 0x100; row <= 0x800; row += 0x100) { // increment through row addresses
				update(matrix_num, row, 0xFF); // 0xFF == all on
				delay_ms(mspb / 8);
			}
		}
		
		// sequentially deactivate all rows in all matrices
		for (uint16_t matrix_num = 0; matrix_num < MATRIX_COUNT; matrix_num++) {
			for (uint16_t row = 0x100; row <= 0x800; row += 0x100) { // increment through row addresses
				update(matrix_num, row, NO_OP);
				delay_ms(mspb / 8);
			}
		}
		
		clear_all();
	}
}
