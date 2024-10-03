#include "inits.h"

void Init_GPIO(){
	Port_init(PORT_DO1, OUT, false);
	Port_init(PORT_DO2, OUT, false);
	Port_init(PORT_DO3, OUT, false);
	Port_init(PORT_DO4, OUT, false);
	Port_init(PORT_DO5, OUT, false);
	Port_init(PORT_DO6, OUT, false);
	Port_init(PORT_DO7, OUT, false);
	Port_init(PORT_DO8, OUT, false);
	Port_init(PORT_DO9, OUT, false);
	Port_init(PORT_DO10, OUT, false);
	Port_init(PORT_DO11, OUT, false);
	Port_init(PORT_TEST, OUT, false);
	Port_init(PORT_TEST2, OUT, false);
	
}
void Init_I2C(){
	I2C_init(I2C_SLAVE_ADDRESS);
}

void Inits_data(){
	Data_init(Crc_getCrc16Flash(FLASH_MEMORY_SIZE));
}

void Init_Timer1(){
	/*
	TCCR1B |= (1 << WGM12);
	TIMSK |= (1 << OCIE1A);
	OCR1AH = 0b01111010; // число для сравнения
	OCR1AL = 0b00010010;
	TCCR1B |= (1 << CS12);// | (0 << CS11) | (1 << CS10);
	//SREG = 0b10000000;
	*/
	TIMSK = (1 << OCIE1A);
	TCCR1A = (0 << WGM11) | (0 << WGM10);
	TCCR1B = (1 << CS10) | (0 << CS11) | (0 << CS12) | (0 << WGM13) | (1 << WGM12);
	OCR1A = 1000;
	
}

void Init_Timer3(){
	ETIMSK = (1 << OCIE3A);
	TCCR3A = (0 << WGM31) | (0 << WGM30);
	TCCR3B = (1 << CS30) | (0 << CS31) | (0 << CS32) | (0 << WGM33) | (1 << WGM32);
	OCR3A = 1000;
}

void Init_Planner(){
	Planner_addTask(test_led, 500, true);
	Planner_addTask(updateI2C_data, 500, true);
	Planner_init();
}