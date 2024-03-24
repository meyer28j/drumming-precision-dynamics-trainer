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
	

	uint16_t receive_data = 0;
	uint16_t send_data = 0;
	uint16_t register_address = 0xA00; // light intensity
	
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
		
		update(2, 0xF00, 1);
		delay_ms(500);
		update(2, 0xF00, 0);
		delay_ms(500);
		
		
		/*
		for (uint16_t matrix_num = 0; matrix_num < MATRIX_COUNT; matrix_num++) {
			update(matrix_num, 0xF00, 1); // test mode on
			delay_ms(500);
			update(matrix_num, 0xF00, 0); // test mode off
			delay_ms(500);
		}
		*/
		/*
		register_address = 0xA00; // light intensity
		for (uint16_t i = 0; i < 0x10; i++) { // cycle through intensities
			send_data = register_address + i;
			receive_data = spi_send_receive(send_data);
			
			delay_ms(500);
			
			LD2_TOGGLE();
		}
		*/
		
	}
}

