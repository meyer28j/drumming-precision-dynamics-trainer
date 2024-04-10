#include "stm32f10x.h"
#include "max7219.h"
#include "spi.h"
#include "timer.h"

void max_init(void) {

	for (uint16_t m = 0; m < MATRIX_COUNT; m++) {
		update(m, ADDR_TEST, NO_OP); // disable display test for all matrices
		update(m, ADDR_SHUTDOWN, 0xC01); // set shutdown register for normal operation
		update(m, ADDR_DECODE, 0x900); // set decode mode to normal operation
		update(m, ADDR_INTENSITY, 0xA04); // set intensity to middle
		update(m, ADDR_SCAN, 0xB07); // set scan limit to include all LEDs
	}
	clear_all();

	return;
}


void clear(uint16_t matrix_num) {
	// clear all rows for a given matrix
	for (uint16_t row = 0x100; row <= 0x800; row += 0x100) {
			update(matrix_num, row, NO_OP);
	}
}


void clear_all(void) {
	// clear all rows in all matrices
	for (uint16_t matrix = 0; matrix < MATRIX_COUNT; matrix++) {
		clear(matrix);
	}
	
	return;
}


void test_all(uint16_t speed) {
	// TEST all matrices on, then all off
	for (int i = 0; i < MATRIX_COUNT; i++) {
		spi_start();
		spi_send(0xF01); // enable display test
		spi_stop();
		delay_ms(speed);
	}
	for (int i = 0; i < MATRIX_COUNT; i++) {
		spi_start();
		spi_send(0xF00); // enable display test
		spi_stop();
		delay_ms(speed);
	}
	
	// enable and disable each matrix one at a time
	for (uint16_t matrix_num = 0; matrix_num < MATRIX_COUNT; matrix_num++) {
		update(matrix_num, ADDR_TEST, 1);
		delay_ms(speed);
		update(matrix_num, ADDR_TEST, 0);
		delay_ms(speed);
	}
	
	// do the same as above except backwards
	for (uint16_t matrix_num = MATRIX_COUNT; matrix_num > 0; matrix_num--) {
		update(matrix_num - 1, ADDR_TEST, 1);
		delay_ms(speed);
		update(matrix_num - 1, ADDR_TEST, 0);
		delay_ms(speed);
	}
		
	// sequentially activate all rows in all matrices
	for (uint16_t matrix_num = 0; matrix_num < MATRIX_COUNT; matrix_num++) {
		for (uint16_t row = 0x100; row <= 0x800; row += 0x100) { // increment through row addresses
			update(matrix_num, row, 0xFF); // 0xFF == all on
			delay_ms(speed / 8);
		}
	}
	
	// sequentially deactivate all rows in all matrices
	for (uint16_t matrix_num = 0; matrix_num < MATRIX_COUNT; matrix_num++) {
		for (uint16_t row = 0x100; row <= 0x800; row += 0x100) { // increment through row addresses
			update(matrix_num, row, NO_OP);
			delay_ms(speed / 8);
		}
	}
	
	return;
}

/* For example, if four MAX7219s are cascaded, then to 
write to the fourth chip, sent the
desired 16-bit word, followed by three no-op codes (hex
0xX0XX, see Table 2). When LOAD/CS goes high, data is
latched in all devices. The first three chips receive no-op
commands, and the fourth receives the intended data. */

// updates a single matrix
// address: 0xX00 - bytes 0 and 1 must be set to 0
// data: 0xXX - lowest 2 bytes only; must not be larger than 0xFF
// matrix_num: 0x0 - 0x3 - represents which matrix to address
void update(uint16_t matrix_num, uint16_t address, uint16_t data) {
	spi_start();
	
	// send no-ops to matrices further than desired matrix
	for (uint16_t i = matrix_num; i < MATRIX_COUNT; i++){
		spi_send(NO_OP);
	}

	spi_send(address | data);

	// send no-ops to push data to desired matrix
	for (uint16_t i = 0; i < matrix_num; i++) {
		spi_send(NO_OP);
	}
	
	spi_stop();
	
	return;
}


void bin_count(uint16_t speed) {
	uint16_t test_address = 0xF00;
	
	update(0, test_address, 1); // 1 on
	delay_ms(speed);
	update(1, test_address, 1); // 2 on
	update(0, test_address, 0); // 1 off
	delay_ms(speed);
	update(0, test_address, 1); // 1 on
	delay_ms(speed);
	update(2, test_address, 1); // 3 on
	update(1, test_address, 0); // 2 off
	update(0, test_address, 0); // 1 off
	delay_ms(speed);
	
	update(0, test_address, 1); // 1 on
	delay_ms(speed);
	update(1, test_address, 1); // 2 on
	update(0, test_address, 0); // 1 off
	delay_ms(speed);
	update(0, test_address, 1); // 1 on
	delay_ms(speed);
	
	update(2, test_address, 0); // 3 off
	update(1, test_address, 0); // 2 off
	update(0, test_address, 0); // 1 off
	delay_ms(speed);
}

