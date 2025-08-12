/*
 * TMAG5170.c
 *
 * Created: 8/8/2025 15:38:23
 *  Author: ADMIN
 */ 

#include <avr/io.h>
#include "spi_gen.h"

// Define CS pin
#define CS_DDR  DDRB
#define CS_PORT PORTB
#define CS_PIN  PB2

void cs_low()  { CS_PORT &= ~(1 << CS_PIN); }
void cs_high() { CS_PORT |= (1 << CS_PIN); }

void write_register(uint8_t reg, uint8_t valA, uint8_t valB, uint8_t cmd) {
	cs_low();
	spi_transfer(reg);
	spi_transfer(valA);
	spi_transfer(valB);
	spi_transfer(cmd);
	cs_high();
}

uint16_t read_register(uint8_t reg, uint8_t valA, uint8_t valB, uint8_t cmd) {
	uint8_t hi, lo;
	cs_low();
	spi_transfer(reg);
	hi = spi_transfer(valA);
	lo = spi_transfer(valB);
	spi_transfer(cmd);
	cs_high();
	return (hi << 8) | lo;
}
