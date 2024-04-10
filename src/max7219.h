#include "stm32f10x.h"

const static int MATRIX_COUNT = 4;

const static uint16_t NO_OP = 0x000;
const static uint16_t DIGIT_0 = 0x100;
const static uint16_t DIGIT_1 = 0x200;
const static uint16_t DIGIT_2 = 0x300;
const static uint16_t ADDR_DECODE = 0x900;
const static uint16_t ADDR_INTENSITY = 0xA00;
const static uint16_t ADDR_SCAN = 0xB00;
const static uint16_t ADDR_SHUTDOWN = 0xC00;
const static uint16_t ADDR_TEST = 0xF00;

void max_init(void);
void clear_all(void);
void test_all(uint16_t speed);
void update(uint16_t matrix_num, uint16_t address, uint16_t data);
void update_gpt(uint16_t matrix_num, uint16_t address, uint16_t data);
void bin_count(uint16_t speed);

