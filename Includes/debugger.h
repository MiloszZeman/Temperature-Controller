/*
 * debugger.h
 *
 * Created: 13/09/2021 03:52:46
 *  Author: Milosz
 */ 


#ifndef DEBUGGER_H_
#define DEBUGGER_H_
#include <avr/io.h>

void print_byte(uint8_t byte);
void clear_display(void);
void begin_message(void);
void end_message(void);






#endif /* DEBUGGER_H_ */