/*
 * spi_gen.c
 *
 * Created: 8/8/2025 15:31:53
 *  Author: ADMIN
 */ 

#include <avr/io.h>

#define CS_PIN  PB2

void spi_init() {
	// MOSI, SCK y SS como salida
	DDRB |= (1 << PB3) | (1 << PB5) | (1 << CS_PIN);
	// Habilitar SPI, Modo Master, Fosc/16, modo 0
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

uint8_t spi_transfer(uint8_t data) {
	SPDR = data;
	while (!(SPSR & (1 << SPIF))); // esperar fin de transmisión
	return SPDR;
}