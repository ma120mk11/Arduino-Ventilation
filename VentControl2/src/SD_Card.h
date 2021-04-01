#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include <SD.h>
#include <RTClib.h>
#include "nextionDisp.h"
#include "settings.h"
#include "Motor.h"
#include "sensor.h"
#include "ErrorHandling.h"

/***********************************************************************
https://lastminuteengineers.com/arduino-micro-sd-card-module-tutorial/

Pinout:			MOSI	MISO	SCK		CS (chipSelect)
Arduino Mega	51		50		52		53
Arduino Uno		11		12		13		10
Arduino Nano	11		12		13		10

*************************************************************************/

void SD_Card_INIT();
void SD_log(String date, String time);
void SD_errorlog(int ErrorType, String msg, int count, String date = "", String time = "");
void SD_unmount();

#endif