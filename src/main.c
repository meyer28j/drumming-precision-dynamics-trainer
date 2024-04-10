// Name: Jacob Meyer
// SID: 200364255
// Start Date: 2024-03-23

#include "stm32f10x.h"
#include "main.h"

uint16_t bpm = 60; // beats per minute
uint16_t mspb = 1000; // milliseconds per beat; used with delay_ms()


void EXTI0_IRQHandler(void) {
	
	delay_ms(100); // simple handle for switch bounce
		
	// since this is a higher priority than EXTI1
	// we can look to see if the EXTI1 pending bit
	// is also set --> activate calibration mode
	if ((EXTI->PR & EXTI_PR_PR1) == EXTI_PR_PR1) { // EXTI1 pending interrupt request
		// other button is pressed too
		EXTI->PR |= EXTI_PR_PR1; // clear pending request for other button
		
		/////////////////
		// TODO: implement calibration mode entry
		/////////////////
		return;		
	}

	bpm -= 1;
	mspb = 60000 / bpm;
	LD2_TOGGLE();
	
	EXTI->PR |= EXTI_PR_PR0; // clear pending interrupt bit
	
	return;
}

void EXTI1_IRQHandler(void) {
	
	delay_ms(100); // simple handle for switch bounce

	bpm += 1;
	mspb = 60000 / bpm;
	LD2_TOGGLE();
	
	EXTI->PR |= EXTI_PR_PR1; // clear pending interrupt bit

	
	return;
}


void EXTI9_5_IRQHandler(void) {
	
	delay_ms(100); // simple handle for switch bounce

	// both buttons are pressed: enter calibration mode
	if ((EXTI->PR & EXTI_PR_PR8) && (EXTI->PR & EXTI_PR_PR9)) {
		/////////////////////////
		// TODO: implement calibration mode
		test_all(mspb / 8);
		/////////////////////////
		EXTI->PR = EXTI_PR_PR8;
		EXTI->PR = EXTI_PR_PR9;
		return;
	}
		
	else if (EXTI->PR & EXTI_PR_PR8) { // interrupt caused by PA8
		bpm -= 2;
		mspb = 60000 / bpm;
		EXTI->PR |= EXTI_PR_PR8; // clear pending interrupt bit	
	} 
	else if (EXTI->PR & EXTI_PR_PR9) { // interrupt cause by PA9
		bpm += 2;
		mspb = 60000 / bpm;
		EXTI->PR |= EXTI_PR_PR9; // Clear the pending bit for PA9
	}
	return;
}


void initialize(void) {
	// enable clock for ports A, B, C
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN);
	RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN);	// enable clock for AFIO

	enable_GPIO_output(GPIOA, 5);	// enable on-board LED
	enable_GPIO_input(GPIOC, 13);	// enable on-board button

	///////////////////////// 
	// ENABLE INTERRUPTS ON PORT A
	// for breadboard buttons to change BPM
	
	
	// TEST PB0 as source
	//AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PB; // select port B as source for EXTI0 events
	//EXTI->IMR |= EXTI_IMR_MR0; // unmask PB0 as the interrupt source
	//EXTI->FTSR |= EXTI_FTSR_TR0; // enable falling edge of signal on PB0 as the trigger
	//NVIC->ISER[0] |= NVIC_ISER_SETENA_6; // unmask EXTI0 as an interrupt source in the NVIC (using bit 6)
	
	// set PA8, PA9 as interrupt source using EXTI9_5
	// EXTI9_5 is priority #23
	AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PA | AFIO_EXTICR3_EXTI9_PA; // select port A as source for EXTI9_5 events
	EXTI->IMR |=  EXTI_IMR_MR8 | EXTI_IMR_MR9; // unmask PA8 and PA9 as the interrupt source
	EXTI->FTSR |=  EXTI_FTSR_TR8 | EXTI_FTSR_TR9; // enable falling edge of signal on PA8 and PA9 as the trigger
	NVIC->ISER[0] |= NVIC_ISER_SETENA_23; // unmask EXTI9_5 as an interrupt source in the NVIC
	
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

