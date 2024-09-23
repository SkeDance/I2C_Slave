#include <avr/io.h>
#include <avr/interrupt.h>

#include "mtr_planner/mtr_planner.h"

#include "inits.h"

int main(void)
{
	cli();
	
	Init_I2C();
	Init_Timer1();
	Init_Timer2();
	
	
	sei();
	
    while (1) 
    {
		Planner_runTasks();
    }
}

