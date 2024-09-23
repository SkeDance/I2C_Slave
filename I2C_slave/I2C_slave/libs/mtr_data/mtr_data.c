#include "mtr_data.h"

uint8_t fullData[DATA_SIZE];
slaveByte slavesBytes[I2C_ALL_SLAVES_REGISTERS_MAP];
uint16_t slavesBytesAmount;

uint8_t masks[8] = {
	ZERO_BIT,
	FIRST_BIT,
	SECOND_BIT,
	THIRD_BIT,
	FOURTH_BIT,
	FIFTH_BIT,
	SIXTH_BIT,
	SEVENTH_BIT
};

void Data_init(uint16_t crc){
	Data_setReg(REG_CRC_A, crc & 0xff);
	Data_setReg(REG_CRC_B, crc >> 8);
}

uint8_t Data_getReg(uint8_t position){
	return fullData[position];
}

void Data_setReg(uint8_t index, uint8_t registerData){
	fullData[index] = registerData;
}

uint8_t Data_getRegBit(uint8_t index, uint8_t bit){
	switch (bit){
		case 0: {
			return (fullData[index] & ZERO_BIT);
		}
		case 1: {
			return (fullData[index] & FIRST_BIT);
		}
		case 2: {
			return (fullData[index] & SECOND_BIT);
		}
		case 3: {
			return (fullData[index] & THIRD_BIT);
		}
		case 4: {
			return (fullData[index] & FOURTH_BIT);
		}
		case 5:{
			return (fullData[index] & FIFTH_BIT);
		}
		case 6:{
			return (fullData[index] & SIXTH_BIT);
		}
		case 7:{
			return (fullData[index] & SEVENTH_BIT);
		}
		default:
			break;
	}
}

void Data_setRegBit(uint8_t index, uint8_t bit){
	switch(bit){
		case 0: {
			fullData[index] |= ZERO_BIT;
			break;
		}
		case 1: {
			fullData[index] |= FIRST_BIT;
			break;
		}
		case 2: {
			fullData[index] |= SECOND_BIT;
			break;
		}
		case 3: {
			fullData[index] |= THIRD_BIT;
			break;
		}
		case 4: {
			fullData[index] |= FOURTH_BIT;
			break;
		}
		case 5: {
			fullData[index] |= FIFTH_BIT;
			break;
		}
		case 6: {
			fullData[index] |= SIXTH_BIT;
			break;
		}
		case 7: {
			fullData[index] |= SEVENTH_BIT;
			break;
		}
		default:
		break;
	}
}

void Data_resetRegBit(uint8_t index, uint8_t bit){
	switch(bit){
		case 0: {
			fullData[index] &= ~ZERO_BIT;
			break;
		}
		case 1: {
			fullData[index] &= ~FIRST_BIT;
			break;
		}
		case 2: {
			fullData[index] &= ~SECOND_BIT;
			break;
		}
		case 3: {
			fullData[index] &= ~THIRD_BIT;
			break;
		}
		case 4: {
			fullData[index] &= ~FOURTH_BIT;
			break;
		}
		case 5: {
			fullData[index] &= ~FIFTH_BIT;
			break;
		}
		case 6: {
			fullData[index] &= ~SIXTH_BIT;
			break;
		}
		case 7: {
			fullData[index] &= ~SEVENTH_BIT;
			break;
		}
		default:
		break;
	}
}

void Data_setRegBits(uint8_t regIndex, uint8_t data, uint8_t firstBitIndex, uint8_t bitsAmount){
	for (uint8_t index = 0; index < bitsAmount; index++){
		if (data & masks[index]){
			fullData[regIndex] |= (1 << (firstBitIndex + index));
			} else {
			fullData[regIndex] &= ~(1 << (firstBitIndex + index));
		}
	}
}

