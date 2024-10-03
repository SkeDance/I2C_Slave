#ifndef DEVICE_H_
#define DEVICE_H_

// Ports:
#define	PORT_DO1			PortE, pin0
#define	PORT_DO2			PortE, pin1
#define	PORT_DO3			PortE, pin2
#define	PORT_DO4			PortE, pin4
#define	PORT_DO5			PortE, pin5
#define	PORT_DO6			PortE, pin6
#define	PORT_DO7			PortE, pin7

#define PORT_DO8			PortB, pin2
#define PORT_DO9			PortB, pin3
#define PORT_DO10			PortB, pin5
#define PORT_DO11			PortB, pin6

#define PORT_TEST			PortA, pin0
#define PORT_TEST2			PortA, pin1

// Addresses
#define	I2C_SLAVE_ADDRESS	0b00000001

// Reg map
typedef enum
{
	RegisterMap_CrcA = 0,
	RegisterMap_CrcB = 1,

}RegisterMap;

#define REG_CRC_A		RegisterMap_CrcA
#define REG_CRC_B		RegisterMap_CrcB

#endif /* DEVICE_H_ */