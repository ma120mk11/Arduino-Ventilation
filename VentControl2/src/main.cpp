#include <Arduino.h>
#include <EEPROM.h>
#include "Nextion.h"
#include "nextionDisp.h"
#include "SD_Card.h"
#include "sensor.h"
#include "Motor.h"
#include "heating.h"
#include "settings.h"
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <ThingSpeak.h>
#include <CytronWiFiShield.h>
#include <CytronWiFiClient.h>


Motor motor1(MOTOR1);				// Heated air
Motor motor2(MOTOR2);				// Cool air
DigitalTemp	t_Outside(T_OUTSIDE);
AnalogTemp 	t_Panel(T_PANEL);
AnalogTemp 	t_HeatedAir(T_AIR);
AnalogTemp	t_Inside(T_LIVINGROOM);
CurrentSensor current(CURRENT);		
VoltageSensor voltage(VOLTAGE);

const char *ssid = "HUAWEI P20 Pro";
const char *pass = "3665d14cd73b";

ESP8266Client client;					// WIFI client

DS1302 rtc(RTC_RST, RTC_SCL, RTC_IO);	// Initialize rtc object

OneWire oneWirePin(outside_temp_pin);
DallasTemperature sensors(&oneWirePin);

//***********************  SETTINGS  ***************************
//bool enableSerialPrint = 0;		// Turn on/off printing sensor values to serial 1
//bool sendToNextion 	= 1;		// Turn on/off sending sensor values to nextion (serial 2)
bool allow2motors 	= 0;		// Nextion setting, set default value here
bool enableHeating 	= 1;		// Enable AUTO setting for motor 1
int tempUpper = 40;				// At which temperature to start motor
int tempLower = 30;				// At which temp to stop motor

struct eepromVariables {
	int tempUpper;
	int tempLower;
	float e_voltageThr;
};
			
//********************** ERROR SETTINGS **************************
float e_voltageThr = 10.7;    	// If voltage goes under this value, send error message

// **********************  VARIABLES  ****************************
float derivate;					// The derivate of panel temperature
float tempDelta;				// How much the temperature is increased when flowing through panel

int voltageErrorCount = 0;		// 
bool errorPending = 0;			//

int nextionPage;				// the page that is currently active
int nextionMode;				// the active mode
float temperature; // for debug
int prevDay;
unsigned long loop_timer_1s;
unsigned long loop_timer_3s;
unsigned long loop_timer_5s;
unsigned long loop_timer_1min;

void RtcInit() {
	DBPRINT("Initializing RTC...");
	rtc.begin();
	if(!rtc.isrunning()) {
		rtc.adjust(DateTime(__DATE__,__TIME__));
		DateTime now = rtc.now();
		String date = String(now.day()) + "." + String(now.month()) + "." + String(now.year());
		String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
		if(date == "165.165.2165"){
			DBPRINT_LN("ERROR");
		}
		else verboseDbln("adjusted to " + date + "  " + time);
	} else DBPRINT_LN("DONE");
}

void WifiInit() {
	DBPRINT("Initializing Wifi module...");
	if(!wifi.begin(10,11)){
		DBPRINT_LN(F("cannot connect to WIFI module."));
	}
	if(!wifi.connectAP(ssid, pass))
  	{
    	DBPRINT_LN(F("Error connecting to WiFi"));
  	}
	else{ DBPRINT_LN("DONE"); }
}

void storeEEPROM() {
	#ifdef EEPROM_STORE
		DBPRINT("Storing in EEPROM...");
		eepromVariables eeStore;
		eeStore.e_voltageThr = e_voltageThr;
		eeStore.tempLower = tempLower;
		eeStore.tempUpper = tempUpper;
		int eeAddress = 0;
		EEPROM.put(eeAddress, eeStore);

		DBPRINT_LN("DONE");
	#endif
}

void readSensors() {
	t_Outside.read();
	t_Panel.read();
	t_HeatedAir.read();
	t_Inside.read();
	current.read();
	voltage.read();

	tempDelta = t_HeatedAir.value - t_Outside.value;
}

