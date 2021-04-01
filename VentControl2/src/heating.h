#ifndef HEATING_H
#define HEATING_H

#include "Motor.h"
#include "sensor.h"

extern int mode;
extern int tempUpper;
extern int tempLower;
extern Motor motor1;
extern DigitalTemp  t_Outside;
extern AnalogTemp   t_Panel;
extern AnalogTemp   t_HeatedAir;
extern Sensor       t_Delta;
void heating();

#endif