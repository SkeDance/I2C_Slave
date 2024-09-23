#include "mtr_uart.h"

uint32_t timerBaudSpeed = 0;
UART_t UartStructs[UART_AMOUNT];

extern uint8_t toBootloaderResetCommand[39];

UART_t* Uart_getMainStruct(Uart_Name uart){

	switch (uart)
	{
		case Uart0 : return &UartStructs[0];
		case Uart1 : return &UartStructs[1];
		default: return &UartStructs[0];
	}
}


uint8_t Uart_readByte(Uart_Name uart){
	
	switch (uart){
		case Uart0 : return UDR0;
		case Uart1 : return UDR1;
		default: return 0;
	}
}


void Uart_setRxInterruptFunction(Uart_Name uart, void (*rxFunc)(Uart_Name)){
	
	UART_t* uartStr = &UartStructs[uart-Uart0];
	uartStr->callback_userRxInterrupt = rxFunc;
}


void Uart_setTxInterruptFunction(Uart_Name uart, void (*txFunc)(Uart_Name)){
	
	UART_t* uartStr = &UartStructs[uart-Uart0];
	uartStr->callback_userTxInterrupt = txFunc;
}

void Uart_setAfterByteRecieveFunction(Uart_Name uart, void (*someFunc)(Uart_Name)){
	
	UART_t* uartStr = &UartStructs[uart-Uart0];
	uartStr->callback_afterByteRecieved = someFunc;
}

void Uart_setNumOfTimeoutSize(Uart_Name uart, uint8_t timeoutNum, uint16_t timeoutSize){
	
	UART_t* uartStr = &UartStructs[uart-Uart0];
	
	if (timeoutNum < UART_TIMER_NUMS_TOTAL_AMOUNT){
		uartStr->numsOfRepeats[timeoutNum] = timeoutSize;
	}
}

uint32_t Uart_getTimerBaudSpeed(){
	
	return timerBaudSpeed;
}


void Uart_sendNextByte(Uart_Name uart){
	
	UART_t* uartStr = &UartStructs[uart-Uart0];

	if (uartStr->txBuffer.amount){
		
		uint16_t* read_pos = &(uartStr->txBuffer.readPos);	
		uint8_t byte = uartStr->txBuffer.buffer[(*read_pos)];

		switch (uart)
		{
			case Uart0:				
				UDR0 = byte;	
			break;
			
			case Uart1:
				UDR1 = byte;
			break;
		}

		*read_pos = (*read_pos >= (UART_BUFFER_SIZE - 1)) ? 0 : (*read_pos + 1);
		
		uartStr->txBuffer.amount -= 1;
#ifdef UART_DEBUG
		uartStr->totalBytesSended++;
		uartStr->lastSendedByte = byte;	
#endif
	}
}


bool Uart_initTransmite(Uart_Name uart){

	UART_t* uartStr = &UartStructs[uart-Uart0];
	
	if (uartStr->direction == IN){
		
		Uart_setDirection(uart, OUT);
		
		if (uartStr->type == RS485){
			Uart_setNeedTimerNum(uart, uartTimerRepeatesStartTransmit);
		}
		else 
		{
			Uart_sendNextByte(uart);
		}
		return true;
	}
	else
	{
		return false;
	}

}

void Uart_addByteToRxBuffer(Uart_Name uart, uint8_t byte){

	UART_t* uartStruct = &UartStructs[uart-Uart0];
	uint16_t* writePos = &uartStruct->rxBuffer.writePos;
	
#ifdef UART_DEBUG
	uartStruct->totalBytesRecieved += 1;
#endif

	if (*writePos < UART_BUFFER_SIZE){
		uartStruct->rxBuffer.buffer[*writePos] = byte;
		Uart_bufferIncrementer(&uartStruct->rxBuffer);
	}		
	
#ifdef UART_ENABLE_MAGIC_CODE
	if (uartStruct->rxBuffer.amount == sizeof(toBootloaderResetCommand)){
		if (isEqualData((uint32_t) sizeof(toBootloaderResetCommand), uartStruct->rxBuffer.buffer, toBootloaderResetCommand)){
			
			eeprom_write_dword((uint32_t*) (PARAMETER_BOOTSELECT * 4), (uint32_t) uart);
			eeprom_busy_wait();
			
			Port_SomePin_t enPin;
			enPin.direction = OUT;
			enPin.portName = uartStruct->enablePort;
			enPin.portPin = uartStruct->enablePin;	
			enPin.pullUp = false;
			
			eeprom_write_dword((uint32_t*) (PARAMETER_CONNECTION_ENPINPORT * 4), Port_getUint32FromPin(&enPin));
			eeprom_busy_wait();
									
			uint32_t connectionParameters = ((uint32_t)(uartStruct->baud)) | ((uint32_t) (uartStruct->parity)) << 24 | ((uint32_t)(uartStruct->type)) << 27 | ((uint32_t)(uartStruct->enableEnablePinInvesion)) << 28;
			
			eeprom_write_dword((uint32_t*) (PARAMETER_CONNECTION_SPEED_AND_PARITY * 4), connectionParameters);
			eeprom_busy_wait();

			goToBootloader();
		}
	}

#endif
	
}

