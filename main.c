#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "src/uart.h"
#include "src/TMAG5170.h"

// Define Serial Communication
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

// SPI Commands TMAG5170
#define TEST_CONFIG    0x0F
#define SENSOR_CONFIG  0x01
#define DEVICE_CONFIG  0x00

#define X_CH_RESULT    0x89
#define Y_CH_RESULT    0x8A
#define Z_CH_RESULT    0x8B
#define TEMP_RESULT    0x8C // Temperatura

float x_mT,y_mT,z_mT;

void sensar(){
	uint16_t x_raw = read_register(X_CH_RESULT, 0x00, 0x00, 0x00);
	uint16_t y_raw = read_register(Y_CH_RESULT, 0x00, 0x00, 0x00);
	uint16_t z_raw = read_register(Z_CH_RESULT, 0x00, 0x00, 0x00);
	x_mT = ((int16_t)x_raw) * (100.0 / 32768.0);
	y_mT = ((int16_t)y_raw) * (100.0 / 32768.0);
	z_mT = ((int16_t)z_raw) * (100.0 / 32768.0);	
}

void conf_TMAG(){
	//Config TMAG5170
	// Desactivar CRC
	write_register(TEST_CONFIG, 0x00, 0x04, 0x07);
	_delay_ms(50);
	// Configurar canales X/Y/Z y ±100 mT
	write_register(SENSOR_CONFIG, 0x19, 0xEA, 0x00);
	_delay_ms(50);
	// Modo activo continuo
	write_register(DEVICE_CONFIG, 0x31, 0x20, 0x00);
	_delay_ms(100);
}

int main(void) {
	UART_init(MYUBRR);
	spi_init();
	cs_high();

	_delay_ms(100);
	conf_TMAG();

	while (1) {
		sensar();
		
		char buffer[80];		
		snprintf(buffer, sizeof(buffer), "VAL -> X:%.2f mT  Y:%.2f mT  Z:%.2f mT \r\n", x_mT, y_mT, z_mT);
		UART_sendString(buffer);

		_delay_ms(500);
	}
}






/* Prueba con temperatura


#define F_CPU 16000000U // Clock Speed

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

// Define CS pin
#define CS_DDR  DDRB
#define CS_PORT PORTB
#define CS_PIN  PB2

// SPI Commands
#define TEST_CONFIG    0x0F
#define SENSOR_CONFIG  0x01
#define DEVICE_CONFIG  0x00

#define X_CH_RESULT    0x89
#define Y_CH_RESULT    0x8A
#define Z_CH_RESULT    0x8B
#define TEMP_RESULT    0x8C // Temperatura

// --- SPI Functions ---
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

// --- TMAG5170 Functions ---
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

// --- UART Functions (9600 baud @ 16MHz) ---
void uart_init() {
	UBRR0H = 0;
	UBRR0L = 103; // 9600 baud
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
}

void uart_tx(uint8_t data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void uart_print(const char* str) {
	while (*str) uart_tx(*str++);
}

void uart_print_hex16(uint16_t val) {
	char buf[5];
	for (int i = 0; i < 4; i++) {
		uint8_t nibble = (val >> ((3 - i) * 4)) & 0x0F;
		buf[i] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
	}
	buf[4] = 0;
	uart_print(buf);
}

// --- Main ---
int main(void) {
	uart_init();
	spi_init();
	cs_high();

	_delay_ms(100);

	// Desactivar CRC
	write_register(TEST_CONFIG, 0x00, 0x04, 0x07);
	_delay_ms(50);

	// Configurar canales X/Y/Z y ±100 mT
	write_register(SENSOR_CONFIG, 0x19, 0xEA, 0x00);
	_delay_ms(50);

	// Modo activo continuo
	write_register(DEVICE_CONFIG, 0x31, 0x20, 0x00);
	_delay_ms(100);

	while (1) {
		uint16_t x_raw = read_register(X_CH_RESULT, 0x00, 0x00, 0x00);
		uint16_t y_raw = read_register(Y_CH_RESULT, 0x00, 0x00, 0x00);
		uint16_t z_raw = read_register(Z_CH_RESULT, 0x00, 0x00, 0x00);
		uint16_t t_raw = read_register(TEMP_RESULT, 0x00, 0x00, 0x00);

		float x_mT = ((int16_t)x_raw) * (100.0 / 32768.0);
		float y_mT = ((int16_t)y_raw) * (100.0 / 32768.0);
		float z_mT = ((int16_t)z_raw) * (100.0 / 32768.0);
		float temp_C = ((int16_t)t_raw) / 64.0;

		char buffer[80];
		snprintf(buffer, sizeof(buffer),
		"RAW -> X:0x"); uart_print(buffer);
		uart_print_hex16(x_raw);
		uart_print(" Y:0x"); uart_print_hex16(y_raw);
		uart_print(" Z:0x"); uart_print_hex16(z_raw);
		uart_print(" T:0x"); uart_print_hex16(t_raw);
		uart_print("\r\n");

		snprintf(buffer, sizeof(buffer),
		"VAL -> X:%.2f mT  Y:%.2f mT  Z:%.2f mT  Temp:%.2f C\r\n",
		x_mT, y_mT, z_mT, temp_C);
		uart_print(buffer);

		_delay_ms(500);
	}
}

*/

