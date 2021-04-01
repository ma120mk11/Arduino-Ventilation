#include "nextionDisp.h"

/**
 * @param page ex: "12"
 */
void nextion_goToPage(String page){
	#ifdef NEXTION
    nexSerial.print("page ");
    nexSerial.print(page);
    nexSerial.write(0xff);
    nexSerial.write(0xff);
    nexSerial.write(0xff);
	#endif
}
/**
 * @param object Ex. "data.sd_ok.val="
 * @param value Ex. 23.0
 */
void nextion_update(String object, float value){
	#ifdef NEXTION
    nexSerial.print(object);
    nexSerial.print(value);
    nexSerial.write(0xff);
    nexSerial.write(0xff);
    nexSerial.write(0xff);
	#endif
}	
void nextion_update(String object, int value){
	#ifdef NEXTION
    nexSerial.print(object);
    nexSerial.print(value);
    nexSerial.write(0xff);
    nexSerial.write(0xff);
    nexSerial.write(0xff);
	#endif
}
void nextion_update(String object, String message){
	#ifdef NEXTION
    nexSerial.print(object);
	nexSerial.print("\"");
    nexSerial.print(message);
	nexSerial.print("\"");
    nexSerial.write(0xff);
    nexSerial.write(0xff);
    nexSerial.write(0xff);
	#endif
}


void sysValUpdate(){
	#ifdef NEXTION

	// Current motor speeds
  	nextion_update("data.M1.val=", motor1.getSpeed());
  	nextion_update("data.M2.val=", motor2.getSpeed());

	// Sensor data:
	int t0 = t_Outside.getValue() 	* 10.0;
	int t1 = t_Panel.getValue() 	* 10.0;
	int t2 = t_HeatedAir.getValue() * 10.0;
	int t3 = t_Inside.getValue() 	* 10.0;
	int v  = voltage.getValue() 	* 10.0;
	int i  = current.getValue() 	* 10.0;
	int tDelta = tempDelta			* 10.0;
	
	nextion_update("data.T0.val=", t0);
	nextion_update("data.T1.val=", t1);
	nextion_update("data.T2.val=", t2);
	nextion_update("data.T3.val=", t3);
	nextion_update("data.V0.val=", v);
	nextion_update("data.I0.val=", i);
	nextion_update("data.tDelta.val=", tDelta);
	//nextion_update("data.L0.val=", light);
	nextion_update("data.mode.val=", mode);
	
	#endif
}

void NEXsensor_maxUpdate(){
	#ifdef NEXTION
	// Updates recorded sensor max values to nextion
	int t0_max = t_Outside.max	* 10;
	int t1_max = t_Panel.max 	* 10;
	int t2_max = t_HeatedAir.max* 10;
	int t3_max = t_Inside.max	* 10;
	int v_max  = voltage.max	* 10;
	int v_min  = voltage.min 	* 10;
	int i_max  = current.max 	* 10;
	
	nextion_update("sensor_top.t0_top.val=", t0_max);
	nextion_update("sensor_top.t1_top.val=", t1_max);
	nextion_update("sensor_top.t2_top.val=", t2_max);
	nextion_update("sensor_top.t3_top.val=", t3_max);
	nextion_update("sensor_top.voltage_low.val=", v_min);
	nextion_update("sensor_top.voltage_top.val=", v_max);
	nextion_update("sensor_top.current_top.val=", i_max);
	#endif
}

void NEXtempThrUpdate(){
	#ifdef NEXTION
	// Updates Upper and Lower threshold values in spring_sett1
	nextion_update("spring_sett1.n0.val=", tempUpper);
	nextion_update("spring_sett1.n1.val=", tempLower);
	#endif
}

void SD_Card_Error(String message){
	#ifdef NEXTION
	nextion_update("SD_CARD_error.message.txt=", message);	
	nextion_goToPage("page SD_CARD_error");
	#endif
}