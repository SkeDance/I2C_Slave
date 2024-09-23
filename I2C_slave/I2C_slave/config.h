#ifndef CONFIG_FILE
#define CONFIG_FILE

#define CPU_FREQUENCY 16000000UL
#define F_CPU CPU_FREQUENCY
#define FLASH_MEMORY_SIZE 131072UL

// For i2c lib
#define DATA_SIZE 8				//размер карты регистров
#define READ_REGISTERS_SIZE 8	//количество байт для чтения
#define TX_DATA_SIZE 17			//не меньше 1
#define I2C_MASTER_BUFFER_SIZE 1
#define I2C_MASTER_MAP_SIZE 1
#define I2C_ALL_SLAVES_REGISTERS_MAP 1

// For port libs
#define CONFIG_PORT_CONNECTED_PIN_AMOUNT 16

// For planner libs
#define WORK_PLANNER_TIMER0

// For UART base lib
#define UART_BUFFER_SIZE 255
#define UART_AMOUNT 2
#define UART_TIMER_NUMS_TOTAL_AMOUNT 6
#define UART_PROCESSING_RECIEVED_MESSAGE_IN_BYTES 4
//#define UART_ENABLE_MAGIC_CODE

#define ZERO_BIT	0x01
#define FIRST_BIT	0x02
#define SECOND_BIT	0x04
#define THIRD_BIT	0x08
#define FOURTH_BIT	0x10
#define FIFTH_BIT	0x20
#define SIXTH_BIT	0x40
#define SEVENTH_BIT	0x80

#endif
