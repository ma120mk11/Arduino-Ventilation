#ifndef SETTINGS_H
#define SETTINGS_H
//************************    PINS    *********************************

// USE TX2 for Nextion display communication

#define MOTOR1 9            // motor1 pin HEATED AIR
#define MOTOR2 10			// motor2 pin DIRECT AIR

#define outside_temp_pin 52 // DS18B20 digital temperature sensor
/**
 * Use a 10k resistor between signal and ???
 */

#define T_OUTSIDE A4		// LM35 temp sensor0 pin	OUTSIDE TEMP
#define T_PANEL A1   	    // LM35 temp sensor1 pin  	PANEL TEMP
#define T_AIR A2   	        // LM35 temp sensor2 pin	HOT AIR
#define T_LIVINGROOM A3   	// LM35 temp sensor3 pin	LIVING ROOM
#define TempSensor4 A5 
#define CURRENT A7          // Current sensor pin
#define VOLTAGE A8          // Voltage sensor pin
#define LightSensorPin A9   // Light sensor pin

#define chipSelect 53		// pin for SD card chip select (see SD_card.h)
#define pinReference 5      // The measured value of +5V pin

// pins for DS1302 RTC Module
#define rtc_RST 8
#define rtc_IO 7
#define rtc_SCL 6


// ***************** SENSOR OFFSET **********************
#define t0sensorOffset 0
#define t1sensorOffset 1
#define t2sensorOffset 1.5
#define t3sensorOffset 2
#define voltageOffset 0
#define currentOffset 0

#define light_max_Reading 900		// The adc value when full light MAX = 1023





// *****************  THINGSPEAK **********************
#define CHANNEL_ID 1327374
#define CHANNEL_API_KEY "SYJB15HBTE9V8CWR"



// ****************  LOOP FUNCTION *********************
#define ONE_SEC 1000
#define THREE_SEC 3000
#define ONE_MIN 60000

#endif