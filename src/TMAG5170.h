/*
 * TMAG5170.h
 *
 * Created: 8/8/2025 15:38:18
 *  Author: ADMIN
 */ 


#ifndef TMAG5170_H_
#define TMAG5170_H_
#include "spi_gen.h"

void cs_low();
void cs_high();
void write_register(uint8_t reg, uint8_t valA, uint8_t valB, uint8_t cmd);
uint16_t read_register(uint8_t reg, uint8_t valA, uint8_t valB, uint8_t cmd);



#endif /* TMAG5170_H_ */