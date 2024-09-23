#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#include "../../config.h"
#include "mtr_port/mtr_port.h"
#include "mtr_service/mtr_service.h"

#define UART_TIMER_NUMS_TOTAL_AMOUNT 6

#define uartTimerRepeatesNotCounting 0
#define uartTimerRepeatesSwitchToReadAfterTransmit 1
#define uartTimerRepeatesProccessingRecievedMessage 2
#define uartTimerRepeatesUserTimeout1 3
#define uartTimerRepeatesUserTimeout2 4
#define uartTimerRepeatesStartTransmit 5

typedef enum {
    Speed2400   = 2400UL,
    Speed4800   = 4800UL,
    Speed9600   = 9600UL,
    Speed19200  = 19200UL,
    Speed38400  = 38400UL,
    Speed57600  = 57600UL,
    Speed115200 = 115200UL
} Uart_Speed;

typedef enum {
    NoParity,
    Even,
    Odd
} Uart_Parity;

typedef enum {
    OneStop,
    TwoStop
} Uart_StopBit;

typedef enum {
    Uart0 = 10,
    Uart1 = 11
} Uart_Name;

typedef enum {
	RS485 = 0,
	UART = 1
}Uart_Type;

typedef enum {
	NONE,  	// none symbol
	LFCR,     	// line feed and carriage return (0x0A, 0x0D)
	CRLF,     	// carriage return and line feed (0x0D,0x0A)
	LF,        	// line feed (0x0A)
	CR,       	// carriage return (0x0D)
	COLON,		// ":" 	0x3A
	SPACE,	    	// " " 	0x20
	DOT			// "." 	0x2E
} Uart_Lastbyte;

typedef struct {

	uint8_t buffer[UART_BUFFER_SIZE]; 	// RX buffer
	uint16_t writePos;				    // tx pointer
	uint16_t readPos;            	    // rx pointer
	uint16_t amount;

}Uart_BufferStruct_t;


typedef struct {
	Uart_Speed baud;      	        // Speed
	Uart_Parity parity;
	Uart_StopBit stopBit;
	Uart_Type type;					//Type RS
	Port_Direction_t direction;
	Port_Name_t enablePort;
	Port_Pin_t enablePin;
	bool enableEnablePinInvesion;
	uint16_t numsOfRepeats[UART_TIMER_NUMS_TOTAL_AMOUNT];
	uint16_t timerCounterDown;
	uint8_t needTimerNum;
	Uart_BufferStruct_t rxBuffer;
	Uart_BufferStruct_t txBuffer;
	bool inited;
	void (*callback_userRecieve)(Uart_Name);
	void (*callback_userSwitchToIn)(Uart_Name);
	void (*callback_userRxInterrupt)(Uart_Name);
	void (*callback_userTxInterrupt)(Uart_Name);
	void (*callback_afterByteRecieved)(Uart_Name);
#ifdef UART_DEBUG
	uint8_t lastSendedByte;
	uint32_t totalBytesRecieved;
	uint32_t totalBytesSended;
#endif

}UART_t;

void Uart_init(Uart_Name uart, Uart_Speed speed, Uart_Parity parity, Uart_StopBit stopBit, void (*userRecieveFunc)(Uart_Name), void (*switchToInFunc)(Uart_Name));
void Uart_deinit(Uart_Name uart);
UART_t* Uart_getMainStruct(Uart_Name uart);
uint8_t Uart_readByte(Uart_Name uart);
void Uart_sendNextByte(Uart_Name uart);
bool Uart_initTransmite(Uart_Name uart);
uint32_t Uart_getTimerBaudSpeed();
void Uart_setRxInterruptFunction(Uart_Name uart, void (*rxFunc)(Uart_Name));
void Uart_setTxInterruptFunction(Uart_Name uart, void (*txFunc)(Uart_Name));
void Uart_setAfterByteRecieveFunction(Uart_Name uart, void (*someFunc)(Uart_Name));

void Uart_addByte(Uart_Name uart, uint8_t byte);
void Uart_addHalfWord(Uart_Name uart, uint16_t halfWord);
void Uart_addWord(Uart_Name uart, uint32_t word);
void Uart_addString(Uart_Name uart, char* pointer, Uart_Lastbyte end_cmd);
void Uart_addArray(Uart_Name uart, uint8_t *data, uint16_t cnt);
void Uart_addCrc16(Uart_Name uart, uint16_t(*crc16Func)(uint8_t*, uint32_t));

void Uart_addByteToRxBuffer(Uart_Name uart, uint8_t byte);
void Uart_bufferIncrementer(Uart_BufferStruct_t* buffer);
void Uart_resetRxPointer(Uart_Name uart);
void Uart_resetTxPointer(Uart_Name uart);
bool Uart_isDataToSend(Uart_Name uart);
void Uart_setDirection(Uart_Name uart, Port_Direction_t direction);
Port_Direction_t Uart_getDirection(Uart_Name uart);
void Uart_setEnablePin(Uart_Name uart, Port_Name_t port, Port_Pin_t pin, bool inversion);

void Uart_setNumOfTimeoutSize(Uart_Name uart, uint8_t timeoutNum, uint16_t timeoutSize);

//timer
void Uart_timerInit(uint32_t baudSpeed);
void Uart_timerStop();
void Uart_timerStart();
void Uart_setNeedTimerNum(Uart_Name uart, uint16_t timerNum);

#endif