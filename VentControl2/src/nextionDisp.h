#ifndef DEXTIONDISP_H
#define DEXTIONDISP_H

#include <Arduino.h>
#include "Nextion.h"
#include "sensor.h"
#include "Motor.h"
#include "settings.h"

extern bool sendToNextion;
extern Motor motor1;
extern Motor motor2;
extern DigitalTemp	t_Outside;
extern AnalogTemp 	t_Panel;
extern AnalogTemp 	t_HeatedAir;
extern AnalogTemp	t_Inside;
extern CurrentSensor current;
extern VoltageSensor voltage;

extern int light;				
//extern float tempDelta_max;
extern float tempDelta;			
extern int nextionPage;			
extern int nextionMode;				
extern int voltageErrorCount;	
extern bool enableHeating;
extern int tempUpper, tempLower;


void nextion_goToPage(String page);
void nextion_update(String object, float value);
void nextion_update(String object, int value);
void nextion_update(String object, String message);
void sysValUpdate();
void NEXsensor_maxUpdate();
void NEXtempThrUpdate();
void SD_Card_Error(String message);

#endif