#ifndef MTR_I2C_H
#define MTR_I2C_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "mtr_data/mtr_data.h"

#include "config.h"

//Master
#define SEND_START							0x08
#define SEND_REPEAT							0x10
#define LOST_PRIORITET						0x38
//Mster-передатчик
#define SEND_WRITE_ADDRESS_RECIEVE_ACK		0x18
#define SEND_WRITE_ADDRESS_RECIEVE_NACK		0x20
#define SEND_DATA_RECIEVE_ACK				0x28
#define SEND_DATA_RECIEVE_NACK				0x30
//Master-приемник
#define SEND_READ_ADDRESS_RECIEVE_ACK		0x40
#define SEND_READ_ADDRESS_RECIEVE_NACK		0x48
#define RECIEVE_DATA_RECIEVE_ACK			0x50
#define RECEIVE_DATA_RECIEVE_NACK			0x58
//Slave-приемник
#define TRANSMISSION_BEGIN					0x60
#define LOST_ARBTRATION_TRANSMISSION_BEGIN	0x68
#define GENERAL_CALL						0x70
#define LOST_ARBTRATION_GENERAL_CALL		0x78
#define RECIEVE_DATA						0x80
#define RECIEVE_DATA_GENERAL_CALL			0x90
#define RECIEVE_LAST_DATA					0x88
#define RECIEVE_LAST_DATA_GENERAL_CALL		0x98
#define STOP_OR_REPEAT						0xA0
//Slave-передатчик
#define SENDING_BEGIN						0xA8
#define LOST_ARBTRATION_SENDING_BEGIN		0xB0
#define SEND_DATA							0xB8
#define END_OF_SENDING						0xC0
#define END_OF_DATA							0xC8

#define I2C_STATUS_CODE (TWSR & 0xF8)

#define I2C_ACK		(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(1<<TWEN)|(1<<TWIE)
#define I2C_NACK	(1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(1<<TWEN)|(1<<TWIE)
#define I2C_STOP	(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|(1<<TWEN)|(1<<TWIE)

#define TRANSACTION_TYPE_SLAVE_TRANSMITTER	0b00
#define TRANSACTION_TYPE_SLAVE_RECEIVER		0b01
#define TRANSACTION_TYPE_MASTER_TRANSMITTER	0b10
#define TRANSACTION_TYPE_MASTER_RECEIVER	0b11

void I2C_init(uint8_t slaveAddress);
void I2C_initMaster();
uint8_t I2C_positionReg();
void I2C_writeReg(uint8_t *data, uint8_t number);
void I2C_sendMessage(uint8_t* data, uint8_t size);
void I2C_readMessage(uint8_t addressSlave, uint8_t bytesAmount);
bool I2C_wasTransaction();
bool I2C_endTransaction();
void I2C_setEndTransaction(bool flag);
void I2C_setWasTransaction(bool flag);
void I2C_turnOn();
void I2C_turnOff();
uint8_t I2C_getLastTransactionType();
bool I2C_isStartTransaction();
void I2C_setStartTransaction(bool flag);

#endif