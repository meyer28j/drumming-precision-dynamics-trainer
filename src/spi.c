#include "stm32f10x.h"
#include "spi.h"

void spi2_init(void) {

	// enable clocks for GPIOB, AFIO, SPI2
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN; // 72MHz
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; // 36MHz
	
	// set baud rate to divide by 32
	SPI2->CR1 |= SPI_CR1_BR_2;
	//SPI2->CR1 |= SPI_CR1_BR_1 | SPI_CR1_BR_0; // divide by 16
	//SPI2->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1; // divide by 8

	// set CPOL and CPHA
	
	// PB15, PB13 (MOSI, SCK) need to be configured as AFIO push/pull
	// 0b1011, CNF 10 and MODE 11
	GPIOB->CRH |= GPIO_CRH_MODE15 | GPIO_CRH_MODE13;
	GPIOB->CRH |= GPIO_CRH_CNF15_1 | GPIO_CRH_CNF13_1;
	GPIOB->CRH &= ~GPIO_CRH_CNF15_0 & ~GPIO_CRH_CNF13_0;
	// PB14 (input) configured as floating input by default
	
	// PB9 (GPIO control, CS line) configured as GPIO push/pull
	GPIOB->CRH &= ~GPIO_CRH_CNF9; // 0b00 GPO push-pull
	GPIOB->CRH |= GPIO_CRH_MODE9; // 0b11 output 50MHz
	
	// set PB9 (CS line) as HIGH
	GPIOB->BSRR |= GPIO_BSRR_BS9;

	// set NSS to operate in software mode
	// CR1 SSM and SSI are HIGH
	SPI2->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
	// CR2 SSOE is LOW
	SPI2->CR2 &= ~SPI_CR2_SSOE;
	


	//configure SPI2 to use 16-bit transfer sizes
	// set DFF == 1 for 16-bit data frame
	// set MSTR == 1 for master configuration
	// set SPE == 1 to enable SPI
	SPI2->CR1 |= SPI_CR1_DFF | SPI_CR1_MSTR | SPI_CR1_SPE;

	return;
}


void spi_start(void) {
	// reset PB9 (CS) LOW
	GPIOB->BSRR |= GPIO_BSRR_BR9;
	return;
}


void spi_stop(void) {
		// set PB9 (CS) HIGH
	GPIOB->BSRR |= GPIO_BSRR_BS9;
	return;
}


void spi_send(uint16_t send_data) {
	// wait for transmitter buffer to be empty
	while ((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE);
	// once TXE is HIGH (buffer is empty) put data into SPI-DR
	SPI2->DR = send_data;
	
	// wait for transfer to complete (i.e. RXNE goes HIGH)
	while ((SPI2->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
	SPI2->DR; // read DR to clear RXNE flag

	return;
}


uint16_t spi_receive(void) {	
	// wait for transfer to complete (i.e. RXNE goes HIGH)
	while ((SPI2->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
	uint16_t receive_data = SPI2->DR; // read DR to clear RXNE flag
	
	return receive_data;
}


uint16_t spi_send_receive(uint16_t send_data) {
	
	// begin transmission
	// set PB9 (CS) to go LOW
	GPIOB->BSRR |= GPIO_BSRR_BR9;

	// transfer control/data
	// wait for transmitter buffer to be empty
	while ((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE);
	// once TXE is HIGH (buffer is empty) put data into SPI-DR
	SPI2->DR = send_data;

	// wait for transfer to complete (i.e. RXNE goes HIGH)
	while ((SPI2->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
	// read the data register to clear the RXNE flag
	uint16_t receive_data = SPI2->DR;

	// make CS line (PB9) go HIGH
	GPIOB->BSRR |= GPIO_BSRR_BS9;

	return receive_data;
}

