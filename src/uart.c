/*
 * CFile1.c
 *
 * Created: 8/8/2025 15:24:53
 *  Author: ADMIN
 */ 
#include "uart.h"
#include <string.h>

volatile char uart_buffer[UART_BUFFER_SIZE];
volatile uint8_t uart_head = 0;
volatile uint8_t uart_tail = 0;
volatile char uart_line[UART_LINE_SIZE];
volatile uint8_t uart_line_ready = 0;

// ------------------ Funciones UART ------------------

void UART_init(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;

	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_send(char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void UART_sendString(const char *str) {
	while (*str) {
		UART_send(*str++);
	}
}

void UART_print_hex16(uint16_t val) {
	char buf[5];
	for (int i = 0; i < 4; i++) {
		uint8_t nibble = (val >> ((3 - i) * 4)) & 0x0F;
		buf[i] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
	}
	buf[4] = 0;
	UART_sendString(buf);
}

// ------------------ Funciones RX ------------------
uint8_t UART_available(void) {
	return (uart_head != uart_tail);
}

char UART_read(void) {
	if (uart_head == uart_tail) {
		return 0; // no hay datos
	}
	char data = uart_buffer[uart_tail];
	uart_tail = (uart_tail + 1) % UART_BUFFER_SIZE;
	return data;
}

uint8_t UART_availableLine(void) {
	return uart_line_ready;
}

void UART_readLine(char *dest) {
	if (uart_line_ready) {
		strcpy(dest, (char*)uart_line);
		uart_line_ready = 0; // limpiar flag
		} else {
		dest[0] = '\0'; // no hay línea
	}
}

// ------------------ ISR RX ------------------
ISR(USART_RX_vect) {
	static uint8_t line_index = 0;
	char data = UDR0;

	// Guardar en buffer circular (para compatibilidad con UART_read)
	uint8_t next_head = (uart_head + 1) % UART_BUFFER_SIZE;
	if (next_head != uart_tail) {
		uart_buffer[uart_head] = data;
		uart_head = next_head;
	}

	// Construir línea hasta encontrar '\n' o '\r'
	if (!uart_line_ready) {
		if (data == '\n' || data == '\r') {
			uart_line[line_index] = '\0';
			uart_line_ready = 1;
			line_index = 0;
			} else {
			if (line_index < (UART_LINE_SIZE - 1)) {
				uart_line[line_index++] = data;
				} else {
				line_index = 0; // overflow ? reset
			}
		}
	}
}