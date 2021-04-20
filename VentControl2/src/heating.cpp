#include "heating.h"
#include "settings.h"
#include "VoltageCheck.h"
#include "ErrorHandling.h"

int stepsUntilOn = HYSTERESIS;
int stepsUntilOff = HYSTERESIS;
extern DateTime now;

void heating() {
	// The ADVANCED mode uses the RTC. If the RTC fails or is not present activate HEATING mode instead
	if (isActiveErrorType(ERR_RTC) && mode == ADVANCED) {
		mode = HEATING;
		verboseDbln("MODE switched to HEATING (RTC is not working)");
	}

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


	/**
	 * If it is very sunny, heat up the panel to 45 degrees before starting.
	 *  - This is indicated by the t_Delta slope > ??.
	 *  - Start with motor speed 5
	 * If it is not very sunny:
	 * - Start motor with speed 4
	 * 
	 * Loop:
	 * - Wait 5 min
	 * - Check slope of heated air
	 * 		if temp is increasing -> mode++
	 * 		if temp is decreasing -> mode--
	 * 
	 * If time is 12:00 and the motor is running
	 * - Speed-- (min 3)
	 * - Run until (heated air - inside) < 5
	 * 
	 * If time is after 13:00
	 *  - Use lower start threshold and speed
	 *  
	*/

	if (mode == ADVANCED)
	{
		// t_Delta.getSlope();
		if (voltageCheck()) {
			



		}
	}
}