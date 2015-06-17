// Do not remove the include below
#include "M6050comp.h"

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
//Adafruit_PCD8544 LCD = Adafruit_PCD8544(7, 6, 5, 4, 3);
// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 6 - Data/Command select (D/C)
// pin 5 - LCD chip select (CS)
// pin 4 - LCD reset (RST)
Adafruit_PCD8544 LCD = Adafruit_PCD8544(6, 5, 4);
HMC5883L compass;
MPU6050 mpu;
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
int buttonPin = 8;
//int battAPin = A0; int battBPin = A1;
//float _battA = 0; float _battB = 0;
int interval = 5; int iz = 0;
float tempBMP;
float tempMPU;
float pressure;
float altitude;
float heading;
float headingDegrees;
float calculseaLevelPressure;
float seaLevelPressure = 1013.25;	//Standardny tlak na hladine mora
//		float seaLevelPressure = 1013.25; 	//Nadmorska vyska

// Set declination angle on your location and fix heading
// You can find your declination on: http://magnetic-declination.com/
// (+) Positive or (-) for negative
// For Bytom / Poland declination angle is 4'26E (positive)
// Formula: (deg + (min / 60.0)) / (180 / M_PI);
float declinationAngle = (4.0 + (34.0 / 60.0)) / (180 / M_PI);

// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!
void zobraz()
{
	LCD.setTextSize(1);
	LCD.setTextColor(BLACK);
	LCD.setCursor(0,0);
	LCD.print("Tepl.BMP:");
	LCD.print(tempBMP);
	LCD.print("Tepl.MPU:");
	LCD.print(tempMPU);
	LCD.print("Tlak:");
	LCD.print(pressure/100, 2);
	LCD.print("hPa");
	LCD.print("More:");
	LCD.print(calculseaLevelPressure,1);
	LCD.print("hPa");
	LCD.print("Nadm.v.:");
	LCD.println(altitude, 1);
	LCD.print("Azimut :");
	LCD.print(headingDegrees, 0);
	LCD.display();
	LCD.clearDisplay();
}
void zapis()
{
	Serial.print(tempBMP, 1);
	Serial.print("\t");
	Serial.print(tempMPU, 1);
	Serial.print("\t");
	Serial.print(pressure/100, 2);
	Serial.print("\t");
	Serial.print(calculseaLevelPressure, 1);
	Serial.print("\t");
	Serial.print(altitude, 1);
	Serial.print("\t");
	Serial.println(headingDegrees, 0);
}
//The setup function is called once at startup of the sketch
void setup()
{
// Add your initialization code here
	pinMode(buttonPin,INPUT_PULLUP);
	LCD.begin(58);
	delay(500);
	LCD.clearDisplay();
	LCD.setRotation(0);
	LCD.setTextSize(1);
	LCD.setTextColor(BLACK);
	LCD.setCursor(20,0);
	LCD.println("MPU-6050");
	LCD.setCursor(40,15);
	LCD.println("+");
	LCD.setCursor(22,30);
	LCD.println("BMP-085");
	LCD.display();
	delay(1500);
	LCD.clearDisplay();
	Serial.begin(57600);
// If you have GY-86 or GY-87 module.
// To access HMC5883L you need to disable the I2C Master Mode and Sleep Mode, and enable I2C Bypass Mode
	while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
	{
		Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
	    delay(500);
	}
	while(!bmp.begin())
	{
	    Serial.println("Could not find a valid BMPU085 sensor, check wiring!");
	    delay(500);
	 }
	mpu.setI2CMasterModeEnabled(false);
	mpu.setI2CBypassEnabled(true) ;
	mpu.setSleepEnabled(false);
	// Initialize Initialize HMC5883L
	Serial.println("Initialize HMC5883L");
	while (!compass.begin())
	{
	    Serial.println("Could not find a valid HMC5883L sensor, check wiring!");
	    delay(500);
	}
// Set measurement range
	compass.setRange(HMC5883L_RANGE_1_3GA);
// Set measurement mode
	compass.setMeasurementMode(HMC5883L_CONTINOUS);
// Set data rate
	compass.setDataRate(HMC5883L_DATARATE_15HZ);
// Set number of samples averaged
	compass.setSamples(HMC5883L_SAMPLES_8);
// Set calibration offset. See HMC5883L_calibration.ino
	compass.setOffset(-111, 56, -66); //0,0 povodne
}
// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
	iz++;
	tempBMP =  0; tempMPU = 0; pressure = 0;
	altitude = 0; headingDegrees = 0; float _tempBMP = 0;
	float _pressure = 0;
	calculseaLevelPressure = 0;		//Prepoc. tlak na urovni mora
	sensors_event_t event;
	bmp.getEvent(&event);
	for(int i=1;i<=4;i++)
	{
		Vector norm = compass.readNormalize();
// Calculate heading
		heading = atan2(norm.YAxis, -norm.ZAxis);
		heading += declinationAngle;
// Correct for heading < 0deg and heading > 360deg
		if (heading < 0) heading += 2 * PI;
		if (heading > 2 * PI) heading -= 2 * PI;
// Convert to degrees
		headingDegrees += heading * 180/M_PI;
// Read temp from MPU
		tempMPU += mpu.readTemperature();
// Read temp from BMP
		bmp.getTemperature(&tempBMP);
		_tempBMP += tempBMP;
		bmp.getPressure(&pressure);
		_pressure += pressure;
		bool buttonValue = digitalRead(buttonPin);
//		Serial.println(buttonValue); 	//stlaceny = 0
		if(!buttonValue) seaLevelPressure=_pressure/i/100;
// Then convert the atmospheric pressure, and SLP to altitude
// Update this next line with the current SLP for better results
// Read altitude from BMP
		calculseaLevelPressure += bmp.seaLevelForAltitude(326, event.pressure);
		altitude += bmp.pressureToAltitude(seaLevelPressure, event.pressure);
		delay(240);
	}
	headingDegrees /= 4; tempMPU /= 4;
	tempBMP = _tempBMP/4; altitude /= 4;
	calculseaLevelPressure /= 4; pressure = _pressure/4;
	zobraz();
	if(Serial){
		if (interval==iz)
		{
			zapis();
			iz = 0;
		}
	}
	else iz=0;
}


