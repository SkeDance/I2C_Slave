// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup() {
  Wire.begin(); // join I2C bus (address optional for master)
}

byte x = 1;

void loop() {
  Wire.beginTransmission(1); // transmit to device #1
  Wire.write(8);              // sends one bytes
  Wire.write(1);              // sends one byte
  Wire.endTransmission();    // stop transmitting

  //x++;
  delay(500);
}