#pragma region nextion
//#########################         NEXTION               #############################
// Declare Nextion objects
/* Types of objects:
	* NexButton - Button
	* NexDSButton - Dual-state Button
	* NexHotspot - Hotspot, that is like an invisible button
	* NexCheckbox - Checkbox
	* NexRadio - "Radio" checkbox, that it's exactly like the checkbox but with a rounded shape
	* NexSlider - Slider
	* NexGauge - Gauge
	* NexProgressBar - Progress Bar
	* NexText - Text box
	* NexScrolltext - Scroll text box
	* NexNumber - Number box
	* NexVariable - Variable inside the nextion display
	* NexPage - Page touch event
	* NexRtc - To use the real time clock for Enhanced Nextion displays
*/
// PAGE 0 - menu
// MENU - Page 0
	NexButton springHeat = NexButton(0,4,"springHeat");
// Set Time
	NexButton setTime = NexButton(3,17,"setTime");
//  MANUAL MOTOR 1 - Page 4
	NexButton bMS1 = NexButton(4,1,"bMS1");             // Button 1
	NexButton bMS2 = NexButton(4,2,"bMS2");             // Button 2
	NexButton bMS3 = NexButton(4,3,"bMS3");             // Button 3
	NexButton bMS4 = NexButton(4,4,"bMS4");             // Button 4
	NexButton bMS5 = NexButton(4,5,"bMS5");             // Button 5
	NexButton bMS0 = NexButton(4,6,"bMS0");             // Button OFF
	NexButton bM2  = NexButton(4,12,"bM2");				// Change to motor 2 (page 5)
//  MANUAL MOTOR 2 - Page 5
	NexButton bM2S1 = NexButton(5,1,"bM2S1");           // Button 1
	NexButton bM2S2 = NexButton(5,2,"bM2S2");           // Button 2
	NexButton bM2S3 = NexButton(5,3,"bM2S3");           // Button 3
	NexButton bM2S4 = NexButton(5,4,"bM2S4");           // Button 4
	NexButton bM2S5 = NexButton(5,5,"bM2S5");           // Button 5
	NexButton bM2S0 = NexButton(5,6,"bM2S0");           // Button OFF
	NexButton bM1	= NexButton(5,11,"bM1");			// Change to motor 1 (page 4)
// Spring Heating MODE
	NexButton springExit = NexButton(6,1,"springExit");	// Exit spring mode
//	spring_sett1 - SPRING SETTINGS
	NexButton Dec_Utemp = NexButton(7,5,"Dec_Utemp");	// Upper temp DECREASE
	NexButton Inc_Utemp = NexButton(7,7,"Inc_Utemp");	// Upper temp INCREASE
	NexButton Dec_Ltemp = NexButton(7,6,"Dec_Ltemp");
	NexButton Inc_Ltemp = NexButton(7,8,"Inc_Ltemp");
// 9 - Sensor_data
	NexButton Measure = NexButton(9,22,"Measure");
// 10 - sensor_top
	NexButton Reset = NexButton(10,22,"Reset");			// Reset top_values
	NexButton Update = NexButton(10,25,"Update");
// 12 - disp_settings
	NexButton eeStore = NexButton(12,21,"eeStore");
// 14 Voltage Error
	NexButton v_err_exit = NexButton(14,6,"v_err_exit");// Exit mode
	NexButton ignore = NexButton(14,3,"ignore");		// Ignore error	
// 15 - Settings 2
	NexButton v_errDec = NexButton(15,3,"v_errDec");
	NexButton v_errInc = NexButton(15,4,"v_errInc");
// 21 - sd_card_sett
	NexButton sd_unmount = NexButton(21,4,"sd_unmount");
	NexButton sd_init = NexButton(21,5,"sd_init");


