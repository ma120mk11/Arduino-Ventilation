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
extern bool errorPending;
extern void readSensors();
extern NexTouch *nex_listen_list[];
extern bool errorPending;
static int voltageErrorCount    = 0;

bool voltageCheck();
void dismissVoltageError();

#endif