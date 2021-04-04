#include "Motor.h"

#include <Arduino.h>
#include "nextionDisp.h"
#include "ErrorHandling.h"

Motor::Motor(int pin) { 
	output_pin = pin;
}    

void Motor::setPin(int pin) { output_pin = pin; }

int Motor::getSpeed() 	{ return speed; }
int Motor::getMax()		{ return max; }

void Motor::setSpeed(int s)
{
	// Update motor start time
	if(s > 0 && !isRunning) { startMillis = millis(); }
	// Update max
	if(s > max) max = s;

	verboseDbln("Motor on pin " + (String)output_pin + " set to " + s);

	if ( s == 5 ){
		speed = 5;                    // Update global variable
		isRunning = 1;					// Update global variable	
		sysValUpdate();                  // Updates Display
		
		while( PWM < s5){             
			PWM = PWM + 5;
			analogWrite(output_pin, PWM);
			delay(dly);                  // optional delay
		}
		current.read();

		// Check that motor starts
		if(current.getValue() > noCurrentThr) createError(ERR_MOTOR);

		return;   
	} // end of 5


	if ( s == 4 ){
		speed = 4;
		isRunning = 1;					// Update global variable		
		sysValUpdate();
		if(PWM > s4){                 // If current motor speed is higher, slow down
			while( PWM > s4 ){
				PWM=PWM-5;              // decrease speed by 5
				analogWrite(output_pin, PWM);
				delay(dly);                 //optional delay
			}
		}
		if(PWM < s4){                // if current speed is lower, speed up
			while( PWM < s4){
				PWM=PWM+5;              // increase speed by 5
				analogWrite(output_pin, PWM);
				delay(dly);                 //optional delay
			}
		}

		// Check that motor starts
		if(current.getValue() > noCurrentThr) createError(ERR_MOTOR);
		return;
	} // end of 4


	if ( s == 3 ){
		speed = 3;
		isRunning = 1;					// Update global variable		
		sysValUpdate();
		if(PWM > s3){                 // If current motor speed is higher, slow down
			while( PWM > s3 ){
				PWM=PWM-5;              // decrease speed by 5
				analogWrite(output_pin, PWM);
				delay(dly);                  //optional delay
			}
		}
		if(PWM < s3){                // if current speed is lower, speed up
			while( PWM < s3){
				PWM=PWM+5;              // increase speed by 5
				analogWrite(output_pin, PWM);
				delay(dly);                  //optional delay
			}
		}
		// Check that motor starts
		if(current.getValue() > noCurrentThr) createError(ERR_MOTOR);
		return;  
	} // end of 3


	if ( s == 2 ){
		speed = 2;
		isRunning = 1;					// Update global variable		  
		sysValUpdate();
		if(PWM > s2){                 // If current motor speed is higher, slow down
			while( PWM > s2 ){
				PWM=PWM-5;              // decrease speed by 5
				analogWrite(output_pin, PWM);
				delay(dly);                 //optional delay
			}
		}
		if(PWM < s2){                // if current speed is lower, speed up
			while( PWM < s2){
				PWM=PWM+5;              // increase speed by 5
				analogWrite(output_pin, PWM);
				delay(dly);                 //optional delay
			}
		}
		// Check that motor starts
		if(current.getValue() > noCurrentThr) createError(ERR_MOTOR);		
		return;   
	} // end of 2


	if ( s == 1 ){
		speed = 1;
		isRunning = 1;					// Update global variable	
		sysValUpdate();
		if(PWM > s1){                 // If current motor speed is higher, slow down
			while( PWM > s1 ){
				PWM = PWM - 5;              // decrease speed by 5
				analogWrite(output_pin, PWM);
				delay(dly);                  //optional delay
			}
		}
		if(PWM < s1){                // if current speed is lower, speed up
			while( PWM < s1){
				PWM=PWM+5;              // increase speed by 5
				analogWrite(output_pin, PWM);
				delay(dly);                 //optional delay
			}
		}
		// Check that motor starts
		if(current.getValue() > noCurrentThr) createError(ERR_MOTOR);		
		return;
	} // end of 1


	if ( s == 0 ){
		speed = 0;                
		isRunning = 0;						
		sysValUpdate();
			while( PWM > 15){            // Value after it goes to zero  
				PWM = PWM - 5;
				analogWrite(output_pin, PWM);
				delay(dly);                      // optional delay
			}
			PWM=0;
			analogWrite(output_pin, PWM);
		return;   
	}
}

int Motor::getTimeOn() {
	int timeOn = 0;
	if (isRunning) timeOn = (millis() - startMillis) / 60000;
	return timeOn;
}