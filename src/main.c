// Name: Jacob Meyer
// SID: 200364255
// Start Date: 2024-03-23

#include "stm32f10x.h"
#include "main.h"

uint16_t bpm = 60; // beats per minute
uint16_t mspb = 1000; // milliseconds per beat; used with delay_ms()


void EXTI9_5_IRQHandler(void) {
	
	start_calibration_timer();
		
	if (EXTI->PR & EXTI_PR_PR8) { // interrupt caused by PA8
		if (bpm > 10) { // lower cap on BPM
			bpm -= 4;
			mspb = 60000 / bpm;
		}
		EXTI->PR |= EXTI_PR_PR8; // clear pending interrupt bit	
	} 
	else if (EXTI->PR & EXTI_PR_PR9) { // interrupt cause by PA9
		if (bpm < 300) { // upper cap on BPM
			bpm += 4;
			mspb = 60000 / bpm;
		}
		EXTI->PR |= EXTI_PR_PR9; // Clear the pending bit for PA9
	}
	return;
}


void TIM3_IRQHandler(void) {
	// handle event that both buttons are held down for calibration mode
	if (TIM3->SR & TIM_SR_UIF) {
		TIM3->SR &= ~TIM_SR_UIF; // clear update interrupt flag
		
    if (!(GPIOA->IDR & GPIO_IDR_IDR8) && !(GPIOA->IDR & GPIO_IDR_IDR9)) {
		// both buttons are still pressed after debounce period
		EXTI->PR = EXTI_PR_PR8;
		EXTI->PR = EXTI_PR_PR9;

		test_all(150); // temporary test for demo purposes
		/////////////////////////
		// TODO: implement calibration mode
		/////////////////////////
		}
	}
	return;
}


void initialize(void) {
	// enable clock for AFIO and ports A and B
	RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN);

	// enable interrupts on port A for
	// breadboard buttons to change BPM
	
	// set PA8, PA9 as interrupt source using EXTI9_5
	AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PA | AFIO_EXTICR3_EXTI9_PA; // select port A as source for EXTI9_5 events
	EXTI->IMR |=  EXTI_IMR_MR8 | EXTI_IMR_MR9; // unmask PA8 and PA9 as the interrupt source
	EXTI->FTSR |=  EXTI_FTSR_TR8 | EXTI_FTSR_TR9; // enable falling edge of signal on PA8 and PA9 as the trigger
	NVIC->ISER[0] |= NVIC_ISER_SETENA_23; // unmask EXTI9_5 as an interrupt source in the NVIC
	
	bpm = 60;
	mspb = 1000;	
	
	return;
}


int main(void)
{
	SystemInit(); 	// 72 MHz system clock
	// LD2_init();	// on-board LED for test purposes
	tim2_init(); 		// Delay function timer
	tim3_init();		// calibration input delay timer
	spi2_init(); 		// Serial peripheral interface
	max_init();  		// LED matrix
	
	initialize();

	while (1) {
				
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
		}
	}
}

