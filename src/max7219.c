#include "stm32f10x.h"
#include "max7219.h"
#include "spi.h"
#include "timer.h"

void max_init(void) {
	
	
	for (int i = 0; i < MATRIX_COUNT; i++) {
		spi_send_receive(0xF00); // disable display test for all matrices
		spi_send_receive(0xC01); // set shutdown register for normal operation
		//spi_send_receive(0x900); // set decode mode to normal operation
		spi_send_receive(0xA04); // set intensity to middle
		//spi_send_receive(0xB07); // set scan limit to include all LEDs
		delay_ms(1000);
	}
	
	
	
	/*
	spi_start();
	for (uint16_t i = 0; i < MATRIX_COUNT; i++) {
		spi_send(0xF00); // disable test mode
	}
	spi_stop();
	*/
		
	return;
}


void test_all(void) {
		spi_send_receive(0xF01); // enable display test
		delay_ms(200);
		spi_send_receive(0xF00); // disable display test
		delay_ms(200);
		spi_send_receive(0); // send no-ops
		delay_ms(200);
		spi_send_receive(0); // send no-ops
		delay_ms(200);
}

	/* For example, if four MAX7219s are cascaded, then to 
	write to the fourth chip, sent the
  desired 16-bit word, followed by three no-op codes (hex
  0xX0XX, see Table 2). When LOAD/CS goes high, data is
  latched in all devices. The first three chips receive no-op
	commands, and the fourth receives the intended data.
	*/

// updates a single matrix
// address: 0xX00 - bytes 0 and 1 must be set to 0
// data: 0xXX - lowest 2 bytes only; must not be larger than 0xFF
// matrix_num: 0x0 - 0x3 - represents which matrix to address
void update(uint16_t matrix_num, uint16_t address, uint16_t data) {
	spi_start();
	// send no-ops prior to desired matrix update
	for (uint16_t i = matrix_num; i < MATRIX_COUNT; i++) {
		spi_send(0);
	}
	
	spi_send(address + data);
	for (uint16_t i = 0; i < matrix_num; i++) {
		spi_send(0);
	}
	spi_stop();
	
	return;
}


void bin_count(void) {
	uint16_t test_address = 0xF00;
	
	update(0, test_address, 1); // 1 on
	delay_ms(500);
	update(1, test_address, 1); // 2 on
	update(0, test_address, 0); // 1 off
	delay_ms(500);
	update(0, test_address, 1); // 1 on
	delay_ms(500);
	update(2, test_address, 1); // 3 on
	update(1, test_address, 0); // 2 off
	update(0, test_address, 0); // 1 off
	delay_ms(500);
	
	update(0, test_address, 1); // 1 on
	delay_ms(500);
	update(1, test_address, 1); // 2 on
	update(0, test_address, 0); // 1 off
	delay_ms(500);
	update(0, test_address, 1); // 1 on
	delay_ms(500);
	
	update(2, test_address, 0); // 3 off
	update(1, test_address, 0); // 2 off
	update(0, test_address, 0); // 1 off
	delay_ms(500);
}