void Uart_bufferIncrementer(Uart_BufferStruct_t* buffer){

	if (buffer->amount < UART_BUFFER_SIZE){

		buffer->writePos += 1;
		buffer->amount += 1;

		if (buffer->writePos == UART_BUFFER_SIZE){
			buffer->writePos = 0;
		}
	}
	else 
	{
		buffer->writePos += 1;
		buffer->readPos += 1;

		if (buffer->writePos == UART_BUFFER_SIZE){
			buffer->writePos = 0;
		}

		if (buffer->readPos == UART_BUFFER_SIZE){
			buffer->readPos = 0;
		}
	}
}

bool Uart_isDataToSend(Uart_Name uart){
	
	UART_t* uartStr = &UartStructs[uart-Uart0];
	
	return uartStr->txBuffer.amount != 0;
}


void Uart_init(Uart_Name uart, Uart_Speed speed, Uart_Parity parity, Uart_StopBit stopBit, void (*userRecieveFunc)(Uart_Name), void (*switchToInFunc)(Uart_Name))
{
    UART_t* uartStr = &UartStructs[uart-Uart0];
		
	switch (speed){

		case 2400UL:
		case 4800UL:
		case 9600UL:
		case 19200UL:
		case 38400UL:
		case 57600UL:
		case 115200UL:
		break;		
		
		default:
		speed = Speed19200;
		break;
	}
	
	uint16_t UBRR = ((CPU_FREQUENCY/speed) /8) - 1;

	uartStr->baud = speed;
	uartStr->stopBit = stopBit;
	uartStr->parity = parity;
	
	if (uartStr->type == RS485){
		
		Port_init(uartStr->enablePort, uartStr->enablePin, OUT, false);
		Uart_setDirection(uart, IN);
		
	}
	
	switch (uart){
		case Uart0:
		{
			UCSR0A = 1 << U2X0;

			UCSR0B = 0;
			UCSR0B |= (1 << TXEN0)|(1 << RXEN0);
			UCSR0B |= (1 << RXCIE0)|(1 << TXCIE0);

			UBRR0L = (uint8_t)(UBRR);
			UBRR0H = (uint8_t)(UBRR >> 8);

			switch (parity)
			{
				case NoParity:
					UCSR0C = (0 << UPM0);
				break;
			
				case Even:
					UCSR0C = (2 << UPM0);
				break;
			
				case Odd:
					UCSR0C = (3 << UPM0);
				break;
			}

			switch (stopBit)
			{
				case OneStop:
					UCSR0C |= (0 << USBS0);
				break;
	
				case TwoStop:
					UCSR0C |= (1 << USBS0);
				break;
			}
			UCSR0C |= (3 << UCSZ00);
		}
		break;
		
		case  Uart1:
		{
			UCSR1A = 1 << U2X1;

			UCSR1B = 0;
			UCSR1B |= (1 << TXEN1)|(1 << RXEN1);
			UCSR1B |= (1 << RXCIE1)|(1 << TXCIE1);

			UBRR1L = (uint8_t) (UBRR);
			UBRR1H = (uint8_t) (UBRR >> 8);

			switch (parity)
			{
				case NoParity:
				UCSR1C = (0 << UPM0);
				break;
			    
				case Even:
				UCSR1C = (2 << UPM0);
				break;
			    
				case Odd:
				UCSR1C = (3 << UPM0);
				break;
			}

			switch (stopBit)
			{
				case OneStop:
				UCSR1C |= (0 << USBS1);
				break;
			    
				case TwoStop:
				UCSR1C |= (1 << USBS1);
				break;
			}
			UCSR1C |= (3 << UCSZ10);
		}
		
		break;
	}
	
	uartStr->inited = true;
	
	if (!uartStr->callback_userSwitchToIn && switchToInFunc){
		uartStr->callback_userSwitchToIn = switchToInFunc;
	}
	
	if (!uartStr->callback_userRecieve && userRecieveFunc){
		uartStr->callback_userRecieve = userRecieveFunc;
	}
	
	//find max uart baud speed
	uint32_t baudSpeedToInit = 0;
	
	for (uint8_t i = 0; i < UART_AMOUNT; i++){
		baudSpeedToInit = (baudSpeedToInit < UartStructs[i].baud) ? UartStructs[i].baud : baudSpeedToInit;
	}

    for (uint8_t i = 0; i < UART_AMOUNT; i++){
	    if (UartStructs[i].inited){
			UartStructs[i].numsOfRepeats[uartTimerRepeatesSwitchToReadAfterTransmit] = (baudSpeedToInit / UartStructs[i].baud) * 2;
			UartStructs[i].numsOfRepeats[uartTimerRepeatesProccessingRecievedMessage] = (baudSpeedToInit / UartStructs[i].baud) * UART_PROCESSING_RECIEVED_MESSAGE_IN_BYTES;
			UartStructs[i].numsOfRepeats[uartTimerRepeatesStartTransmit] = (baudSpeedToInit / UartStructs[i].baud);
			UartStructs[i].numsOfRepeats[uartTimerRepeatesUserTimeout1] = ( (float) baudSpeedToInit / UartStructs[i].baud) * UartStructs[i].numsOfRepeats[uartTimerRepeatesUserTimeout1];
	    }
    }
	
	timerBaudSpeed = baudSpeedToInit;
	
	Uart_timerInit(baudSpeedToInit);
	Uart_timerStart();
}

