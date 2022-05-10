/*
 * Termostat.c
 *
 * Created: 07/09/2021 19:56:41
 * Author : Milosz
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "debugger.h"
#include "1wire.h"
#include "1wire_basic.h"
#include "1wire_defines.h"
#include "DS18B20.h"

//#define DEBUG_HOUR

#define DAY_TEMP 29
#define NIGHT_TEMP 25
#define NIGHT_START 22
#define DAY_START 8

#define DS1307_HOURS_REG 0x02
#define DS1307_ADDRESS 0xD1

static inline void I2C_WaitForComplete(void) {while (!(TWCR & _BV(TWINT)));}

uint8_t BDC_TO_DC(uint8_t hours){
	return ((hours & 0b00110000) >> 4) * 10 + (hours & 0x0F);
}

int main(void)
{
	
	uint8_t thermometer_address[8] = {0x28, 0x39, 0xE0 ,0x76, 0xE0, 0x01, 0x3C, 0x00};
	uint8_t temp_to_keep;
	
	_delay_ms(1000);
	
	// WYLACZ PRZEKAZNIK
	DDRC |= 1 << 1;
	PORTC &=  ~(0 << 1);
	
	// REZYSTORY PODCIAGAJACE DLA I2C
	PORTC |= 1 << 4;
	PORTC |= 1 << 5;
	
    while (1) {
		
		#ifdef DEBUG_HOUR
	
		uint8_t hours = read_hours();
		//print_byte(BDC_TO_DC(hours));
		_delay_ms(1000);
	
		#endif
	
		#ifndef DEBUG_HOUR
		
		uint8_t hours;
	
		// W??CZENIE TWI
		TWCR = _BV(TWEN);
		
		// OKRE?LENIE PR?DKO?CI MAGISTRALI NA 50kHz
		TWBR = 39;
		TWSR |= 1;
		
		// SYGNA? START
		TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
		I2C_WaitForComplete();
		//print_byte(TW_STATUS); // 0b00010000
		
		// WYS?ANIE ADRESU URZ?DZENIA I BITU OZNACZAJ?CEGO ZAPIS
		TWDR = DS1307_ADDRESS - 1;
		TWCR = _BV(TWINT) | _BV(TWEN);
		I2C_WaitForComplete();
		//print_byte(TW_STATUS); // 0b00011000
		
		
		// Wyslanie bajta wskazuj?cego na adres odczytu
		TWDR = 0x02;
		TWCR = _BV(TWINT) | _BV(TWEN);
		I2C_WaitForComplete();
		//print_byte(TW_STATUS); // 0b00010100
		
		// Wyslanie powtórzonego startu
		TWCR = _BV(TWSTA) | _BV(TWINT) | _BV(TWEN);
		I2C_WaitForComplete();
		//print_byte(TW_STATUS); // 0b00001000
		
		// wyslanie SLA+R
		TWDR = DS1307_ADDRESS;
		TWCR = _BV(TWEN) | _BV(TWINT);
		I2C_WaitForComplete();
		//print_byte(TW_STATUS); // 0b00000010
		
		// odczytanie bajtu, odes?anie NACK
		TWCR = _BV(TWEN) | _BV(TWINT);
		I2C_WaitForComplete();
		hours = TWDR;
		//print_byte(TW_STATUS); // 0b00011010
		
		// wyslanie stop
		TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
		while (TWCR & _BV(TWSTO));
		
		
		hours = BDC_TO_DC(hours);
		if (hours >= NIGHT_START || hours < DAY_START) temp_to_keep = NIGHT_TEMP;
		else temp_to_keep = DAY_TEMP;
	
		OW_Start_Conversion(1);
		int16_t temp = (OW_GetTemperature(thermometer_address) >> 2); // 2 LSB are not defined due to resolution, hence the bit move
	
		if (temp < ((temp_to_keep - 1) << 2) + 3) {
			PORTC |= 1 << 1;
			PORTC |= 1 << 3;
		} else if (temp > ((temp_to_keep << 2) + 1)){
			PORTC &= ~(1 << 1);
			PORTC &= ~(1 << 3);
		}
	
		_delay_ms(500);
	
		#endif
	
	}
}

