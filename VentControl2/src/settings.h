#ifndef SETTINGS_H
#define SETTINGS_H

// Enable features
//#define DEBUG
//#define VERBOSE_DB
#define NEXTION
#define THINGSPEAK 
#define EEPROM_STORE 
//#define TELEMETRY 


//************************    PINS    *********************************
// USE TX2 for Nextion display communication

#define MOTOR1 12            // motor1 pin HEATED AIR
#define MOTOR2 13			 // motor2 pin DIRECT AIR
#define outside_temp_pin 52  // DS18B20 digital temperature sensor
/**
 * Use a 10k resistor between signal and 5V
*/

#define T_OUTSIDE 52		// LM35 temp sensor0 pin	OUTSIDE TEMP
#define T_PANEL A1   	    // LM35 temp sensor1 pin  	PANEL TEMP
#define T_AIR A2   	        // LM35 temp sensor2 pin	HOT AIR
#define T_LIVINGROOM A3   	// LM35 temp sensor3 pin	LIVING ROOM
#define CURRENT A7          // Current sensor pin
#define VOLTAGE A8          // Voltage sensor pin
#define LightSensorPin A9   // Light sensor pin

// Select which SD card reader is used:
#define CYTRON_SD
//#define EXTERNAL_SD

#ifdef EXTERNAL_SD
    #define chipSelect 53		// pin for SD card chip select (see SD_card.h)
#endif
#ifdef CYTRON_SD
    #define chipSelect 4        // pin for SD card on Wifi shield
#endif

// pins for DS1302 RTC Module
#define RTC_RST 8
#define RTC_IO 7
#define RTC_SCL 6


// ***************** SENSOR OFFSET **********************
#define t0sensorOffset 0
#define t1sensorOffset 1
#define t2sensorOffset 1.5
#define t3sensorOffset 2
#define voltageOffset 0
#define currentOffset 0

#define HYSTERESIS 20           // Number of times to read value over threshold before changing motor speed

#define light_max_Reading 900   // The adc value when full light MAX = 1023

#define pinReference 5          // The measured value of +5V pin

// *****************  THINGSPEAK **********************
#define CHANNEL_ID 1327374
#define CHANNEL_API_KEY "SYJB15HBTE9V8CWR"


// *****************  MODES **********************
#define MANUAL 0
#define HEATING 1
#define ADVANCED 2

// ****************  LOOP FUNCTION *********************
#define ONE_SEC 1000
#define THREE_SEC 3000
#define FIVE_SEC 5000
#define ONE_MIN 60000

// ****************  ERRORS  *********************
#define ERR_VOLTAGE     // Low voltage
#define ERR_CURRENT     
#define ERR_MOTOR1      // Motor1 didn't start (no current detected)
#define ERR_MOTOR1      // Motor2 didn't start (no current detected)
#define ERR_WFIF        // Wifi 
#define 


#ifdef DEBUG
    #define DBPRINT(...) Serial.print(__VA_ARGS__)
    #define DBPRINT_LN(...) Serial.println(__VA_ARGS__)
#else
    #define DBPRINT(...) 
    #define DBPRINT_LN(...)
#endif

#ifdef VERBOSE_DB
    #define verboseDb(a) Serial.print(a)
    #define verboseDbln(a) Serial.println(a)
#else
    #define verboseDb(a) 
    #define verboseDbln(a)  
#endif

#ifdef NEXTION
    #define NEXPRINT(...) Serial2.println(__VA_ARGS__)
#endif

#endif