void Uart_deinit(Uart_Name uart)
{
	switch (uart)
	{
		case Uart0:
			UBRR0L = 0; UBRR0H = 0; UCSR0A = 0; UCSR0B = 0; UCSR0C = 0;
		break;
	
		case Uart1:
			UBRR1L = 0; UBRR1H = 0; UCSR1A = 0; UCSR1B = 0; UCSR1C = 0;
		break;
	}
}


void Uart_addByte(Uart_Name uart, uint8_t byte){

	UART_t* uartStr = &UartStructs[uart-Uart0];
	if (uartStr->txBuffer.amount < UART_BUFFER_SIZE){
		
		uartStr->txBuffer.buffer[uartStr->txBuffer.writePos]= byte ;
		Uart_bufferIncrementer(&uartStr->txBuffer);
	}
}


void Uart_addHalfWord(Uart_Name uart, uint16_t halfWord){

	Uart_addByte(uart, (uint8_t) (halfWord >> 8));
	Uart_addByte(uart, (uint8_t) halfWord);
}


void Uart_addWord(Uart_Name uart, uint32_t word){

	Uart_addHalfWord(uart, (uint16_t) (word >> 16));
	Uart_addHalfWord(uart, (uint16_t) word);
}


void Uart_addString(Uart_Name uart, char* pointer, Uart_Lastbyte end_cmd)
{
	if  (*pointer != 0 ) {
		while (*pointer != 0){
			//copy to buffer 1 symbol at once
			Uart_addByte(uart, *pointer);
			pointer += 1;
		}
	}
	else 
	{
	 	return;
	}

	if (*pointer == 0){

		switch (end_cmd) {

			case LFCR:
			Uart_addByte(uart, 0x0A);
			Uart_addByte(uart, 0x0D);
			break;

			case CRLF:
			Uart_addByte(uart, 0x0D);
			Uart_addByte(uart, 0x0A);
			break;

			case LF:
			Uart_addByte(uart, 0x0A);
			break;

			case CR:
			Uart_addByte(uart, 0x0D);
			break;

			case COLON:
			Uart_addByte(uart, 0x3A);
			break;

			case SPACE:
			Uart_addByte(uart, 0x20);
			break;

			case DOT:
			Uart_addByte(uart, 0x2E);
			break;
			
			default:
			break;
		}
	}
}

