#ifndef VOLTAGECHECK_H
#define VOLTAGECHECK_H

#include <Arduino.h>
#include "Motor.h"
#include "sensor.h"
#include "Nextion.h"
#include "settings.h"
#include "nextionDisp.h"

extern VoltageSensor voltage;
extern Motor motor1;
extern Motor motor2;
extern float e_voltageThr;
extern int mode;


bool voltageCheck();
void dismissVoltageError();

#endif