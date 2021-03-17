#ifndef HEATING_H
#define HEATING_H

#include "Motor.h"
#include "sensor.h"

extern bool enableHeating;
extern int tempUpper;
extern int tempLower;
extern int autoCyckle;
extern int n;
extern int k;
extern Motor motor1;
extern DigitalTemp t_Outside;
extern AnalogTemp t_Panel;
extern AnalogTemp t_HeatedAir;

void heating();

#endif