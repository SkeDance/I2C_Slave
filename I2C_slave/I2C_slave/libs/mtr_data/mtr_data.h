#ifndef MTR_DATA_H
#define MTR_DATA_H

#include "avr/io.h"

#include "config.h"
#include "device.h"
#include "mtr_i2c/mtr_i2c.h"

typedef struct
{
	uint8_t addressSlaveDevice;
	uint8_t indexByte;
	uint8_t valueByte;
	bool lastWrittenByte;
	bool initByte;
} slaveByte;

void Data_init(uint16_t crc);

uint8_t Data_getReg(uint8_t position);
void Data_setReg(uint8_t index, uint8_t registerData);

uint8_t Data_getRegBit(uint8_t index, uint8_t bit);
void Data_setRegBit(uint8_t index, uint8_t bit);
void Data_resetRegBit(uint8_t index, uint8_t bit);
void Data_setRegBits(uint8_t regIndex, uint8_t data, uint8_t firstBitIndex, uint8_t bitsAmount);
void Data_setReadIndexByte(uint8_t addressSlave, uint8_t byteIndex);
void Data_fulfillRegistersMap(uint8_t addressSlave, uint8_t* masterRecieverBuffer, uint8_t readAmount);
uint8_t Data_getSlaveRigister(uint8_t address, uint8_t byteIndex);
void Data_addSlaveRegister(uint8_t address, uint8_t byteIndex, uint8_t value);
uint8_t* Data_getAddressRegister(uint8_t index);

#endif
