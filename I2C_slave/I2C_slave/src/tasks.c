#include "tasks.h"

	volatile static int z = 0;
	volatile static int T = 1800;
	volatile static int t = 0;
	volatile static int e = 0;
	
void count0 (){
	
	switch (e){
		case 0:{
			PORTE = (1 << 1) | (1 << 0) | (1 << 2);
			PORTB = (1 << 6) | (1 << 2) | (1 << 3) | (0 << 5); //0
			break;
		}
		case 1:{
			PORTE = (0 << 1) | (1 << 0) | (1 << 2);
			PORTB = (0 << 6) | (0 << 2) | (0 << 3) | (0 << 5); //1
			break;
		}
		case 2:{
			PORTE = (1 << 1) | (1 << 0) | (0 << 2);
			PORTB = (0 << 6) | (1 << 2) | (1 << 3) | (1 << 5); //2
			break;
		}
		case 3:{
			PORTE = (1 << 1) | (1 << 0) | (1 << 2);
			PORTB = (0 << 6) | (0 << 2) | (1 << 3) | (1 << 5); //3
			break;
		}
		case 4:{
			PORTE = (0 << 1) | (1 << 0) | (1 << 2);
			PORTB = (1 << 6) | (0 << 2) | (0 << 3) | (1 << 5); //4
			break;
		}
		case 5:{
			PORTE = (1 << 1) | (0 << 0) | (1 << 2);
			PORTB = (1 << 6) | (0 << 2) | (1 << 3) | (1 << 5); //5
			break;
		}
		case 6:{
			PORTE = (1 << 1) | (0 << 0) | (1 << 2);
			PORTB = (1 << 6) | (1 << 2) | (1 << 3) | (1 << 5); //6
			break;
		}
		case 7:{
			PORTE = (1 << 1) | (1 << 0) | (1 << 2);
			PORTB = (0 << 6) | (0 << 2) | (0 << 3) | (0 << 5); //7
			break;
		}
		case 8:{
			PORTE = (1 << 1) | (1 << 0) | (1 << 2);
			PORTB = (1 << 6) | (1 << 2) | (1 << 3) | (1 << 5); //8
			break;
		}
		case 9:{
			PORTE = (1 << 1) | (1 << 0) | (1 << 2);
			PORTB = (1 << 6) | (0 << 2) | (1 << 3) | (1 << 5); //9
			break;
		}
	}
}

ISR(TIMER0_COMP_vect)
{
	z++;
	if (z == 2000){
		--T;
		z = 0;
	}
	if (T == 0){
		T = 1800;
	}
}

ISR(TIMER2_COMP_vect)
{
	static int m = 0;
	PORTE &=~ ((1<<7)|(1<<6)|(1<<5)|(1<<4));
	if (m == 0){
		t = T / 60;
		e = t % 10;
		count0(e);
		PORTE |=(1<<6);
	}
	if (m == 1){
		t = T / 60;
		e = t / 10;
		count0(e);
		PORTE |=(1<<7);
	}
	if (m == 2){
		t = T % 60;
		e = t % 10;
		count0(e);
		PORTE |=(1<<5);
	}
	if (m == 3){
		t = T % 60;
		e = t / 10;
		count0(e);
		PORTE |=(1<<4);
	}
	m++;
	if (m == 4){
		m = 0;
	}
}