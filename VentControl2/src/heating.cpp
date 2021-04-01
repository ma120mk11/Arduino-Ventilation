#include "heating.h"
#include "settings.h"
#include "VoltageCheck.h"

int stepsUntilOn = HYSTERESIS;
int stepsUntilOff = HYSTERESIS;

void heating() {
	// Check if heating is enabled
	if (mode == HEATING)
	{
		if (voltageCheck()) {
			// Check if we should turn motor on
			if (t_Panel.value > tempUpper){
				stepsUntilOn--;
				if(stepsUntilOn <= 0) {
					motor1.setSpeed(5);
					stepsUntilOn = HYSTERESIS;
				}
			}

			if (t_Panel.value < tempUpper){
				stepsUntilOn = HYSTERESIS;
			}

			// Turn off
			if (t_HeatedAir.value < tempLower && motor1.getSpeed() == 5 ) {
				if (stepsUntilOff <= 0) {
					motor1.setSpeed(0);
					stepsUntilOff = HYSTERESIS;
				}
				else {
					stepsUntilOff--;
				}
			}
			
			if (t_HeatedAir.value > tempLower && motor1.getSpeed() == 5 ) {
				stepsUntilOff = HYSTERESIS;
			}
		}
	}

	if (mode == ADVANCED) 
	{
		/**
		 * 
		 *  
		*/
		t_Delta.getSlope();

	}
}