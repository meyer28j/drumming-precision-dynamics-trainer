#include "stm32f10x.h"


void enable_GPIO_output(GPIO_TypeDef* GPIO, int port_number);

void enable_GPIO_input(GPIO_TypeDef* GPIO, int port_number);


// Dave's original functions from lab 1
void LD2_init(void);
void LD2_OFF(void);
void LD2_ON(void);
void LD2_TOGGLE(void);

