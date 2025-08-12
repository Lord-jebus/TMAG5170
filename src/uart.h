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

void UART_init(unsigned int ubrr);
void UART_send(char data);
void UART_sendString(const char *str);
void UART_print_hex16(uint16_t val);
//char UART_receive(void);
//int UART_waitForOK(void);


#endif /* uart_H_ */