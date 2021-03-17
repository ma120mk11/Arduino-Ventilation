#include <Arduino.h>
#include "nextionDisp.h"
//#include "settings.h"

class Motor {
	private:
		int speed;
		int PWM;
		int dly = 30;            // How fast the motor goes from one value to another
		int output_pin = 9;
		int max = 0;
	public:
		Motor();
		Motor(int pin) { output_pin = pin;}    
		void setPin(int pin) { output_pin = pin; }
		int getSpeed() { return speed; }
		int getMax(){ return max; }
		bool isRunning = 0;

		// PWM SETTINGS :
		static const int s1 = 70;				// motor PWM value at setting 1
		static const int s2 = 100;				// motor PWM value at setting 2
		static const int s3 = 150;				// motor PWM value at setting 3
		static const int s4 = 210;				// motor PWM value at setting 4
		static const int s5 = 255;				// motor PWM value at setting 5

		void setSpeed(int s)
		{
			if ( s == 5 ){
				speed = 5;                    // Update global variable
				isRunning = 1;					// Update global variable	
				sysValUpdate();                  // Updates Display
				
				while( PWM < s5){             
					PWM = PWM + 5;
					analogWrite(output_pin, PWM);
					delay(dly);                  // optional delay
				}  
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
};
