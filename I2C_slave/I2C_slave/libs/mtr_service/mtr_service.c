#include "mtr_service/mtr_service.h"
#include "mtr_uart/mtr_uart.h"

uint32_t deviceID;
uint8_t bootloaderMagicData[] = "IREALLYWANTTOGOTOBOOTLOADERNOW";
uint8_t toBootloaderResetCommand[39];

volatile static uint32_t delay_dec = 0;
extern UART_t UartStructs[UART_AMOUNT];

hexCodes_t hexCodes[] = {
    {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9}, {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}, {'a', 10}, {'b', 11}, {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15},
};

uint32_t checkParameter(uint32_t value, uint32_t minValue, uint32_t maxValue, uint32_t defaultValue){

	if (value > maxValue){
		return defaultValue;
	}

	if (value < minValue){
		return defaultValue;
	}

	if (value == 0xFFFFFFFF){
		return defaultValue;
	}

	return value;
}

uint16_t bitesToBytes(uint16_t bites){

	uint16_t bytes = 0;

	if (bites < 8){

		bytes = 1;
	}
	else
	{

		while (bites > 7){

			bytes +=1;
			bites -= 8;
		}

		if (bites != 0) bytes += 1;

	}

	return bytes;

}

bool isEqualData(uint32_t byteSize, uint8_t *pBuffer1, uint8_t *pBuffer2)
{
    uint32_t i;

    for(i = 0; i < byteSize; i++)
    {
        if (*pBuffer1++ != *pBuffer2++){
            return(false);
        }
    }
  return(true);
}

inline void write32To8Bits(uint8_t* to, uint32_t what){

	to[0] = (uint8_t) (what >> 24);
	to[1] = (uint8_t) (what >> 16);
	to[2] = (uint8_t) (what >> 8);
	to[3] = (uint8_t) (what >> 0);
}

inline uint32_t get32From8Bits(uint8_t* from){

	uint32_t temp = 0;
	temp |= 0x000000FF & from[3];
	temp |= 0x0000FF00 & (from[2] << 8);
	temp |= 0x00FF0000 & (from[1] << 16);
	temp |= 0xFF000000 & (from[0] << 24);
	return temp;
}


inline uint16_t get16From8BitsBE(uint8_t* from){
	uint16_t temp = 0;

	temp = from[1] | (((uint16_t) from[0]) << 8);
	return temp;
}

inline uint16_t get16From8BitsLE(uint8_t* from){
	uint16_t temp = 0;

	temp = from[0] | (((uint16_t) from[1]) << 8);
	return temp;
}


inline void set8From32Bits(uint8_t* to, uint32_t* from){

	*to = (uint8_t) (*from >> 24);
	*(to + 1) = (uint8_t) (*from >> 16);
	*(to + 2) = (uint8_t) (*from >> 8);
	*(to + 3) = (uint8_t) *from;
}

inline void set8From16Bits(uint8_t* to, uint16_t* from){

	*to = (uint8_t) (*from >> 8);
	*(to + 1) = (uint8_t) *from;
}

//Array shift
void byteShiftLeft(uint8_t* where, uint16_t size, uint16_t shiftNum){

	for (uint16_t i = 0; i < (size - shiftNum); i++){
		*(where + i) = *(where + i + shiftNum);
	}

	for (uint16_t i = shiftNum; i < size ; i++){

		*(where + i) = 0;

	}
}

inline uint32_t swapBytes32(uint32_t wordToSwap){
	return ((wordToSwap >> 24) | (wordToSwap & 0xFF0000) >> 8 | (wordToSwap & 0xFF00) << 8 | (wordToSwap & 0xFF) << 24);
}

inline uint16_t swapBytes16(uint16_t wordToSwap){
	return ((wordToSwap & 0xFF00) >> 8 | (wordToSwap & 0xFF) << 8);
}

inline void toZeroByteFunction(uint8_t* toZeroPointer, uint16_t size){

	for (uint16_t i = 0; i < size; i++){

		toZeroPointer[i] = 0;
//		*(toZeroPointer+i) = 0;

	}
}

inline void toZeroHalfWordFunction(uint16_t* toZeroPointer, uint16_t size){

	for (uint16_t i = 0; i < size; i++){

		toZeroPointer[i] = 0;
		//*(toZeroPointer+i) = 0;

	}
}

inline void toZeroWordFunction(uint32_t* toZeroPointer, uint16_t size){

	for (uint16_t i = 0; i < size; i++){

		toZeroPointer[i] = 0;
		//*(toZeroPointer+i) = 0;

	}
}

