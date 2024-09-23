#include "mtr_port.h"
#include <avr/io.h>

Port_t PORTS[] = {

	{&PORTA, &DDRA, &PINA},
	{&PORTB, &DDRB, &PINB},
	{&PORTC, &DDRC, &PINC},
	{&PORTD, &DDRD, &PIND},
	{&PORTE, &DDRE, &PINE},
	{&PORTF, &DDRF, &PINF},
	{&PORTG, &DDRG, &PING}

};

Port_ConnectedPins_t connectedPins;

void Port_deInit(Port_Name_t port){

	Port_init(port, pinAll, IN, false);
}

void Port_init(Port_Name_t port, Port_Pin_t pin, Port_Direction_t direction, bool pullUp){
 
	*(PORTS[port].outputData) &= ~pin;
 
   if (direction == IN)
   {
		*(PORTS[port].direction) &= ~pin;
   }
   else
   {      
		*(PORTS[port].direction) |= pin;   
   }
}


void Port_setPin(Port_Name_t port, Port_Pin_t pin){
	if (*(PORTS[port].direction) & pin){
		*(PORTS[port].outputData) |= pin;  
	}
}

void Port_resetPin(Port_Name_t port, Port_Pin_t pin){
    if (*(PORTS[port].direction) & pin){
	    *(PORTS[port].outputData) &= ~pin;
    }
}

bool Port_getPin(Port_Name_t port, Port_Pin_t pin){

	if (~(*(PORTS[port].direction)) & pin){
		return (bool) ((*PORTS[port].inputData) & pin);	
	}
	
	return false;
}

uint8_t Port_getPort(Port_Name_t port){

	return ((*PORTS[port].inputData) & ~(*PORTS[port].direction));
}

void Port_writePin(Port_Name_t port, Port_Pin_t pin, bool status){

	if (status){
		Port_setPin(port, pin);
	}
	else 
	{
		Port_resetPin(port, pin);
	}
}


void Port_writePort(Port_Name_t port, uint8_t data){
	for(uint8_t pin = 0; pin < 8; pin++){
		if (port == PortG && pin > 4){
			break;
		}
		Port_writePin(port, 1 << pin, getBit(data, pin));
	}
}


void Port_connectPin(Port_Name_t port, Port_Pin_t pin, Port_Direction_t direction, bool* value, bool pullUp){

	if (connectedPins.currentPinAmount < CONFIG_PORT_CONNECTED_PIN_AMOUNT){

		connectedPins.pins[connectedPins.currentPinAmount].portName = port;
		connectedPins.pins[connectedPins.currentPinAmount].portPin = pin;
		connectedPins.pins[connectedPins.currentPinAmount].direction = direction;
		connectedPins.pins[connectedPins.currentPinAmount].connectedWith = value;

		connectedPins.currentPinAmount += 1;

		Port_init(port, pin, direction, pullUp);
		
		if (direction == OUT){
			Port_resetPin(port, pin);
		}
	}
}

void Port_updatePins(){
	Port_ConnectedPin_t* currentPin;
	
	static uint8_t i = 0;
	
	currentPin = &connectedPins.pins[i];
	if (currentPin->direction == OUT){

		if (*currentPin->connectedWith){
			Port_setPin(currentPin->portName, currentPin->portPin);
		}
		else
		{
			Port_resetPin(currentPin->portName, currentPin->portPin);
		}
	}

	if (currentPin->direction == IN){
		*currentPin->connectedWith =
		Port_getPin(currentPin->portName, currentPin->portPin);
	}
	
	i = (i < ( connectedPins.currentPinAmount - 1 ) ) ? \
		(i + 1) : 0 ;
}

uint8_t* Port_getDirectionRegister(Port_Name_t port){
	
	return PORTS[port].direction;
}

uint8_t* Port_getOutputDataRegister(Port_Name_t port){
	
	return PORTS[port].outputData;
}

uint8_t* Port_getInputDataRegister(Port_Name_t port){
	
	return PORTS[port].inputData;
}

void Port_getPinFromUint32(uint32_t value, Port_SomePin_t* pinToWrite){

	pinToWrite->portPin = value & 0xFFFF;
	pinToWrite->portName = 0xF &(value >> 16);
	pinToWrite->direction = getBit(value, 23);
	pinToWrite->pullUp = getBit(value, 25);
}

uint32_t Port_getUint32FromPin(Port_SomePin_t* pin){

	uint32_t temp = 0;

	temp = pin->portPin & 0xFFFF;
	temp |= (((uint32_t) (pin->portName & 0xF)) << 16);
	temp |= (((uint32_t) (pin->direction & 0x1)) << 23);
	temp |= (((uint32_t) (pin->pullUp & 0x1)) << 25);

	return temp;
}
