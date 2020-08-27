#include "motor_speed.h"

extern bool enableHeating;
extern float temp1C;
extern float temp2C;
extern int tempUpper;
extern int tempLower;
extern int M1Speed;
extern int autoCyckle;
extern int n;
extern int k;

void heating(){
	if (enableHeating == 1)
	{
		if (temp1C > tempUpper){
			n--;
			if(n<=0){
				m1SetSpeed(5);
				n=autoCyckle;
			}
		}

		if (temp1C < tempUpper){
			n=autoCyckle;
		}
	  
		if (temp2C < tempLower && M1Speed==5){
			if(k<=0){
				m1SetSpeed(0);
				k=autoCyckle;
			}
			else{
			k--;
			}
		}
	  
		if (temp2C > tempLower && M1Speed==5){
			k=autoCyckle;
		}
	}
}