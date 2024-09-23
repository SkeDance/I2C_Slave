#ifndef MTR_PORT_H_
#define MTR_PORT_H_

#include <stdbool.h>
#include <stdint.h>
#include "mtr_service/mtr_service.h"

typedef enum{

    PortA,
    PortB,
    PortC,
    PortD,
    PortE,
    PortF,
	PortG

}Port_Name_t;

/*
 * Port modes
 */

typedef enum{

	pin0 = 0x0001U,  /*!< Pin 0 selected */
	pin1 = 0x0002U,  /*!< Pin 1 selected */
	pin2 = 0x0004U,  /*!< Pin 2 selected */
	pin3 = 0x0008U,  /*!< Pin 3 selected */
	pin4 = 0x0010U,  /*!< Pin 4 selected */
	pin5 = 0x0020U,  /*!< Pin 5 selected */
	pin6 = 0x0040U,  /*!< Pin 6 selected */
	pin7 = 0x0080U,  /*!< Pin 7 selected */
	pin8 = 0x0100U,  /*!< Pin 8 selected */
	pin9 = 0x0200U,  /*!< Pin 9 selected */
	pin10 = 0x0400U,  /*!< Pin 10 selected */
	pin11 = 0x0800U,  /*!< Pin 11 selected */
	pin12 = 0x1000U,  /*!< Pin 12 selected */
	pin13 = 0x2000U,  /*!< Pin 13 selected */
	pin14 = 0x4000U,  /*!< Pin 14 selected */
	pin15 = 0x8000U,  /*!< Pin 15 selected */
	pinAll = 0xFFFFU  /*!< All pins selected */

}Port_Pin_t;


typedef struct
{
	uint8_t* outputData;
	uint8_t* direction;
	uint8_t* inputData;
	
}Port_t;

typedef enum
{
  IN            = 0x0,
  OUT           = 0x1
}Port_Direction_t;


typedef struct{

    Port_Name_t portName;
    Port_Pin_t portPin;
    Port_Direction_t direction;
    bool pullUp;

}Port_SomePin_t;

typedef struct{

	Port_Name_t portName;
	Port_Pin_t portPin;
	Port_Direction_t direction;
	bool* connectedWith;

}Port_ConnectedPin_t;

typedef struct{
	Port_ConnectedPin_t pins[CONFIG_PORT_CONNECTED_PIN_AMOUNT];
	uint8_t currentPinAmount;
}Port_ConnectedPins_t;


void Port_deInit(Port_Name_t port);

void Port_init(Port_Name_t port, Port_Pin_t pin, Port_Direction_t direction, bool pullUp);

void Port_setPin(Port_Name_t port, Port_Pin_t pin);

void Port_resetPin(Port_Name_t port, Port_Pin_t pin);

bool Port_getPin(Port_Name_t port, Port_Pin_t pin);

uint8_t Port_getPort(Port_Name_t port);

void Port_writePin(Port_Name_t port, Port_Pin_t pin, bool status);

void Port_writePort(Port_Name_t port, uint8_t data);

void Port_connectPin(Port_Name_t port, Port_Pin_t pin, Port_Direction_t direction, bool* value, bool pullUp);

void Port_updatePins();

uint8_t* Port_getDirectionRegister(Port_Name_t port);

uint8_t* Port_getOutputDataRegister(Port_Name_t port);

uint8_t* Port_getInputDataRegister(Port_Name_t port);

void Port_getPinFromUint32(uint32_t value, Port_SomePin_t* pinToWrite);

uint32_t Port_getUint32FromPin(Port_SomePin_t* pin);

#endif /* MTR_PORT_H_ */