/* Funcionando el campo magnético básico
Lógica del programa
1. Inicializa SPI en modo maestro, modo 0
2. Desactiva el CRC del TMAG5170 (registro 0x0F)
3. Configura los canales X, Y, Z y rango ±100?mT
4. Activa el sensor en modo continuo
5. En el loop: lee los registros de resultado y envía los valores por UART

*/

/*
#define F_CPU 16000000U // Clock Speed

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

// Define CS pin
#define CS_DDR  DDRB
#define CS_PORT PORTB
#define CS_PIN  PB2

// SPI Commands
#define TEST_CONFIG    0x0F
#define SENSOR_CONFIG  0x01
#define DEVICE_CONFIG  0x00

#define X_CH_RESULT    0x89
#define Y_CH_RESULT    0x8A
#define Z_CH_RESULT    0x8B

// --- SPI Functions ---
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

// --- TMAG5170 Functions ---

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

// --- UART Functions (9600 baud @ 16MHz) ---

void uart_init() {
	UBRR0H = 0;
	UBRR0L = 103; // 9600 baud
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
}

void uart_tx(uint8_t data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void uart_print(const char* str) {
	while (*str) uart_tx(*str++);
}

void uart_print_hex(uint16_t val) {
	char buf[7];
	for (int i = 0; i < 4; ++i) {
		uint8_t nibble = (val >> ((3 - i) * 4)) & 0x0F;
		buf[i] = nibble < 10 ? '0' + nibble : 'A' + nibble - 10;
	}
	buf[4] = '\r'; buf[5] = '\n'; buf[6] = 0;
	uart_print(buf);
}

// --- Main ---

int main(void) {
	uart_init();
	spi_init();
	cs_high();

	_delay_ms(100);

	// Desactivar CRC
	write_register(TEST_CONFIG, 0x00, 0x04, 0x07);
	_delay_ms(50);

	// Configurar canales X/Y/Z y ±100 mT
	write_register(SENSOR_CONFIG, 0x19, 0xEA, 0x00);
	_delay_ms(50);

	// Modo activo continuo
	write_register(DEVICE_CONFIG, 0x31, 0x20, 0x00);
	_delay_ms(100);

	while (1) {
		uint16_t x = read_register(X_CH_RESULT, 0x00, 0x00, 0x00);
		uint16_t y = read_register(Y_CH_RESULT, 0x00, 0x00, 0x00);
		uint16_t z = read_register(Z_CH_RESULT, 0x00, 0x00, 0x00);

		float x_mT = ((int16_t)x) * (100.0 / 32768.0);
		float y_mT = ((int16_t)y) * (100.0 / 32768.0);
		float z_mT = ((int16_t)z) * (100.0 / 32768.0);

		char buffer[64];
		snprintf(buffer, sizeof(buffer), "X:%.2f Y:%.2f Z:%.2f\r\n", x_mT, y_mT, z_mT);
		uart_print(buffer);

		uart_print("X: "); uart_print_hex(x);
		uart_print("Y: "); uart_print_hex(y);
		uart_print("Z: "); uart_print_hex(z);
		uart_print("\r\n");

		_delay_ms(500);
	}
}
*/