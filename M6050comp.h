// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _M6050comp_H_
#define _M6050comp_H_
#include "Arduino.h"
//add your includes for the project M6050comp here
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Wire.h>
#include <HMC5883L.h>
#include <MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project M6050comp here
void zobraz(); 	//Zobrazi na LCD
void zapis();	//Zapis cez serial

//Do not add code below this line
#endif /* _M6050comp_H_ */