// ************* Register button objects to the touch event list. *****************
NexTouch *nex_listen_list[] = {
// Page 0 menu
	&springHeat,
// Page 1
// Page 2
// Page 3
	&setTime,
// Manual Page 4
	&bMS1,
	&bMS2,
	&bMS3,
	&bMS4,
	&bMS5,
	&bMS0,
	&bM2,
// Manual Page 5
	&bM2S1,
	&bM2S2,
	&bM2S3,
	&bM2S4,
	&bM2S5,
	&bM2S0,
	&bM1,
// spring mode page 6
	&springExit,
// spring_sett1
	&Dec_Utemp,
	&Inc_Utemp,
	&Dec_Ltemp,
	&Inc_Ltemp,
// Settings Page 10
// display_settings Page 12
	&eeStore,
// sensor_data
	&Measure,
// sensor_top
	&Reset,
	&Update,
// 14 Voltage Error
	&v_err_exit,
	&ignore,
// 15 - settings 2
	&v_errDec,
	&v_errInc,
// 21 sd_card_sett
	&sd_unmount,
	&sd_init,	
NULL
};

//*****************************    DISPLAY BUTTON FUNCTIONS   ********************************************

// ********** MENU **********
void springHeatPopCallback(void *ptr){
	enableHeating = 1;
	DBPRINT("Enabled Heating on display. enableHeating = ");
	DBPRINT_LN(enableHeating);
}

void setTimePopCallback(void *ptr){
	/**
	 * TODO:
	 * Get the values that are set on the display and update RTC.
	 */
	
	
	DateTime now = rtc.now();
	// Update nextion clock:
	nextion_update("rtc2=", now.day());
	nextion_update("rtc1=", now.month());
	// TODO //nextion_update("rtc0.val=", now.year());
	nextion_update("rtc3=", now.hour());
	nextion_update("rtc4=", now.minute());
}

// ************MANUAL - page 4 **************
// 1-button release function
	void bMS1PopCallback(void *ptr){
		motor1.setSpeed(1);                		// Sets motor speed
		dbSerialPrintln("Page4-Button-1");
	}	
// 2-button release function
	void bMS2PopCallback(void *ptr){
		motor1.setSpeed(2);
	}
// 3-button release function
	void bMS3PopCallback(void *ptr){
		motor1.setSpeed(3); 
	}
// 4-button release function
	void bMS4PopCallback(void *ptr){
		motor1.setSpeed(4);  
	}
// 5-button release function
	void bMS5PopCallback(void *ptr){
		motor1.setSpeed(5);
	}
// OFF-button release function
	void bMS0PopCallback(void *ptr){
		motor1.setSpeed(0); 
	}
// Motor 2 button
	void bM2PopCallback(void *ptr){
		if(allow2motors == 0){
			motor1.setSpeed(0);
		}	
	}
// ************   MANUAL Motor 2 - page 5 **************

// 1-button release function
	void bM2S1PopCallback(void *ptr){
		motor2.setSpeed(1);                	// Sets motor speed
		dbSerialPrintln("Page5-Button-1");
	}
// 2-button release function
	void bM2S2PopCallback(void *ptr){
		motor2.setSpeed(2);
	}
// 3-button release function
	void bM2S3PopCallback(void *ptr){
		motor2.setSpeed(3); 
	}
// 4-button release function
	void bM2S4PopCallback(void *ptr){
		motor2.setSpeed(4);  
	}
// 5-button release function
	void bM2S5PopCallback(void *ptr){
		motor2.setSpeed(5);
	}
// OFF-button release function
	void bM2S0PopCallback(void *ptr){
		motor2.setSpeed(0); 
	}
// Motor 1 button
	void bM1PopCallback(void *ptr){		// When switching page, check if both can be on at the same time, if not -> off
		if(allow2motors == 0){
			motor2.setSpeed(0);
		}
	}
// PAGE 6 SPRING HEATING
	void springExitPopCallback(void *ptr){
		enableHeating = 0;
		motor1.setSpeed(0);
	}
// Upper temp decrease
	void Dec_UtempPopCallback(void *ptr){
		tempUpper --;
		NEXtempThrUpdate();
	}
// Upper temp increase
	void Inc_UtempPopCallback(void *ptr){
		tempUpper ++;
		NEXtempThrUpdate();
	}
