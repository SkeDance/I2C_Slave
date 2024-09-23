/**
 * @file
 * @brief This file contain function for calculating CRC hash codes.
 *
 * This is full description of the file
 * @ingroup mtr_crc
 *
 *
 */

#ifndef MTR_CRC_H_
#define MTR_CRC_H_

#include <stdint.h>
#include <avr/pgmspace.h>


/*
 * @brief Calculate CRC16-MODBUS using data pointer and length and get it
 * @param data : pointer to data need to calculate
 * @param length : length of data
 * @retval : return CRC16-MODBUS hash code
 *
 */
uint16_t Crc_getCrc16Modbus(uint8_t *data, uint32_t length);


/*
 * @brief Calculate CRC8 using data pointer and length and get it
 * @param data : pointer to data need to calculate
 * @param length : length of data
 * @return : return CRC8 hash code
 *
 */
uint8_t Crc_getCrc8(uint8_t *data, uint32_t length);


/*
 * @brief Calculate CRC16 using data pointer and length and get it
 * @param data : pointer to data need to calculate
 * @param length : length of data
 * @return : return CRC16 hash code
 *
*/
uint16_t Crc_getCrc16(uint8_t* data, uint32_t length);

/*
 * @brief Calculate CRC32 using data pointer and length and get it
 * @param data : pointer to data need to calculate
 * @param length : length of data
 * @return : return CRC32 hash code
 *
*/
uint32_t Crc_getCrc32(uint8_t* data, uint32_t length);


/*
 * @brief Calculate CRC16 using full flash memory size (default 131072UL) and get it
 * @param length : flash memory size
 * @return : return CRC16 flash memory
 *
*/
uint16_t Crc_getCrc16Flash (uint32_t length);

#endif /* MTR_CRC_H_ */