void Data_setReadIndexByte(uint8_t addressSlave, uint8_t byteIndex){
	bool readIndexFound = false;
	
	addressSlave = addressSlave >> 1;
	
	for (uint8_t index = 0; index < slavesBytesAmount; index++){
		if (slavesBytes[index].addressSlaveDevice == addressSlave){
			if (slavesBytes[index].indexByte == byteIndex){
				slavesBytes[index].lastWrittenByte = true;
				readIndexFound = true;
				} else {
				slavesBytes[index].lastWrittenByte = false;
			}
		}
	}
	
	if (!readIndexFound){
		slavesBytes[slavesBytesAmount].addressSlaveDevice = addressSlave;
		slavesBytes[slavesBytesAmount].indexByte = byteIndex;
		slavesBytes[slavesBytesAmount].lastWrittenByte = true;
		slavesBytes[slavesBytesAmount].initByte = false;
		slavesBytesAmount++;
	}
	
	I2C_setEndTransaction(true);
}

void Data_fulfillRegistersMap(uint8_t addressSlave, uint8_t* masterRecieverBuffer, uint8_t readAmount){
	uint8_t byteIndex = 0;
	bool byteFound = false;

	addressSlave = (addressSlave >> 1);

	for (uint16_t registersMapIndex = 0; registersMapIndex < slavesBytesAmount; registersMapIndex++){
		if ((slavesBytes[registersMapIndex].addressSlaveDevice == addressSlave) & (slavesBytes[registersMapIndex].lastWrittenByte == true)){
			byteIndex = slavesBytes[registersMapIndex].indexByte;
			break;
		}
	}
	
	for (uint8_t bufferIndex = 0; bufferIndex < readAmount; bufferIndex++){
		byteFound = false;
		for (uint16_t registersMapIndex = 0; registersMapIndex < slavesBytesAmount; registersMapIndex++){
			if ((slavesBytes[registersMapIndex].addressSlaveDevice == addressSlave) & (slavesBytes[registersMapIndex].indexByte == (byteIndex + bufferIndex))){
				slavesBytes[registersMapIndex].valueByte = masterRecieverBuffer[bufferIndex];
				slavesBytes[registersMapIndex].initByte = true;
				byteFound = true;
			}
		}
		
		if (!byteFound){
			slavesBytes[slavesBytesAmount].addressSlaveDevice = addressSlave;
			slavesBytes[slavesBytesAmount].indexByte = byteIndex + bufferIndex;
			slavesBytes[slavesBytesAmount].valueByte = masterRecieverBuffer[bufferIndex];
			slavesBytes[slavesBytesAmount].lastWrittenByte = false;
			slavesBytes[slavesBytesAmount].initByte = true;
			slavesBytesAmount++;
		}
	}
	
	I2C_setEndTransaction(true);
}

uint8_t Data_getSlaveRigister(uint8_t address, uint8_t byteIndex){
	for (uint8_t index = 0; index < slavesBytesAmount; index++){
		if ((slavesBytes[index].addressSlaveDevice == address) & (slavesBytes[index].indexByte == byteIndex)){
			return slavesBytes[index].valueByte;
		}
	}
	return 0;
}

void Data_addSlaveRegister(uint8_t address, uint8_t byteIndex, uint8_t value){
	bool present = false;
	
	for (uint8_t index = 0; index < slavesBytesAmount; index++){
		if ((slavesBytes[index].addressSlaveDevice == address) & (slavesBytes[index].indexByte == byteIndex)){
			slavesBytes[index].valueByte = value;
			present = true;
			break;
		}
	}
	
	if (!present){
		slavesBytes[slavesBytesAmount].addressSlaveDevice = address;
		slavesBytes[slavesBytesAmount].indexByte = byteIndex;
		slavesBytes[slavesBytesAmount].valueByte = value;
		slavesBytes[slavesBytesAmount].lastWrittenByte = false;
		slavesBytes[slavesBytesAmount].initByte = false;
		slavesBytesAmount++;
	}
}

uint8_t* Data_getAddressRegister(uint8_t index){
	return &fullData[index];
}