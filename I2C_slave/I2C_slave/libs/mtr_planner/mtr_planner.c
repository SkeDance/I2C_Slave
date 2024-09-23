#include "mtr_planner.h"
#include "mtr_port/mtr_port.h"
#include <avr/io.h>
#include <avr/interrupt.h>

Planner_t Planner;

void Planner_init()
{
	uint16_t timerDivisor = 256;
	
	#ifdef WORK_PLANNER_TIMER1A
	//Timer 1
	TIMSK &= ~(1 << OCIE1A | 1 << TOIE1);	 // disable interrupt
	
	TCCR1A = 0; // normal operating
	TCCR1B &= ~(1 << CS10 | 1 << CS11 | 1 << CS12); // stop clocking
	TCCR1C &= ~(1 << FOC1A); //?????
	TCNT1 = 0;				 //reset counter
	
	OCR1A = (CPU_FREQUENCY / timerDivisor) / 1000;	// set top value
	
	TCCR1C |= (1 << FOC1A);
	TCCR1B |= 1 << WGM12;  // CTC MODE
	TIMSK |= (1 << OCIE1A | 1 << TOIE1);	 // enable interrupts
	
	TCCR1B |= 1 << CS12;
	
	#elif defined(WORK_PLANNER_TIMER3A)
	//Timer 3
	ETIMSK &= ~(1 << OCIE3A | 1 << TOIE3);	 // disable interrupt
	
	TCCR3A = 0; // normal operating
	TCCR3B &= ~(1 << CS30 | 1 << CS31 | 1 << CS32); // stop clocking
	TCCR3C &= ~(1 << FOC3A); //?????
	TCNT3 = 0;				 //reset counter
	
	OCR3A = (CPU_FREQUENCY / timerDivisor) / 1000;	// set top value
	
	TCCR3C |= (1 << FOC3A);
	TCCR3B |= 1 << WGM32;  // CTC MODE
	ETIMSK |= (1 << OCIE3A | 1 << TOIE3);	 // enable interrupts
	
	TCCR3B |= 1 << CS32;
	
	#elif defined(WORK_PLANNER_TIMER0)
	//Timer 0
	TIMSK &= ~(1 << OCIE0 | 1 << TOIE0);	 // disable interrupt
	
	TCCR0 = 0;		//disable all
	TCNT0 = 0;		//reset counter
	
	OCR0 = (CPU_FREQUENCY / timerDivisor) / 1000;	// set top value
	
	TCCR0 |= (1 << FOC0 | 1 << WGM01);
	TIMSK |= (1 << OCIE0 | 1 << TOIE0);
	TCCR0 |= (1 << CS02 | 1 << CS01);		//set divisor 256 and start timer
	
	#else
		#error "There no planner timer"
	
	#endif
}


Task_t* Planner_addTask(uint32_t function, uint32_t msTimeout, bool enabled){
	
	//create new task
	Task_t* newTask = calloc(sizeof(Task_t), 1);
	uint8_t ranVal = rand();
	newTask->msLeft = (ranVal > msTimeout) ? 1 : ranVal;
	newTask->msTimeout = msTimeout;
	newTask->taskAddress = function;
	newTask->nextTask = 0;
	newTask->enabled = enabled;
	
	if(!Planner.firstTask){
		Planner.firstTask = newTask;
	}
	
	if (Planner.lastTask != NULL) {
		Planner.lastTask->nextTask = newTask;
	}
	
	Planner.lastTask = newTask;
	
	return newTask;
}


Task_t* Planner_getTask(uint32_t functionAddress){
	
	Task_t* task = Planner.firstTask;
	for (;;){
		if (task->taskAddress == functionAddress){
			
			return task;
			
		}
		if (task->nextTask){
			task = task->nextTask;
		}
		else
		{
			break;
		}
	}//1 for
	
	return 0;
}


void _timerDecreaser(Task_t* task){
	if (task->msLeft != 0){
		task->msLeft = task->msLeft - 1;
	}
}

/*
void Planner_start()
{
	#ifdef WORK_PLANNER_TIMER1A
	TCCR1B |= 1 << CS10;
	#else
	TCCR3B |= 1 << CS30;
	#endif

}
*/

void Planner_reset(Task_t* task) {
	task->msLeft = task->msTimeout;
}

void Planner_pause(Task_t* task) {
	task->enabled = false;
}

void Planner_resume(Task_t* task) {
	task->enabled = true;
}

#ifdef WORK_PLANNER_TIMER1A
ISR(TIMER1_COMPA_vect){
	
	//MTR_Timer_stop(CONFIG_PLANNER_TIMER);
	//MTR_Timer_setCounter(CONFIG_PLANNER_TIMER, 0);
	Task_t * currentTask = Planner.firstTask;
	
	for(;;){
		
		if (currentTask->enabled){
			_timerDecreaser(currentTask);
		}
		
		if (currentTask->nextTask){
			currentTask = currentTask->nextTask;
		}
		else
		{
			break;
		}
	}
	
	//MTR_Timer_start(CONFIG_PLANNER_TIMER);
}
#elif defined(WORK_PLANNER_TIMER3A)
ISR(TIMER3_COMPA_vect)
{
	//MTR_Timer_stop(CONFIG_PLANNER_TIMER);
	//MTR_Timer_setCounter(CONFIG_PLANNER_TIMER, 0);
	Task_t * currentTask = Planner.firstTask;
	
	for(;;){
		
		if (currentTask->enabled){
			_timerDecreaser(currentTask);
		}
		
		if (currentTask->nextTask){
			currentTask = currentTask->nextTask;
		}
		else
		{
			break;
		}
	}
	//MTR_Timer_start(CONFIG_PLANNER_TIMER);
}
#elif defined(WORK_PLANNER_TIMER0)
ISR(TIMER0_COMP_vect){
	
	//MTR_Timer_stop(CONFIG_PLANNER_TIMER);
	//MTR_Timer_setCounter(CONFIG_PLANNER_TIMER, 0);
	Task_t * currentTask = Planner.firstTask;
	
	for(;;){
		
		if (currentTask->enabled){
			_timerDecreaser(currentTask);
		}
		
		if (currentTask->nextTask){
			currentTask = currentTask->nextTask;
		}
		else
		{
			break;
		}
	}
	
	//MTR_Timer_start(CONFIG_PLANNER_TIMER);
}
#endif

void Planner_runTasks()
{
	Task_t * currentTask = Planner.firstTask;
	//Planner.currentTask = Planner.firstTask;
	while(currentTask != NULL){
		
		if (currentTask->msLeft == 0 && currentTask->enabled){
			
			typedef  void (*voidFunction)(Task_t*);
			voidFunction voidRunTaskFun;
			
			voidRunTaskFun = (voidFunction) (currentTask->taskAddress);
			currentTask->msLeft = currentTask->msTimeout;
			
			voidRunTaskFun(currentTask);
		}
		
		currentTask = currentTask->nextTask;
	}
}