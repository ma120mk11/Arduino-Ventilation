#include <Arduino.h>
#include "Nextion.h"

extern int M1PWM;           
extern int M2PWM;               
extern int M1Speed;             
extern int M2Speed;           
extern bool m1Running, sendToNextion;

extern float temp0C;					  
extern float temp1C;					
extern float temp2C;					 
extern float temp3C;					
extern float temp4C;		  	
extern float voltage;

extern float v0_1;
extern float v0_2;

extern float current;			

extern float i0_1;
extern float i0_2;				

extern float filteredSignal;

extern float temp0C_max;		
extern float temp1C_max;			
extern float temp2C_max;			
extern float temp3C_max;			
extern float temp4C_max;
extern float voltage_min;			
extern float voltage_max;
extern float current_max;
extern float tempDelta_max;

extern float tempDelta;			

extern int nextionPage;			
extern int nextionMode;				

extern int voltageErrorCount;	
extern bool errorPending;

extern int nexUpload;


extern int tempUpper, tempLower;


void nextion_goToPage(String page){
    Serial2.print("page ");
    Serial2.print(page);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
}
void nextion_update(String object, float value){
    Serial2.print(object);
    Serial2.print(value);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
}	
void nextion_update(String object, int value){
    Serial2.print(object);
    Serial2.print(value);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
}
void nextion_update(String object, String message){
    Serial2.print(object);
	Serial2.print("\"");
    Serial2.print(message);
	Serial2.print("\"");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
}


void sysValUpdate(){
	if (sendToNextion == 0){
		return;
	}
	// Current motor speeds
  	nextion_update("data.M1.val=", M1Speed);
  	nextion_update("data.M2.val=", M2Speed);

	// Sensor data:
	int t0 = temp0C*10.0;
	int t1 = temp1C*10.0;
	int t2 = temp2C*10.0;
	int t3 = temp3C*10.0;
	int v = voltage*10.0;
	int i = current*10.0;
	
	int tDelta = tempDelta*10.0;
	
	nextion_update("data.T0.val=", t0);
	nextion_update("data.T1.val=", t1);
	nextion_update("data.T2.val=", t2);
	nextion_update("data.T3.val=", t3);
	nextion_update("data.V0.val=", v);
	nextion_update("data.I0.val=", i);
	nextion_update("data.tDelta.val=", tDelta);
	return;
}

void NEXsensor_maxUpdate(){
	// Updates recorded sensor max values to nextion
	
	int t0_max = temp0C_max*10;
	int t1_max = temp1C_max*10;
	int t2_max = temp2C_max*10;
	int t3_max = temp3C_max*10;
	int v_max = voltage_max*10;
	int v_min = voltage_min*10;
	int i_max = current_max*10;
	
	nextion_update("sensor_top.t0_top.val=", t0_max);
	nextion_update("sensor_top.t1_top.val=", t1_max);
	nextion_update("sensor_top.t2_top.val=", t2_max);
	nextion_update("sensor_top.t3_top.val=", t3_max);
	nextion_update("sensor_top.voltage_low.val=", v_min);
	nextion_update("sensor_top.voltage_top.val=", v_max);
	nextion_update("sensor_top.current_top.val=", i_max);
}

void NEXtempThrUpdate(){
	// Updates Upper and Lower threshold values in spring_sett1
	nextion_update("spring_sett1.n0.val=", tempUpper);
	nextion_update("spring_sett1.n1.val=", tempLower);
}

void SD_Card_Error(String message){
	nextion_update("SD_CARD_error.message.txt=", message);	
	nextion_goToPage("page SD_CARD_error");
}