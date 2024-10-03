#include <avr/io.h>
#include <avr/interrupt.h>

#include "mtr_planner/mtr_planner.h"

#include "inits.h"

int main(void)
{
	cli();
	
	Init_GPIO();
	Init_Timer1();
	Init_Timer3();
	Init_Planner();
	Init_I2C();
	
	sei();
	
    while (1) 
    {
		Planner_runTasks();
	}
}

