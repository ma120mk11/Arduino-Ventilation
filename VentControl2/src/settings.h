#ifndef SETTINGS_H
#define SETTINGS_H

// Enable features
#define DEVELOPMENT
#define DEBUG
#define VERBOSE_DB
#define PRINT_ERRORS
// #define NEXTION
#define THINGSPEAK 
#define EEPROM_STORE 
//#define TELEMETRY 
// #define OFFLINE_TEST

//************************    PINS    *********************************
// USE TX2 for Nextion display communication
/**
 * RESERVED PINS:
 * 52 - SPI (When using SD Card, must be left as an output if other SS (ChipSelect) pin us used)
 */

#define MOTOR1 12            // motor1 pin HEATED AIR
#define MOTOR2 13			 // motor2 pin DIRECT AIR

//Use a 10k resistor between signal and 5V
#define DS18B20_BUS 28      // DS18B20 digital temperature sensor bus
#define T_PANEL     A1   	// LM35 temp sensor1 pin  	PANEL TEMP
#define T_AIR       A2   	// LM35 temp sensor2 pin	HOT AIR
#define CURRENT     A5      // Current sensor pin
#define VOLTAGE     A8      // Voltage sensor pin
#define LightSensorPin A9   // Light sensor pin

// Select which SD card reader is used:
#define CYTRON_SD
// #define EXTERNAL_SD

#ifdef EXTERNAL_SD
    #define chipSelect 53		// pin for SD card chip select (see SD_card.h)
#endif
#ifdef CYTRON_SD
    #define chipSelect 4        // pin for SD card on Wifi shield
#endif

// pins for DS1302 RTC Module
// #define RTC_RST 8
// #define RTC_IO 7
// #define RTC_SCL 6
#define RTC_RST 30
#define RTC_IO 31
#define RTC_SCL 32

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
#define TEMP_ERR_THR 75

// *****************  THINGSPEAK **********************
#ifdef DEVELOPMENT
    #define CHANNEL_ID 1333797
    #define CHANNEL_API_KEY "FWUUPNPX96UG2NN9"
#else
    #define CHANNEL_ID 1327374
    #define CHANNEL_API_KEY "SYJB15HBTE9V8CWR"
#endif

// *****************  MODES **********************
#define MANUAL 0
#define HEATING 1
#define ADVANCED 2

#define MOTOR_TIMEOUT 60    // In manual mode, how long before motor stops (minutes)

// ****************  LOOP FUNCTION *********************
#define ONE_SEC 1000
#define THREE_SEC 3000
#define FIVE_SEC 5000
#define ONE_MIN 60000
#define ONE_DAY 86400000

#ifdef PRINT_ERRORS
    #define errorPrint(a) Serial.println(a)
#else
    #define errorPrint(a)
#endif

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