// Lower temp decrease
	void Dec_LtempPopCallback(void *ptr){
		tempLower --;
		NEXtempThrUpdate();
	}
// Lower temp increase
	void Inc_LtempPopCallback(void *ptr){
		tempLower ++;
		NEXtempThrUpdate();
	}
// Top values reset button
	void ResetPopCallback(void *ptr){
		t_Outside.resetMinMax();
		t_Panel.resetMinMax();
		t_HeatedAir.resetMinMax();
		t_Inside.resetMinMax();
		voltage.resetMinMax();
		current.resetMinMax();

		NEXsensor_maxUpdate();
	}
// Top values update button
	void UpdatePopCallback(void *ptr){
		NEXsensor_maxUpdate();
	}
// Update sensors
	void MeasurePopCallback(void *ptr){
		readSensors();
		sysValUpdate();
	}
// 12
	void eeStorePopCallBack(void *ptr) {
		storeEEPROM();
	}
// 14 Voltage error
	void v_err_exitPopCallback(void *ptr){
		errorPending=0;
		enableHeating=0;					// Turn off heating
	}
	void ignorePopCallback(void *ptr){
		errorPending=0;
	}
// 15 - settings 2
	void v_errDecPopCallback(void *ptr){
		e_voltageThr -= 0.1;
		int thr = e_voltageThr*10;
		nextion_update("settings_2.v_err.val=", thr);
	}
	void v_errIncPopCallback(void *ptr){
		e_voltageThr += 0.1;
		int thr = e_voltageThr*10;
		nextion_update("settings_2.v_err.val=", thr);
	}
// page 21 sd_card_sett
	void sd_unmountPopCallback(void *ptr){
		SD_unmount();
	}
	void sd_initPopCallback(void *ptr){
		SD_Card_INIT();
	}
#pragma endregion nextion


