#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include <SD.h>
#include <RTClib.h>
#include "nextionDisp.h"
#include "settings.h"
#include "Motor.h"
#include "sensor.h"

/***********************************************************************
https://lastminuteengineers.com/arduino-micro-sd-card-module-tutorial/

Pinout:			MOSI	MISO	SCK		CS (chipSelect)
Arduino Mega	51		50		52		53
Arduino Uno		11		12		13		10
Arduino Nano	11		12		13		10

*************************************************************************/

// extern Motor motor1;
// extern DigitalTemp	t_Outside;
// extern AnalogTemp 	t_Panel;
// extern AnalogTemp 	t_HeatedAir;
// extern AnalogTemp	t_Inside;
// extern CurrentSensor current;
// extern VoltageSensor voltage;

//bool sd_errorFlag;
//bool unmountedFlag;

void SD_Card_INIT();
void SD_log(String date, String time);
void SD_unmount();

#endif