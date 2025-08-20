/*
 * uart.h
 *
 * Created: 8/8/2025 15:24:45
 *  Author: ADMIN
 *  Libreria para recibir y enviar por puerto serial
 */ 


#ifndef uart_H_
#define uart_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

// Tamaño del buffer de recepción
#define UART_BUFFER_SIZE 64
#define UART_LINE_SIZE   64

void UART_init(unsigned int ubrr);
void UART_send(char data);
void UART_sendString(const char *str);
void UART_print_hex16(uint16_t val);

// Nuevas funciones para RX
uint8_t UART_available(void);
char UART_read(void);
uint8_t UART_availableLine(void);
void UART_readLine(char *dest);

#endif