//**********************************************   SETUP     ********************************************//
void setup() {													
	Serial.begin(9600);
	Serial2.begin(9600);
	delay(400);
	DBPRINT_LN("Starting Setup...");
	// sensors.begin();
	DigitalTemp::tempInit();

	#ifdef EEPROM_STORE
		DBPRINT("Reading EEPROM...");
		eepromVariables eeRead;
		int eeAddress = 0;
		EEPROM.get(eeAddress, eeRead);
		e_voltageThr = eeRead.e_voltageThr;
		tempUpper = eeRead.tempUpper;
		tempLower = eeRead.tempLower;

		// // Check for errors
		// if (int1 > 0 && int1 < 80 && int2 > 0 && int2 < 80 && float1 > 8 && float1 < 20) {
		// 	tempLower = int1;
		// 	tempUpper = int2;
		// 	e_voltageThr = float1;
			DBPRINT_LN("DONE");
		// }
		// else { DBPRINT_LN("ERROR - using deafult values."); }
	#endif
	#ifdef NEXTION
		DBPRINT("Initializing nextion display...");
		bool isInitialized = nexInit();					// Initialize nextion display
		nextion_goToPage("ardu_restart");				// Notify the display that arduino was restarted
		nextion_update("settings_2.v_err.val=", e_voltageThr*10);
		if(isInitialized)DBPRINT_LN("DONE");
		else DBPRINT_LN("ERROR");
	#endif
	RtcInit();
	pinMode(MOTOR1, OUTPUT);           	// set motor as output
	pinMode(MOTOR2, OUTPUT);           	// set motor as output
	readSensors();
	// SD_Card_INIT();

	#ifdef THINGSPEAK
		WifiInit();
		ThingSpeak.begin(client);	
	#endif
	

	#pragma region nextion_setup
	springHeat.attachPop(springHeatPopCallback, &springHeat); 	// MENU
	setTime.attachPop(setTimePopCallback, &setTime);  			// Page 3
	bMS1.attachPop(bMS1PopCallback, &bMS1);       				// Page 4
	bMS2.attachPop(bMS2PopCallback, &bMS2);
	bMS3.attachPop(bMS3PopCallback, &bMS3);
	bMS4.attachPop(bMS4PopCallback, &bMS4);
	bMS5.attachPop(bMS5PopCallback, &bMS5);
	bMS0.attachPop(bMS0PopCallback, &bMS0);
	bM2.attachPop(bM2PopCallback, &bM2);
	bM2S1.attachPop(bM2S1PopCallback, &bM2S1);      			// Page 5
	bM2S2.attachPop(bM2S2PopCallback, &bM2S2);
	bM2S3.attachPop(bM2S3PopCallback, &bM2S3);
	bM2S4.attachPop(bM2S4PopCallback, &bM2S4);
	bM2S5.attachPop(bM2S5PopCallback, &bM2S5);
	bM2S0.attachPop(bM2S0PopCallback, &bM2S0);
	bM1.attachPop(bM1PopCallback, &bM1);
	springExit.attachPop(springExitPopCallback, &springExit);   // PAGE 6 SPRING
	Dec_Utemp.attachPop(Dec_UtempPopCallback, &Dec_Utemp);
	Inc_Utemp.attachPop(Inc_UtempPopCallback, &Inc_Utemp);
	Dec_Ltemp.attachPop(Dec_LtempPopCallback, &Dec_Ltemp);
	Inc_Ltemp.attachPop(Inc_LtempPopCallback, &Inc_Ltemp);
	Measure.attachPop(MeasurePopCallback, &Measure);
	Reset.attachPop(ResetPopCallback, &Reset);
	Update.attachPop(UpdatePopCallback, &Update);
	v_err_exit.attachPop(v_err_exitPopCallback, & v_err_exit);
	eeStore.attachPop(eeStorePopCallBack, &eeStore);
	ignore.attachPop(ignorePopCallback, &ignore);
	v_errDec.attachPop(v_errDecPopCallback, &v_errDec);     	// page 15 - settings 2
	v_errInc.attachPop(v_errIncPopCallback, &v_errInc);
	sd_init.attachPop(sd_initPopCallback, &sd_init);			// Page 21
	sd_unmount.attachPop(sd_unmountPopCallback, &sd_unmount);
	#pragma endregion nextion_setup

	// Update nextion start values
	NEXtempThrUpdate();
	NEXsensor_maxUpdate();

	DBPRINT_LN("Setup Done.");
} // END OF SETUP


