#ifndef MTR_PLANNER_H_
#define MTR_PLANNER_H_

#ifdef __cplusplus
extern "C" {
#endif

	#include "mtr_service/mtr_service.h"
	#include <stdint.h>
	#include <stdbool.h>

	typedef struct{

		uint32_t taskAddress;
		uint32_t msTimeout;
		uint32_t msLeft;
		uint32_t nextTask;
		bool enabled;

	}Task_t;

	typedef struct{
		
		Task_t* firstTask;
		Task_t* currentTask;
		Task_t* lastTask;

	}Planner_t;


	void Planner_init();
	Task_t* Planner_addTask(uint32_t function, uint32_t msTimeout, bool enabled);
	Task_t* Planner_getTask(uint32_t function);
	//void Planner_start();
	void Planner_reset(Task_t* task);
	void Planner_pause(Task_t* task);
	void Planner_resume(Task_t* task);
	void Planner_runTasks();

#ifdef __cplusplus
}
#endif

#endif /*MTR_PLANNER_H_ */
