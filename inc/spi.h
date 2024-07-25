#include "stm32f10x.h"

void spi2_init(void);
void spi_start(void);
void spi_stop(void);
void spi_send(uint16_t data);
uint16_t spi_receive(void);
uint16_t spi_send_receive(uint16_t send_data);

