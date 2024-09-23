#include "mtr_i2c.h"

//Буфер приема данных RX
uint8_t dataRX[DATA_SIZE + 1];
uint8_t dataTX[TX_DATA_SIZE];
uint8_t recievedDataCounter;
uint8_t writeFlag;
uint8_t readPosition;
uint8_t writePosition;
uint8_t sizeTX;
bool wasTransaction = false;
bool endTransaction = false;
bool startTransaction = false;
uint8_t lastAddress;
uint8_t masterRecieverBuffer[I2C_ALL_SLAVES_REGISTERS_MAP];
uint8_t readAmount;
uint8_t lastTransactionType;

ISR(TWI_vect){
	wasTransaction = true;
	startTransaction = true;
	endTransaction = false;
	switch(I2C_STATUS_CODE){
		//Master
		case SEND_START:
		case SEND_REPEAT:{
			writePosition = 0;
			TWDR = dataTX[writePosition++];
			TWCR = I2C_ACK;
			break;
		}
		case LOST_PRIORITET:{
			writePosition = 0;
			endTransaction = true;
			TWCR = I2C_ACK;
			break;
		}
		
		//Master-передатчик
		case SEND_WRITE_ADDRESS_RECIEVE_NACK:{
			lastTransactionType = TRANSACTION_TYPE_MASTER_TRANSMITTER;
			endTransaction = true;
			TWCR = I2C_STOP; //Стоп
			break;
		}
		case SEND_DATA_RECIEVE_NACK:{
			TWCR = I2C_STOP; //Стоп
			Data_setReadIndexByte(dataTX[0], dataTX[1]);
			break;
		}
		case SEND_WRITE_ADDRESS_RECIEVE_ACK:
		case SEND_DATA_RECIEVE_ACK:{
			lastTransactionType = TRANSACTION_TYPE_MASTER_TRANSMITTER;
			if (writePosition < sizeTX){
				TWDR = dataTX[writePosition++];
				TWCR = I2C_ACK; //Следующий байт
			} else {
				TWCR = I2C_STOP; //Стоп
				Data_setReadIndexByte(dataTX[0], dataTX[1]);
			}
			break;
		}
		
		//Master-приемник
		case SEND_READ_ADDRESS_RECIEVE_ACK:{
			lastTransactionType = TRANSACTION_TYPE_MASTER_RECEIVER;
			recievedDataCounter = 0;
			if (readAmount == 1){
				TWCR = I2C_NACK;
			} else {
				TWCR = I2C_ACK;
			}
			break;
		}
		case SEND_READ_ADDRESS_RECIEVE_NACK:{
			lastTransactionType = TRANSACTION_TYPE_MASTER_RECEIVER;
			TWCR = I2C_STOP;
			endTransaction = true;
			break;
		}
		case RECIEVE_DATA_RECIEVE_ACK:{
			masterRecieverBuffer[recievedDataCounter++] = TWDR;
			if (recievedDataCounter < (readAmount - 1)){
				TWCR = I2C_ACK;
			} else {
				TWCR = I2C_NACK;	
			}
			break;
		}
		case RECEIVE_DATA_RECIEVE_NACK:{
			masterRecieverBuffer[recievedDataCounter++] = TWDR;
			TWCR = I2C_STOP;
			Data_fulfillRegistersMap(dataTX[0], masterRecieverBuffer, recievedDataCounter);
			break;
		}
		
		//Slave-приемник
		case TRANSMISSION_BEGIN:
		case LOST_ARBTRATION_TRANSMISSION_BEGIN:
		case GENERAL_CALL:
		case LOST_ARBTRATION_GENERAL_CALL:
		{
			lastTransactionType = TRANSACTION_TYPE_SLAVE_RECEIVER;
			recievedDataCounter = 0;
			TWCR = I2C_ACK;	//ACK
			break;
		}
		
		case RECIEVE_DATA:
		case RECIEVE_DATA_GENERAL_CALL:
		{
			if (recievedDataCounter < DATA_SIZE + 1)
			{
				dataRX[recievedDataCounter++] = TWDR;
				TWCR = I2C_ACK;	//ACK
			} else
			{
				TWCR = I2C_NACK;	//NACK
				TWCR = I2C_ACK;	//ACK
			}
			break;
		}
		
		case RECIEVE_LAST_DATA:
		case RECIEVE_LAST_DATA_GENERAL_CALL:
		{
			if (recievedDataCounter < DATA_SIZE + 1)
			{
				dataRX[recievedDataCounter++] = TWDR;
				TWCR = I2C_ACK;	//ACK
				I2C_writeReg(dataRX, recievedDataCounter);
				writeFlag = 1;
			} else
			{
				TWCR = I2C_NACK;	//NACK
				TWCR = I2C_ACK;	//ACK
			}
			
			break;
		}
		
		case STOP_OR_REPEAT:
		{
			TWCR = I2C_ACK;	//ACK
			if (writeFlag == 0)
			{
				I2C_writeReg(dataRX, recievedDataCounter);
			}
			recievedDataCounter = 0;
			writeFlag = 0;
			break;
		}
		
		//Slave-передатчик
		case SENDING_BEGIN:
		case LOST_ARBTRATION_SENDING_BEGIN:
		{
			lastTransactionType = TRANSACTION_TYPE_SLAVE_TRANSMITTER;
			readPosition = I2C_positionReg();
			if (readPosition < DATA_SIZE)
			{
				TWDR = Data_getReg(readPosition++);
				TWCR = I2C_ACK;	//ACK
			} else
			{
				TWDR = 0;
				TWCR = I2C_ACK;	//ACK
			}
			break;
		}
		
		case SEND_DATA:
		{
			if (readPosition < DATA_SIZE)
			{
				TWDR = Data_getReg(readPosition++);
				TWCR = I2C_ACK;	//ACK
			} else
			{
				TWDR = 0;
				TWCR = I2C_ACK;	//ACK
			}
			break;
		}
		
		case END_OF_SENDING:
		case END_OF_DATA:
		{
			endTransaction = true;
			TWCR = I2C_ACK;	//ACK
			break;
		}
		
		default:
		{
			endTransaction = true;
			TWCR = I2C_STOP; //Стоп
			break;
		}
	}
}

