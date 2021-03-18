#ifndef SENSORCHECK_H
#define SERSORCHECK_H

#include <Arduino.h>
#include "Motor.h"
#include "sensor.h"
#include "Nextion.h"

extern VoltageSensor voltage;
extern Motor motor1;
extern Motor motor2;
extern float e_voltageThr;
extern bool enableHeating;
extern int voltageErrorCount;
extern bool errorPending;
extern void sensorRead();

extern NexTouch *nex_listen_list[];


void sensorCheck();


#endif