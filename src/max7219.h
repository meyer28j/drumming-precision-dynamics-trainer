#include "stm32f10x.h"

const static int MATRIX_COUNT = 4;

void max_init(void);
void test_all(void);
void update(uint16_t matrix_num, uint16_t address, uint16_t data);
void bin_count(void);

