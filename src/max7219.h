#include "stm32f10x.h"

const static int MATRIX_COUNT = 4;

const static uint16_t NO_OP = 0x000;
const static uint16_t ROW_0 = 0x100;
const static uint16_t ROW_1 = 0x200;
const static uint16_t ROW_2 = 0x300;
const static uint16_t ROW_3 = 0x400;
const static uint16_t ROW_4 = 0x500;
const static uint16_t ROW_5 = 0x600;
const static uint16_t ROW_6 = 0x700;
const static uint16_t ROW_7 = 0x800;
const static uint16_t ADDR_DECODE = 0x900;
const static uint16_t ADDR_INTENSITY = 0xA00;
const static uint16_t ADDR_SCAN = 0xB00;
const static uint16_t ADDR_SHUTDOWN = 0xC00;
const static uint16_t ADDR_TEST = 0xF00;

const static uint16_t bpm_pattern = 0x18; // 2 center LEDs --> 0b0001'1000
const static uint16_t bpm_rows[2] = { ROW_3, ROW_4 }; // center two rows

const static uint16_t early_0 = 0x4; // 0b0000'0100
const static uint16_t early_1 = 0x6; // 0b0000'0110
const static uint16_t early_2 = 0x7; // 0b0000'0111
const static uint16_t late_0 = 0x20; // 0b0010'0000
const static uint16_t late_1 = 0x60; // 0b0110'0000
const static uint16_t late_2 = 0xE0; // 0b1110'0000


void max_init(void);
void clear(uint16_t matrix_num);
void clear_all(void);
void test_all(uint16_t speed);
void update(uint16_t matrix_num, uint16_t address, uint16_t data);
void update_gpt(uint16_t matrix_num, uint16_t address, uint16_t data);
void bin_count(uint16_t speed);

