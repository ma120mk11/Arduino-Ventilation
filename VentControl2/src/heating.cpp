#include "motor_speed.h"
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

void heating(){
	// Check if heating is enabled
	if (enableHeating == 1)
	{
		if (t_Panel.value > tempUpper){
			n--;
			if(n<=0){
				motor1.setSpeed(5);
				n=autoCyckle;
			}
		}

		if (t_Panel.value < tempUpper){
			n=autoCyckle;
		}
	  
		if (t_HeatedAir.value < tempLower && motor1.getSpeed() == 5 ) {
			if(k<=0){
				motor1.setSpeed(0);
				k=autoCyckle;
			}
			else{
			k--;
			}
		}
	  
		if (t_HeatedAir.value > tempLower && motor1.getSpeed() == 5 ) {
			k=autoCyckle;
		}
	}
}