//**********************************************   MAIN     ********************************************//
void loop() {
	/**
	* TODO:
	* - Periodically run the motor for a specified time automatically. 
	* - What happens if RTC card fails?
	*/
	//DateTime now = rtc.now();

	// Every cycle:
	#ifdef NEXTION
		nexLoop(nex_listen_list);
	#endif


	// Once every second:
	if((millis() - loop_timer_1s) > ONE_SEC){
		

		loop_timer_1s = millis();		// Reset timer
	}
	
	// Once every 5 seconds
	if((millis() - loop_timer_5s) > FIVE_SEC){
		readSensors();
		heating();
		sysValUpdate();

			
	
		String text = (String)t_Outside.getValue() + '\t' + t_Outside.getSlope() + " C/min";
		verboseDbln(text);



		#ifdef TELEMETRY
			//  Message is formatted for use with Telemetry viewer. Use a layout2 in Telemetry folder.
			// TODO: add light sensor
			
			char outside_text[30];
			char panel_text[30];
			char air_text[30];
			char room_text[30];
			char filtered_text[30];
			char voltage_text[30];
			char current_text[30];
			char tdelta_text[30];
			char m1speed_text[30];

			dtostrf(t_Outside.getValue(), 10, 10, outside_text);
			dtostrf(t_Panel.getValue(), 10, 10, panel_text);
			dtostrf(t_HeatedAir.getValue(), 10, 10, air_text);
			dtostrf(t_Inside.getValue(), 10, 10, room_text);
			dtostrf(0, 10, 10, filtered_text);
			dtostrf(voltage.getValue(), 10, 10, voltage_text);
			dtostrf(current.getValue(), 10, 10, current_text);
			dtostrf(tempDelta, 10, 10, tdelta_text);
			dtostrf(motor1.getSpeed(), 10, 10, m1speed_text);

			char text[280];
			snprintf(text, 280, "%s,%s,%s,%s,%s,%s,%s,%s,%s", outside_text, panel_text, air_text, room_text, voltage_text, current_text,filtered_text,tdelta_text,m1speed_text);
			Serial.println(text);
		#endif

		loop_timer_5s = millis();		// Reset timer
	}

	// once every minute
	if(millis() - loop_timer_1min > ONE_MIN){
		// String date = String(now.day()) + "." + String(now.month()) + "." + String(now.year());
		// String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

		// SD_log(date, time);
		
		#ifdef THINGSPEAK
			verboseDb("Sending to Thingspeak...");
			ThingSpeak.setField(1,t_Outside.getValue());
			ThingSpeak.setField(2,t_Panel.getValue());
			ThingSpeak.setField(3,t_HeatedAir.getValue());
			ThingSpeak.setField(4,t_Inside.getValue());
			ThingSpeak.setField(5,voltage.getValue());
			ThingSpeak.setField(6,current.getValue());
			ThingSpeak.setField(7,motor1.getSpeed());
			ThingSpeak.setField(8,t_Outside.getValue());	// TODO
			int status = ThingSpeak.writeFields(CHANNEL_ID,CHANNEL_API_KEY);
			// Print error to nextion and serial
			if(status != 200){
				verboseDb("ERROR - ");
				// 200 - successful.
				// 404 - Incorrect API key (or invalid ThingSpeak server address)
				// -101 - Value is out of range or string is too long (> 255 characters)
				// -201 - Invalid field number specified
				// -210 - setField() was not called before writeFields()
				// -301 - Failed to connect to ThingSpeak
				// -302 - Unexpected failure during write to ThingSpeak
				// -303 - Unable to parse response
				// -304 - Timeout waiting for server to respond
				// -401 - Point was not inserted (most probable cause is the rate limit of once every 15 seconds)
				switch (status)
				{
				case 404:
					verboseDbln("incorrect API key");
					nextion_update("wifi_error.message.txt", "404 Thingspeak-incorrect API key.");
					break;
				case -101:
					verboseDbln("value out of range or string is too long");
					nextion_update("wifi_error.message.txt", "- 101 Thingspeak-value out of range.");
					break;
				case -210:
					verboseDbln("setField() was not called before writeFields()");
					nextion_update("wifi_error.message.txt", "-210 Error");
					break;
				case -301:
					verboseDbln("Failed to connect to ThingSpeak");
					nextion_update("wifi_error.message.txt", "-301 Failed to connect to ThingSpeak");
					break;
				case -302:
					verboseDbln("Unexpected failure during write to ThingSpeak");
					nextion_update("wifi_error.message.txt", "-302 Error");
					break;
				case -303:
					verboseDbln("Unable to parse response");
					nextion_update("wifi_error.message.txt", "-303 Error");
					break;
				case -304:
					verboseDbln("Timeout waiting for server to respond");
					nextion_update("wifi_error.message.txt", "-304 Error");
					break;
				case -401:
					verboseDbln("Point was not inserted (rate lilit of 15s?)");
					nextion_update("wifi_error.message.txt", "-401 Error");
					break;
				default:
					verboseDbln("unknown error");
					nextion_update("wifi_error.message.txt", "Unknown thingspeak error.");
					break;
				}
				nextion_goToPage("wifi_error");
				
			} 
			else { verboseDbln("DONE"); }
		#endif



		loop_timer_1min = millis();
	}

	// //Once every day
	// if(now.day() != prevDay){
	// 	#ifdef NEXTION
	// 		// Update nextion clock:
	// 		// TODO //nextion_update("rtc0.val=", now.year());
	// 		nextion_update("rtc1=", now.month());
	// 		nextion_update("rtc2=", now.day());
	// 		nextion_update("rtc3=", now.hour());
	// 		nextion_update("rtc4=", now.minute());
	// 	#endif
	// 	storeEEPROM();
	// 	prevDay = now.day();			// Reset logic
	// }
}
