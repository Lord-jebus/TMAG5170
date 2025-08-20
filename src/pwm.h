/*
 * IncFile1.h
 *
 * Created: 20/8/2025 18:54:48
 *  Author: ADMIN
 */ 

#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

void PWM_init(uint16_t freq, uint8_t duty);
void PWM_set(uint16_t freq, uint8_t duty);

#endif
