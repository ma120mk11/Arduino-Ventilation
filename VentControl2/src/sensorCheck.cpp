#include <Arduino.h>
#include "motor_speed.h"
#include "sensor.h"
#include "Nextion.h"

// Global variables

extern VoltageSensor voltage;
extern Motor motor1;
extern Motor motor2;
extern float e_voltageThr;
extern bool enableHeating;
extern int voltageErrorCount;
extern bool errorPending;
extern void sensorRead();

extern NexTouch *nex_listen_list[];

void sensorCheck(){
	// '********************** VOLTAGE CHECK ****************************************
	// If voltage is under threshold, wait 20 min.
	if (voltage.value < e_voltageThr){
		
		// Wait and check again
		delay(500);
		sensorRead();
		if(voltage.value < e_voltageThr){
			
			motor1.setSpeed(0);
			motor2.setSpeed(0);
			
			voltageErrorCount++;
			errorPending = 1;
			// Send error message to nextion.
			Serial2.print("page 14");
			Serial2.write(0xff);
			Serial2.write(0xff);
			Serial2.write(0xff);
			
			int i = 0;
			while (i < 2400){
				nexLoop(nex_listen_list);
				sensorRead();
				if(errorPending==0){		// Dismiss button on nextion
					return;
				}
				if(enableHeating==0){		// Exit mode button on nextion
					return;
				}
				delay(500);					// Wait 20 min
				i++;
			}
			if (enableHeating == 1){		// If heating mode is active goto spring_heat page
				Serial2.print("page 6");
				Serial2.write(0xff);
				Serial2.write(0xff);
				Serial2.write(0xff);
			}
			else{							// else go to menu
				Serial2.print("page 0");
				Serial2.write(0xff);
				Serial2.write(0xff);
				Serial2.write(0xff);
			}
		}
	}	
}