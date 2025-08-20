/*
 * CFile1.c
 *
 * Created: 20/8/2025 18:54:37
 *  Author: ADMIN
 */ 
#include "pwm.h"

#define FREQ_CPU 16000000UL

void PWM_init(uint16_t freq, uint8_t duty) {
	// Configurar pin OC1A (PB1) como salida
	DDRB |= (1 << PB1);

	// Modo Fast PWM, TOP = ICR1
	TCCR1A = (1 << COM1A1);               // OC1A en modo no-inversor
	TCCR1B = (1 << WGM13) | (1 << WGM12); // Fast PWM con ICR1 como TOP
	TCCR1A |= (1 << WGM11);

	// Prescaler = 8 (suficiente para muchas frecuencias)
	TCCR1B |= (1 << CS11);

	PWM_set(freq, duty);
}

void PWM_set(uint16_t freq, uint8_t duty) {
	// Calcular TOP (ICR1) según frecuencia
	uint32_t top = (FREQ_CPU / (8UL * freq)) - 1;
	if (top > 65535) top = 65535; // limitar al máximo de 16 bits
	ICR1 = (uint16_t)top;

	// Calcular OCR1A según duty (%)
	uint32_t ocr = ((uint32_t)duty * (top + 1)) / 100;
	OCR1A = (uint16_t)ocr;
}