inline void charToHex(uint8_t *numsFrom, uint16_t length, uint8_t* to)
{
      char charTemp[2];
      for (uint16_t i = 0; i < length; i+= 1){
          charTemp[0] = *(numsFrom + 2*i);
          charTemp[1] = *(numsFrom + 2*i + 1);

          uint8_t temp = 0;
          for(uint8_t i = 0; i < 22; i++){

              if (charTemp[0] == hexCodes[i].leteral){
                  temp |= temp + (hexCodes[i].num * 16);
              }

              if (charTemp[1] == hexCodes[i].leteral){
                  temp |= temp + hexCodes[i].num;
              }
          }
          *(to + i) = temp;
      }
}


inline void hexToChar(uint8_t* numsFrom, uint16_t length, uint8_t* to)
{
      for (uint16_t i = 0; i < length; i+= 1){
        uint8_t num = *(numsFrom + i);
        to[2*i] = hexCodes[num >> 4].leteral;
        to[2*i + 1] = hexCodes[num & 0xf].leteral;
      }
}


uint16_t frvToUint(float doubleNum, uint8_t resolutionPow, uint16_t offset){

	uint32_t temp = 1;
	while(resolutionPow--){
		temp *= 10;
	}

   return  ((uint16_t) ((double) doubleNum * temp) + offset);

}

float uintToFrv(uint16_t uintNum, uint8_t resolutionPow, uint16_t offset){

	uint32_t temp = 1;
	while(resolutionPow--){
		temp *= 10;
	}

   return ((float) ((int) uintNum - offset) / temp);
}

int foundNextBytePosition(uint8_t symbol, uint8_t* data){
	int i = -1;
	while(*(data++)){
		if (++i == 256)
			break;
	}
	return i;
}

void initDevice(){
	
	uint8_t temp = MCUCR;
	MCUCR |= (1<<IVCE);
	MCUCR = temp & ~(1<<IVSEL | 1 << IVCE);
	
	uint32_t tempDeviceId = eeprom_read_dword((uint32_t*)(PARAMETER_ID * 4));
	eeprom_busy_wait();
	setDeviceId(tempDeviceId);
	
	set8From32Bits(toBootloaderResetCommand, &deviceID);
	toBootloaderResetCommand[4] = 0xFF;
	toBootloaderResetCommand[5] = 0xFF;
	toBootloaderResetCommand[6] = 0xFF;
	
	memcpy(toBootloaderResetCommand + 7, bootloaderMagicData, sizeof(bootloaderMagicData) -1);
	uint16_t crcToBootloaderMessage = Crc_getCrc16(toBootloaderResetCommand, (uint32_t) 37);
	
	set8From16Bits(toBootloaderResetCommand + (sizeof(bootloaderMagicData) + 7 - 1), &crcToBootloaderMessage);
	
	
	toZeroByteFunction(UartStructs, 2 * sizeof(UART_t));
}

void goToBootloader(){
	asm("jmp 0x1E000"); //go to bootloader
}

uint32_t getDeviceId(){

	return 0x1FFFFFFF & deviceID;
}

void setDeviceId(uint32_t newDeviceId){

	deviceID =  0x1FFFFFFF & newDeviceId;
}

#ifdef useMotorValueConvertor
//Type data motor float 1/2/3/4
//----Motor float 1----
uint16_t motorValue1(float value) {
	return (uint16_t) ((value + 327.67) / 0.01);
}

float deMotorValue1(uint16_t value) {
	return (float) ((value * 0.01) - 327.67);
}

//----Motor float 2----
uint16_t motorValue2(float value) {
	return (uint16_t) ((value + 3276.7) / 0.1);
}

float deMotorValue2(uint16_t value) {
	return (float) ((value * 0.1) - 3276.7);
}

//----Motor float 3----
uint16_t motorValue3(float value) {
	return (uint16_t) (value / 0.1);
}

float deMotorValue3(uint16_t value) {
	return (float) (value * 0.1);
}

//----Motor float 4----
uint16_t motorValue4(float value) {
	return (uint16_t)(value / 0.01);
}

float deMotorValue4(uint16_t value) {
	return (float) (value * 0.01);
}

//Getter
float getFloatValue(uint16_t dataHi, uint16_t dataLo)
{
	uint16_t floatBuffer[2];
	floatBuffer[0] = dataHi;
	floatBuffer[1] = dataLo;
	return (*((float*) floatBuffer));
}
#endif