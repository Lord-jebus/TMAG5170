#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "src/pwm.h"
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
	cli();
	UART_init(MYUBRR);
	spi_init();
	cs_high();
	sei();
	
	_delay_ms(100);
	conf_TMAG();
	char buffer[80];
	
	// PWM inicializado en ceros
	//Salida PB01 por usar timer 1
	PWM_init(0, 0); 
	
	while (1) {
		//sensado
		float x_mTe = 0;
		float y_mTe = 0;
		float z_mTe = 0;		
		for (int i=0; i<10;i++){			
			sensar();	
			x_mTe = x_mTe + x_mT;
			y_mTe = y_mTe + y_mT;
			z_mTe = z_mTe + z_mT;
		}
		x_mTe = x_mTe / 10;
		y_mTe = y_mTe / 10;
		z_mTe = z_mTe / 10;		
				
		snprintf(buffer, sizeof(buffer), "RAW: VAL -> X:%.2f mT  Y:%.2f mT  Z:%.2f mT \r\n", x_mT, y_mT, z_mT);
		UART_sendString(buffer);

		snprintf(buffer, sizeof(buffer), "PROM: VAL -> X:%.2f mT  Y:%.2f mT  Z:%.2f mT \r\n", x_mTe, y_mTe, z_mTe);
		UART_sendString(buffer);

		_delay_ms(500);
		
		// recepción de datos via uart
		if (UART_availableLine()) {
			UART_readLine(buffer);
			UART_sendString("Recibido: ");
			UART_sendString(buffer);
			UART_sendString("\r\n");
		}
		
		// establecer el PWM
		uint16_t freq = 0;
		uint8_t duty = 0;
		PWM_set(freq, duty);
		
	}
}