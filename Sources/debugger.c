/*
 * debugger.c
 *
 * Created: 13/09/2021 03:54:19
 *  Author: Milosz
 */ 

#include "debugger.h"
#include <util/delay.h>

void print_byte(uint8_t byte){
	begin_message();
	for(uint8_t i = 0; i < 8; i+= 1){
		if (byte & 1){
			 PORTC = 1 << 3;
			 _delay_ms(500);
		}
		else {
			PORTC = 1 << 3;
			_delay_ms(125);
			PORTC = 0;
			_delay_ms(125);
			PORTC = 1 << 3;
			_delay_ms(125);
			PORTC = 0;
			_delay_ms(125);
		}
		PORTC = 0;
		_delay_ms(500);
		
		byte >>= 1;
	}
	end_message();
	return;
}

void clear_display() { 
	PORTD = 0;
}

void begin_message(void){
	DDRC = 1 << 3;
	for(uint8_t i = 0; i < 3; i += 1){
		PORTC = 1 << 3;
		_delay_ms(200);
		PORTC = 0;
		_delay_ms(200);
	}
}

void end_message(void){
	DDRC = 1 << 3;
	for(uint8_t i = 0; i < 5; i += 1){
		PORTC = 1 << 3;
		_delay_ms(200);
		PORTC = 0;
		_delay_ms(200);
	}
}