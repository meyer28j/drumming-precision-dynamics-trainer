#include "stm32f10x.h"
#include "max7219.h"
#include "spi.h"
#include "timer.h"

void max_init(void) {

	for (uint16_t m = 0; m < MATRIX_COUNT; m++) {
		update(m, ADDR_TEST, NO_OP); // disable display test for all matrices
		update(m, ADDR_SHUTDOWN, 0x1); // set shutdown register for normal operation
		update(m, ADDR_DECODE, NO_OP); // set decode mode to normal operation
		update(m, ADDR_INTENSITY, 0x4); // set intensity to middle
		update(m, ADDR_SCAN, 0x7); // set scan limit to include all LEDs
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
	
	// enable and disable each matrix one at a time
	for (uint16_t matrix_num = 0; matrix_num < MATRIX_COUNT; matrix_num++) {
		update(matrix_num, ADDR_TEST, 1);
		delay_ms(speed * 2);
		update(matrix_num, ADDR_TEST, 0);
		delay_ms(speed * 2);
	}
	
	// do the same as above except backwards
	for (uint16_t matrix_num = MATRIX_COUNT; matrix_num > 0; matrix_num--) {
		update(matrix_num - 1, ADDR_TEST, 1);
		delay_ms(speed * 2);
		update(matrix_num - 1, ADDR_TEST, 0);
		delay_ms(speed * 2);
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


// test function for 
void bin_count(uint16_t speed) {
	uint16_t ADDR_TEST = 0xF00;
	
	update(0, ADDR_TEST, 1); // 1 on
	delay_ms(speed);
	update(1, ADDR_TEST, 1); // 2 on
	update(0, ADDR_TEST, 0); // 1 off
	delay_ms(speed);
	update(0, ADDR_TEST, 1); // 1 on
	delay_ms(speed);
	update(2, ADDR_TEST, 1); // 3 on
	update(1, ADDR_TEST, 0); // 2 off
	update(0, ADDR_TEST, 0); // 1 off
	delay_ms(speed);
	
	update(0, ADDR_TEST, 1); // 1 on
	delay_ms(speed);
	update(1, ADDR_TEST, 1); // 2 on
	update(0, ADDR_TEST, 0); // 1 off
	delay_ms(speed);
	update(0, ADDR_TEST, 1); // 1 on
	delay_ms(speed);
	
	update(2, ADDR_TEST, 0); // 3 off
	update(1, ADDR_TEST, 0); // 2 off
	update(0, ADDR_TEST, 0); // 1 off
	delay_ms(speed);
}

