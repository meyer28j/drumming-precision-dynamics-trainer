// Name: Jacob Meyer
// SID: 200364255
// Start Date: 2024-03-23

#include "stm32f10x.h"
#include "main.h"


void initialize(void) {
	// enable clock for ports A, B, C
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN);
	
	
	// enable on-board LED
	enable_GPIO_output(GPIOA, 5);
		
	// enable on-board button
	enable_GPIO_input(GPIOC, 13);

	return;
}


int main(void)
{
	SystemInit(); 	// 72 MHz system clock
	tim2_init(); 		// Delay function timer
	spi2_init(); 		// Serial peripheral interface
	//max_init();  		// LED matrix
	
	initialize();
	
	while (1) {
		
		//test_all();
		bin_count();

		// TEST all matrices on, then all off
		for (int i = 0; i < MATRIX_COUNT; i++) {
			spi_send_receive(0xF01); // enable display test
			delay_ms(200);
		}
		for (int i = 0; i < MATRIX_COUNT; i++) {
			spi_send_receive(0xF00); // send no-ops
			delay_ms(200);
		}	
	}
}

