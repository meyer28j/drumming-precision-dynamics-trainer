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
	mspb = 60000 / bpm;
	LD2_TOGGLE();
	return;
}


void EXTI1_IRQHandler(void) {
	
	EXTI->PR |= EXTI_PR_PR1; // clear pending interrupt bit
	
	bpm -= 1;
	mspb = 60000 / bpm;
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
		
		// output BPM in center 4 LEDs of each matrix		
		for (uint16_t m = 0; m < MATRIX_COUNT; m++) {
		// clear matrix of previous error positions
			clear(m);
			
			for (uint16_t row = 0; row < 2; row++) {
				update(m, bpm_rows[row], bpm_pattern); // update to show pattern
			}
			// modulate brightness from max to min over beat duration
			for (int brightness = 0xF; brightness >= 0; brightness--) {
				update(m, ADDR_INTENSITY, brightness);
				delay_ms(mspb / 16); // 16 stages of brightness: 0x0 -> 0xF
			}
			for (uint16_t row = 0; row < 2; row++) {
				update(m, bpm_rows[row], NO_OP); // disable center pattern
			}
			////////////////////
			// TODO: implement error positioning according to input
			////////////////////
			
			// reset brightness to maximum for error representation
			update(m, ADDR_INTENSITY, 0xF);
			
			// implement input simulations
			
			if (m == 1) { // simulate 'too quiet' in matrix 2
				update(1, ROW_2, bpm_pattern);
			} else if (m == 2) { // simulate 'too early' in matrix 3: two levels
				update(2, ROW_3, early_1);
				update(2, ROW_4, early_1);
			} else if (m == 3) { // simulate 'too loud' 3 levels and 'too late' 1 level in matrix 4
				update(3, ROW_5, bpm_pattern); // too loud level 1
				update(3, ROW_6, bpm_pattern); // too loud level 2
				update(3, ROW_7, bpm_pattern); // too loud level 3
				update(3, ROW_3, late_0);
				update(3, ROW_4, late_0);
			}
		}
		//clear_all();
	}
}

