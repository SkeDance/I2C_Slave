#ifndef MTR_TOOLS_H_
#define MTR_TOOLS_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "mtr_service/mtr_service.h"
#include "../../config.h"

#define getBit(x, y)	( ( x >> y ) & 1		)
#define setBit(x, y) 	(	x |=	 (1UL<<y)	)
#define clearBit(x, y)	(	x &=	(~(1UL<<y))	)

#define	PARAMETER_BOOTSELECT 1
#define PARAMETER_ID 2
#define	PARAMETER_FIRMWARE_SIZE 3
#define PARAMETER_FIRMWARE_CRC 4
#define	PARAMETER_FIRMWARE_VERSION 5
#define PARAMETER_FIRMWARE_TRIES 6
//unused #define PARAMETER_CONNECTION_RXPINPORT 7
//unused #define PARAMETER_CONNECTION_TXPINPORT 8
#define PARAMETER_CONNECTION_ENPINPORT 9
#define PARAMETER_CONNECTION_SPEED_AND_PARITY 10
#define PARAMETER_DEVICE_TEST_LED_PORTPIN 11
#define PARAMETER_START_DELAY 12

#define PARAMETER_EXPERTMODE 511
#define PARAMETER_PROGRAMM_PROFILE 513


//Backward compatibility
#define get16From8Bits(a)		get16From8BitsBE((a))		


typedef struct
{
    char leteral;
    uint8_t num;

}hexCodes_t;

typedef enum{

	Uart0Connection = 10,
	Uart1Connection = 11,
	SpiConnection = 100,
	I2CConnection = 120,
	Unknown = 255,
	NoConnection = 0x1F

}AvaliableConnectionType_t;

/*
 * Error types
 */

typedef uint8_t errorType;

#define E_NO_ERROR 0
#define E_NULL_POINTER 1
#define E_INVALID_VALUE 2
#define E_OUT_OF_RANGE 3
#define E_BUSY 4
#define E_IO_ERROR 5
#define E_OUT_OF_MEMORY 6
#define E_NOT_IMPLEMENTED 7
#define E_CRC 8
#define E_STATE 9
#define E_TIME_OUT 10



void initDevice();
void goToBootloader();
uint32_t checkParameter(uint32_t value, uint32_t minValue, uint32_t maxValue, uint32_t defaultValue);
bool isEqualData(uint32_t byteSize, uint8_t *pBuffer1, uint8_t *pBuffer2);

void write32To8Bits(uint8_t* to, uint32_t what);
uint32_t get32From8Bits(uint8_t* from);
uint16_t get16From8BitsBE(uint8_t* from);		//Big-Endian
uint16_t get16From8BitsLE(uint8_t* from);		//LittleEndian
void set8From32Bits(uint8_t* to, uint32_t* from);
void set8From16Bits(uint8_t* to, uint16_t* from);


/**	@brief Return need bytes to safe bites size of parameter bites
 * 	1bites = 1 byte;
 * 	...
 * 	8bites = 1 byte;
 * 	9bites = 2 byte;
 * 	10 bites = 2 byte;
 * 	@param bites bite size
 * 	@return need bytes size
 *
 */
uint16_t bitesToBytes(uint16_t bites);

void byteShiftLeft(uint8_t* where, uint16_t size, uint16_t shiftNum);

uint32_t swapBytes32(uint32_t wordToSwap);
uint16_t swapBytes16(uint16_t wordToSwap);

/**@brief Set bytes to zero.
 * @param toZeroPointer pointer to data
 * @param size byte size to zero set
 * return void
 */
void toZeroByteFunction(uint8_t* toZeroPointer, uint16_t size);
void toZeroHalfWordFunction(uint16_t* toZeroPointer, uint16_t size);
void toZeroWordFunction(uint32_t* toZeroPointer, uint16_t size);

void charToHex(uint8_t *numsFrom, uint16_t length, uint8_t* to);
void hexToChar(uint8_t* numsFrom, uint16_t length, uint8_t* to);

uint16_t frvToUint(float doubleNum, uint8_t resolutionPow, uint16_t offset);
float uintToFrv(uint16_t uintNum, uint8_t resolutionPow, uint16_t offset);

/**@brief Try to found symbol position in data
 * @param symbol byte try to found
 * @param data pointer to search
 * return position, if not found in next 255 bytes return -1
 */
int foundNextBytePosition(uint8_t symbol, uint8_t* data);

uint32_t getDeviceId();
void setDeviceId(uint32_t newDeviceId);

#ifdef useMotorValueConvertor
uint16_t motorValue1(float value);
float deMotorValue1(uint16_t value);
uint16_t motorValue2(float value);
float deMotorValue2(uint16_t value);
uint16_t motorValue3(float value);
float deMotorValue3(uint16_t value);
uint16_t motorValue4(float value);
float deMotorValue4(uint16_t value);
float getFloatValue(uint16_t dataHi, uint16_t dataLo);
#endif

#ifdef __cplusplus
}
#endif

#endif /* MTR_TOOLS_H_ */
