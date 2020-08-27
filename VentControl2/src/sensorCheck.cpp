#include <Arduino.h>
#include "motor_speed.h"
#include "sensor.h"
#include "Nextion.h"

// Global variables

extern float voltage;
extern float e_voltageThr;
extern bool enableHeating;
extern int voltageErrorCount;
extern bool errorPending;

extern NexTouch *nex_listen_list[];

void sensorCheck(){
	// '********************** VOLTAGE CHECK ****************************************
	// If voltage is under threshold, wait 20 min.
	if (voltage < e_voltageThr){
		
		// Wait and check again
		delay(500);
		sensorRead();
		if(voltage < e_voltageThr){
			
			m1SetSpeed(0);
			m2SetSpeed(0);
			
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