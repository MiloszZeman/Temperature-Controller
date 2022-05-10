/*
 * RTC_DS1302.c
 *
 * Created: 07/09/2021 20:01:18
 *  Author: Milosz
 */ 

#include "RTC_DS1302.h"
#include <util/delay.h>

#define RESET 1
#define DATA 2
#define CLK 3

#define GLUE(a, b)     a##b

/* single-bit macros, used for control bits */
#define SET_(what, p, m) GLUE(what, p) |= (1 << (m))
#define CLR_(what, p, m) GLUE(what, p) &= ~(1 << (m))
#define GET_(/* PIN, */ p, m) GLUE(PIN, p) & (1 << (m))
#define SET(what, x) SET_(what, x)
#define CLR(what, x) CLR_(what, x)
#define GET(x) (GET_(x))


void send_byte(uint8_t byte){
	for(uint8_t b = 0; b < 8; b++){
		if (byte & 0x01) SET(PORT, PIN_DATA);
		else CLR(PORT, PIN_DATA);
		next_bit();
		byte >>= 1;
	}
}

uint8_t read_byte(){
	uint8_t byte = 0;
	for(uint8_t b = 0; b < 8; b++){
		if(PIND & (1 << DATA)) byte |= 0x01 << b;
		next_bit();
	}
	return byte;
}

void next_bit(){
	SET(PORT, PIN_CLOCK);
	_delay_us(1);
	CLR(PORT, PIN_CLOCK);
	_delay_us(1);
}

void rtc_init(){
	SET(DDR, PIN_GND);
	SET(DDR, PIN_POWER);
	SET(DDR, PIN_ENABLE);
	SET(DDR, PIN_CLOCK);
	SET(DDR, PIN_DATA);
	
	CLR(PORT, PIN_ENABLE);
	CLR(PORT, PIN_CLOCK);
	CLR(PORT, PIN_DATA);
	CLR(PORT, PIN_GND);
	SET(PORT, PIN_POWER);
}


uint8_t read_hours(){
	
	uint8_t hours = 0;
	uint8_t command = HOURS_REG;
	
	rtc_init();
	
	SET(PORT, PIN_ENABLE);
	
	send_byte(command);	
		
	CLR(DDR, PIN_DATA);
	CLR(PORT, PIN_DATA);
	
	hours = read_byte();
		
	CLR(PORT, PIN_ENABLE);
	
	return hours;
}

void set_hour(uint8_t byte){
	uint8_t command = 0b100000000 | 0x84;
	
	rtc_init();
	SET(PORT, PIN_ENABLE);
	send_byte(command);
	send_byte(byte);
}