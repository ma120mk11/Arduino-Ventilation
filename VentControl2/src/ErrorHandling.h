#ifndef ERRORHANDLING_H
#define ERRORHANDLING_H

#include <Arduino.h>
#include "settings.h"
#include "nextionDisp.h"
#include "SD_Card.h"

// ****************  ERRORS  *********************
#define ERR_VOLTAGE 1    // Low voltage
#define ERR_CURRENT 2   
#define ERR_MOTOR 3     // A motor didn't start (no current detected)

#define ERR_WFIFI 5     // Wifi 
#define ERR_SD 6
#define ERR_RTC 7
#define ERR_NEXTION 8
#define ERR_TEMP_OUTSIDE 9
#define ERR_TEMP_PANEL 10
#define ERR_TEMP_AIR 11
#define ERR_TEMP_INSIDE 12
#define ERR_THINKSPEAK 13

extern DS1302 rtc;

/**
 * Generates an error if the internal errorflag is not set.
 * @param ErrorType Starts with ERR_...
 * @param msg Optional error description
 */
void createError(int ErrorType, String msg = "");

/**
 * Resets the error flag and counter.
 */
void clearError(int ErrorType);

String getErrors();

#endif