void Uart_addArray(Uart_Name uart, uint8_t *data, uint16_t cnt)
{
	if (cnt != 0 && data != 0){

		for(uint16_t n = 0; n < cnt; n++) {
			Uart_addByte(uart, *(data + n));
		}
	}
}

void Uart_addCrc16(Uart_Name uart, uint16_t(*crc16Func)(uint8_t*, uint32_t)){

	UART_t* uartStr = &UartStructs[uart-Uart0];
	if (uartStr->txBuffer.amount + 2 <= UART_BUFFER_SIZE){
		uint16_t crc = crc16Func((uint8_t*) &uartStr->txBuffer.buffer, uartStr->txBuffer.writePos);
		Uart_addHalfWord(uart, crc);
	}
}


void Uart_resetRxPointer(Uart_Name uart){

	UART_t* uartStr = &UartStructs[uart-Uart0];
	uartStr->rxBuffer.readPos = 0;
	uartStr->rxBuffer.writePos = 0;
	uartStr->rxBuffer.amount = 0;
}


void Uart_resetTxPointer(Uart_Name uart){

	UART_t* uartStr = &UartStructs[uart-Uart0];
	uartStr->txBuffer.readPos = 0;
	uartStr->txBuffer.writePos = 0;
	uartStr->txBuffer.amount = 0;
}


void Uart_setDirection(Uart_Name uart, Port_Direction_t direction){

	UART_t* uartStr = &UartStructs[uart-Uart0];

	if (direction == IN){

		uartStr->direction = IN;
		if (uartStr->type == RS485){
			if (uartStr->enableEnablePinInvesion){
				Port_setPin(uartStr->enablePort, uartStr->enablePin);
			}
			else
			{
				Port_resetPin(uartStr->enablePort, uartStr->enablePin);
			}
		}
	}
	else
	{
		uartStr->direction = OUT;

		if (uartStr->type == RS485){
			if (uartStr->enableEnablePinInvesion){
				Port_resetPin(uartStr->enablePort, uartStr->enablePin);
			}
			else
			{
				Port_setPin(uartStr->enablePort, uartStr->enablePin);
			}
		}
	}
}


void Uart_setEnablePin(Uart_Name uart, Port_Name_t port, Port_Pin_t pin, bool inversion){
	
	UART_t* uartStr = &UartStructs[uart-Uart0];
	uartStr->enablePort = port;
	uartStr->enablePin = pin;
	uartStr->enableEnablePinInvesion = inversion;
}


void Uart_setNeedTimerNum(Uart_Name uart, uint16_t timerNum){
	
	UART_t* uartStr = &UartStructs[uart-Uart0];
	
	if (timerNum < UART_TIMER_NUMS_TOTAL_AMOUNT){

		uartStr->needTimerNum = timerNum;
		uartStr->timerCounterDown = uartStr->numsOfRepeats[timerNum];

	}
}

void Uart_timerInit(uint32_t baudSpeed){

	//Timer 2
	 TIMSK &= ~(1 << OCIE2);	 // disable interrupt
 
	 uint16_t timerDivisor = 256;

	 //stop clocking
	 TCCR2 = 0;
	 TCNT2 = 0;														// reset TCNT counter register
	 OCR2 = (CPU_FREQUENCY / timerDivisor) / (baudSpeed / 8) + 1;	// set top value
 
	 TCCR2 |= 1 << WGM21;
 
}


void Uart_timerStop(){
	
	TIMSK &= ~(1 << OCIE2);
}


void Uart_timerStart(){
	
	TIMSK |= (1 << OCIE2);
	TCCR2 |= 1 << CS22;
}


Port_Direction_t Uart_getDirection(Uart_Name uart){

	UART_t* uartStr = &UartStructs[uart-Uart0];
	return uartStr->direction;
}


