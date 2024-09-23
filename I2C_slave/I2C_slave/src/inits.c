#include "inits.h"

void Init_I2C(){
	//I2C_init(I2C_SLAVE_ADDRESS);
}

void Init_Timer1(){
	TIMSK = (1 << OCIE2) | (1 << OCIE0);
	TCCR0 = (1 << CS00) | (1 << CS01) | (1 << CS02) |
	(0 << WGM00) | (1 << WGM01);
	OCR0 = 7;	
}

void Init_Timer2(){
	TIMSK = (1 << OCIE2) | (1 << OCIE0);
	TCCR2 = (1 << CS20) | (0 << CS21) | (1 << CS22) |
	(0 << WGM20) | (1 << WGM21);
	OCR2 = 7;
}

void Init_Planner(){
	
}