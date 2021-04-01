#include "VoltageCheck.h"
#include "ErrorHandling.h"

#define VOLTAGE_ERROR_WAITTIME 30

static int count = 0;
static bool lowVoltageFLag = false;

void dismissVoltageError(){
	count = 0;
	lowVoltageFLag = false;
}


bool voltageCheck(){
	//********************** VOLTAGE CHECK ****************************************
	//If voltage is under threshold, wait 20 min.

	if (lowVoltageFLag) {
		if (count*12 < VOLTAGE_ERROR_WAITTIME){
			count ++;
			return false;
		}
		else { // Time has lapsed
			count = 0;
			lowVoltageFLag = false;
			clearError(ERR_VOLTAGE);

			if (mode == HEATING) {			// If heating mode is active goto spring_heat page
				nextion_goToPage("spring_heating");
			}
			else {							// else go to menu
				nextion_goToPage("menu");
			}
		}
		return true;
	}
	else {
		// Low voltage and no flag is set
		if (voltage.value < e_voltageThr) {
			// Wait and check again 
			delay(1000);
			readSensors();

			if(voltage.value < e_voltageThr) {
				lowVoltageFLag = true;
				createError(ERR_VOLTAGE,"Low voltage (" + (String)voltage.value + "V).");
				
				motor1.setSpeed(0);
				motor2.setSpeed(0);

				return false;
			}
		}
		return true;
	}			
}