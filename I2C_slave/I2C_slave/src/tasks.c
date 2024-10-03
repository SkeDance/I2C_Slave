#include "tasks.h"

	volatile static int z = 0;
	volatile static int T = 1800;
	volatile static int t = 0;
	volatile static int number = 0;
	volatile static int single_digit;
	volatile static int decemical_digit;
	volatile static int hundredth_digit;
	volatile static int thounsandth_digit;
	
void count0 (){
	
	switch (number){
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

ISR(TIMER1_COMPA_vect){
	
	/* for test /////////////////////////////////
	Data_setReg(8, 1); // zapis` v 8 registr 1
	if (Data_getReg(8) == 1){
		static bool flag = true;
		flag = !flag;
		if (flag){
			Port_setPin(PORT_TEST2);
		} 
		else{
			Port_resetPin(PORT_TEST2);
		}
	}
	
	*/////////////////////////////////////////////
	
	z++;
	if (z == 2000){
		--T;
		z = 0;
	}
	if (T == 0){
		T = 1800;
	}
}

ISR(TIMER3_COMPA_vect){
	static int m = 0;
	PORTE &=~ ((1<<7)|(1<<6)|(1<<5)|(1<<4));
	if (m == 0){
		number = single_digit;
		count0(number);
		PORTE |=(1<<5);
	}
	if (m == 3){
		number = decemical_digit;
		count0(number);
		PORTE |=(1<<4);
	}
	if (m == 1){
		number = hundredth_digit;
		count0(number);
		PORTE |=(1<<6);
	}
	if (m == 2){
		number = thounsandth_digit;
		count0(number);
		PORTE |=(1<<7);
	}	
	m++;
	if (m == 4){
		m = 0;
	}
}
void test_led(){
		
	static bool flag = true;
	flag = !flag;
	
	if (flag){
		Port_setPin(PORT_TEST);
		} else{
		Port_resetPin(PORT_TEST);
	}
}

void updateI2C_data(){
	single_digit  = Data_getReg(11);
	decemical_digit  = Data_getReg(10);
	hundredth_digit  = Data_getReg(9);
	thounsandth_digit  = Data_getReg(8);	
		
	if (Data_getReg(11) == single_digit){
		static bool flag = true;
		flag = !flag;
		if (flag){
			Port_setPin(PORT_TEST2);
		}
		else{
			Port_resetPin(PORT_TEST2);
		}
	}
}