void I2C_init(uint8_t slaveAddress){
	TWAR = (slaveAddress << 1);//адрес slave
	TWSR = 0;
	TWBR = 72;
	TWCR = (0<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC)|(1<<TWEN)|(1<<TWIE); //инициализация
}

void I2C_initMaster(){
	TWSR = 0;
	TWBR = 72;
	TWCR = (0<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC)|(1<<TWEN)|(1<<TWIE); //инициализация

}

void I2C_sendMessage(uint8_t* data, uint8_t size){
	if (size > 0){
		sizeTX = size;
		
		for (uint8_t index = 0; index < size; index++){
			dataTX[index] = data[index];
		}
		
		dataTX[0] = dataTX[0] << 1;
		
		endTransaction = false;
		
		TWCR &= ~(1 << TWSTO);
		TWCR |= ((1 << TWSTA) | (1 << TWINT));
	}
}

void I2C_readMessage(uint8_t addressSlave, uint8_t bytesAmount){
	dataTX[0] = addressSlave << 1;
	dataTX[0] |= ZERO_BIT;
	
	readAmount = bytesAmount;
	
	endTransaction = false;
	
	TWCR &= ~(1 << TWSTO);
	TWCR |= ((1 << TWSTA) | (1 << TWINT));
}

void I2C_writeReg(uint8_t *data, uint8_t number)
{
	lastAddress = data[0];
	if (lastAddress <= DATA_SIZE - 1)
	{
		for (int8_t index = 1 + lastAddress; (index < number + lastAddress) & (index <= DATA_SIZE); index++){
			if (index > READ_REGISTERS_SIZE)
			{
				Data_setReg(index - 1, data[index - lastAddress]);
			}
		}
	}
	endTransaction = true;
}

uint8_t I2C_positionReg(){
	return lastAddress;
}

bool I2C_wasTransaction(){
	return wasTransaction;
}

bool I2C_endTransaction(){
	return endTransaction;
}
void I2C_setEndTransaction(bool flag){
	endTransaction = flag;
}

void I2C_setWasTransaction(bool flag){
	wasTransaction = flag;
}

void I2C_turnOn(){
	TWCR |= (1 << TWEN);
}

void I2C_turnOff(){
	TWCR &= ~(1 << TWEN);
}

uint8_t I2C_getLastTransactionType(){
	return lastTransactionType;
}

bool I2C_isStartTransaction(){
	return startTransaction;
}
void I2C_setStartTransaction(bool flag){
	startTransaction = flag;
}