void Uart_updateTimerNumsFromIRQ(Uart_Name uart){

	UART_t* uartStr = &UartStructs[uart-Uart0];
	
	if (uartStr->timerCounterDown == 0){
		
		switch (uartStr->needTimerNum){
			
			case uartTimerRepeatesStartTransmit:
			
			uartStr->needTimerNum = uartTimerRepeatesNotCounting;
			Uart_sendNextByte(uart);
			break;
			
			case uartTimerRepeatesSwitchToReadAfterTransmit:

			uartStr->needTimerNum = uartTimerRepeatesNotCounting;
			
			if (uartStr->callback_userSwitchToIn){
				uartStr->callback_userSwitchToIn(uart);
			}

			Uart_setDirection(uart, IN);
			Uart_resetTxPointer(uart);
			break;

			case uartTimerRepeatesProccessingRecievedMessage:
					
			uartStr->needTimerNum = uartTimerRepeatesNotCounting;
			if (uartStr->callback_userRecieve){
				uartStr->callback_userRecieve(uart);
			}
			else 
			{
				Uart_resetRxPointer(uart);	
			}
			break;

			case uartTimerRepeatesUserTimeout1:
			
			uartStr->needTimerNum = uartTimerRepeatesNotCounting;
			if (uartStr->callback_userRecieve){
				uartStr->callback_userRecieve(uart);
			}
			else
			{
				Uart_resetRxPointer(uart);
			}
			break;
		}
	}
}


void Uart_setRecieveFunction(Uart_Name uart, void (*newUserRecieve)(Uart_Name)){
	
	UART_t* uartStr = &UartStructs[uart-Uart0];
	uartStr->callback_userRecieve = newUserRecieve;
}


// USART0 Receiver interrupt service routine
ISR(USART0_RX_vect)
{	
	UART_t* uartStr = &UartStructs[0];	// Uart_getMainStruct(Uart0);
	
	if ((uartStr->direction == IN && uartStr->type == RS485) || uartStr->type == UART)
	{
		if (uartStr->callback_userRxInterrupt){
			uartStr->callback_userRxInterrupt(Uart0);
		}
		else
		{
			Uart_addByteToRxBuffer(Uart0, UDR0);
			Uart_setNeedTimerNum(Uart0, uartTimerRepeatesProccessingRecievedMessage);
		}
	
		if (uartStr->callback_afterByteRecieved){
			uartStr->callback_afterByteRecieved(Uart0);
		}
	}
	else UDR0;
	//just read byte register
}

// USART0 Transmitter interrupt service routine
ISR(USART0_TX_vect)
{
	if (UartStructs[0].txBuffer.amount != 0)	// Uart_isDataToSend(Uart0)
	{
		Uart_sendNextByte(Uart0);
	}
	else
	{
		Uart_setNeedTimerNum(Uart0, uartTimerRepeatesSwitchToReadAfterTransmit);
	}
}

// USART1 Receiver interrupt service routine
ISR(USART1_RX_vect)
{	
	UART_t* uartStr = &UartStructs[1];	// Uart_getMainStruct(Uart1);
	
	if ((uartStr->direction == IN && uartStr->type == RS485) || uartStr->type == UART)
	{
		if (uartStr->callback_userRxInterrupt){
			uartStr->callback_userRxInterrupt(Uart1);
		}
		else
		{		
			Uart_addByteToRxBuffer(Uart1, UDR1);
			Uart_setNeedTimerNum(Uart1, uartTimerRepeatesProccessingRecievedMessage);
		}
	
		if (uartStr->callback_afterByteRecieved){
			uartStr->callback_afterByteRecieved(Uart1);
		}
	}
	else UDR1;
	//just read byte register
}

// USART1 Transmitter interrupt service routine
ISR(USART1_TX_vect)
{
	if (UartStructs[1].txBuffer.amount != 0)	// Uart_isDataToSend(Uart1)
	{
		Uart_sendNextByte(Uart1);
	}
	else
	{
		Uart_setNeedTimerNum(Uart1, uartTimerRepeatesSwitchToReadAfterTransmit);	
	}
}

ISR(TIMER2_COMP_vect)
{	
	for (uint8_t i = 0; i < UART_AMOUNT; i++){
		if (UartStructs[i].needTimerNum != uartTimerRepeatesNotCounting){

			if (UartStructs[i].timerCounterDown > 0){
				UartStructs[i].timerCounterDown -= 1;
			}
			if (UartStructs[i].timerCounterDown == 0){
				Uart_updateTimerNumsFromIRQ(Uart0 + i);
			}
		}
	}
}