/*
 * RTC.h
 *
 * Created: 07/09/2021 20:00:52
 *  Author: Milosz
 */ 


#ifndef RTC_DS1302_H_
#define RTC_DS1302_H_

#include <avr/io.h>


#define HOURS_REG 0x85

#define PIN_ENABLE D, 1
#define PIN_DATA  D, 2
#define PIN_CLOCK  D, 3
#define PIN_POWER D, 5
#define PIN_GND D, 7

void send_byte(uint8_t byte);

uint8_t read_byte();

void next_bit(void);

void rtc_init(void);

uint8_t read_hours(void);

void set_hour(uint8_t byte);



#endif /* RTC_H_ */