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
#include "ErrorHandling.h"
#include "VoltageCheck.h"

Motor motor1(MOTOR1);				// Heated air
Motor motor2(MOTOR2);				// Cool air
DigitalTemp	t_Outside(DS18B20_BUS, 1);
AnalogTemp 	t_Panel(T_PANEL);
AnalogTemp 	t_HeatedAir(T_AIR);
DigitalTemp	t_Inside(DS18B20_BUS, 0);
CurrentSensor current(CURRENT);		
VoltageSensor voltage(VOLTAGE);
Sensor t_Delta;						

#ifndef DEVELOPMENT
const char *ssid = "AndroidAP";
const char *pass = "arduinomega";
#else
const char *ssid = "HUAWEIP20Pro";
const char *pass = "3665d14cd73b";
#endif

ESP8266Client client;					// WIFI client
DS1302 rtc(RTC_RST, RTC_SCL, RTC_IO);	// Initialize rtc object
OneWire oneWirePin(DS18B20_BUS);
DallasTemperature sensors(&oneWirePin);

//***********************  SETTINGS  ***************************
//bool enableSerialPrint = 0;		// Turn on/off printing sensor values to serial 1
//bool sendToNextion 	= 1;		// Turn on/off sending sensor values to nextion (serial 2)
bool allow2motors 	= 0;			// Nextion setting, set default value here
int mode = 1;						// System mode
int tempUpper = 30;					// At which temperature to start motor
int tempLower = 25;					// At which temp to stop motor

//********************** ERROR SETTINGS **************************
float e_voltageThr = 10.7;    		// If voltage goes under this value, send error message

// **********************  VARIABLES  ****************************
float tempDelta;					// How much the temperature is increased when flowing through panel
String errorstr_last = "";
static int prevMode = mode;

struct eepromVariables {
	int tempUpper;
	int tempLower;
	int mode;
	float e_voltageThr;
	float mtr1OnTime;		// Total ontime for motor 1
	float mtr2OnTime;		// Total ontime for motor 2
};
			

void RtcInit() {
	DBPRINT("Initializing RTC...");
	rtc.begin();
	// rtc.adjust(DateTime(__DATE__,__TIME__));
	if(!rtc.isrunning()) {
		rtc.adjust(DateTime(__DATE__,__TIME__));
		DateTime now = rtc.now();
		String date = String(now.day()) + "." + String(now.month()) + "." + String(now.year());
		String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
		if(date == "165.165.2165"){
			DBPRINT_LN("ERROR");
			createError(ERR_RTC, "Could not initialize RTC");
		}
		else verboseDbln("adjusted to " + date + "  " + time);
	} else {
		DateTime now = rtc.now();
		String date = String(now.day()) + "." + String(now.month()) + "." + String(now.year());
		String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
		DBPRINT_LN("DONE - " + date + " " + time);
		clearError(ERR_RTC);
	}
}

void WifiInit() {
	#ifdef THINGSPEAK
	DBPRINT("Initializing Wifi module...");
	if(!wifi.begin(10,11)){
		DBPRINT_LN(F("cannot connect to WIFI module."));
	}
	if(!wifi.connectAP(ssid, pass))
  	{
    	DBPRINT_LN(F("Error connecting to WiFi"));
  	}
	else{ 
		DBPRINT_LN("DONE"); 
		nextion_update("data.wifi_status.val=", 1);
		nextion_update("wifi_sett.status.txt=", "Connected to: " + (String)ssid);
	}
	ThingSpeak.begin(client);
	#endif
}

void storeEEPROM() {
	#ifdef EEPROM_STORE
		DBPRINT("Storing in EEPROM...");
		eepromVariables eeStore;
		eeStore.e_voltageThr = e_voltageThr;
		eeStore.tempLower = tempLower;
		eeStore.tempUpper = tempUpper;
		eeStore.mode = mode;
		int eeAddress = 0;
		EEPROM.put(eeAddress, eeStore);

		DBPRINT_LN("DONE");
	#endif
}

void readEEPROM() {
	#ifdef EEPROM_STORE
		DBPRINT("Reading EEPROM...");
		eepromVariables eeRead;
		int eeAddress = 0;
		EEPROM.get(eeAddress, eeRead);
		delay(500);

		// Check that read values is in a sensible range
		if (eeRead.tempLower > 0 && eeRead.tempLower < 80 && 
			eeRead.tempUpper > 0 && eeRead.tempUpper < 80 && 
			eeRead.e_voltageThr > 8 && eeRead.e_voltageThr < 20) 
		{
			e_voltageThr = eeRead.e_voltageThr;
			tempUpper = eeRead.tempUpper;
			tempLower = eeRead.tempLower;
			mode = eeRead.mode;
			DBPRINT_LN("DONE");
			verboseDbln("***");
			verboseDbln("* TempUpper:       " + (String)tempUpper);
			verboseDbln("* TempLower:       " + (String)tempLower);
			verboseDbln("* Low voltage thr: " + (String)e_voltageThr);
			verboseDbln("* Mode:            " + (String)mode);
			verboseDbln("***");
		}
		else { DBPRINT_LN("ERROR - using deafult values."); }

	#endif
}

void init_sensors() {
	DBPRINT("Initializing sensors...");
	sensors.begin();
	DigitalTemp::tempInit();
	DBPRINT("found " + (String)sensors.getDeviceCount() + " digital sensors...");
	t_Outside.read();
	t_Panel.read();
	t_HeatedAir.read();
	t_Inside.read();
	current.read();
	voltage.read();

	tempDelta = t_HeatedAir.value - t_Outside.value;
	t_Delta.newValue(tempDelta);
	DBPRINT_LN("DONE");
}

void readSensors() {
	verboseDb("Reading sensors...");
	unsigned long start = millis();
	
	if(!t_Outside.read()) 	createError(ERR_TEMP_OUTSIDE); 	else clearError(ERR_TEMP_OUTSIDE);
	if(!t_Panel.read()) 	createError(ERR_TEMP_PANEL);	else clearError(ERR_TEMP_PANEL);
	if(!t_HeatedAir.read())	createError(ERR_TEMP_AIR);		else clearError(ERR_TEMP_AIR);
	if(!t_Inside.read())	createError(ERR_TEMP_INSIDE);	else clearError(ERR_TEMP_INSIDE);
	if(!current.read())		createError(ERR_CURRENT);		else clearError(ERR_CURRENT);
	if(!voltage.read())		createError(ERR_VOLTAGE_SENSOR);else clearError(ERR_VOLTAGE_SENSOR);

	tempDelta = t_HeatedAir.value - t_Outside.value;
	t_Delta.newValue(tempDelta);

	verboseDbln("DONE " + (String)(millis()-start) + "ms");
}

void telemetryLog() {
	#ifdef TELEMETRY
		// Message is formatted for use with Telemetry viewer. Use a layout2 in Telemetry folder.
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
}

void serialMonLog() {
	#ifdef VERBOSE_DB
	String text = "****** ";
	verboseDbln(text);
	text = "* Outside: " + (String)t_Outside.getValue() 	+ "C" + '\t' + t_Outside.getSlope() + " C/min";
	verboseDbln(text);
	text = "* Panel:   " + (String)t_Panel.getValue() 		+ "C" + '\t' + t_Panel.getSlope() + " C/min";
	verboseDbln(text);
	text = "* Air:     " + (String)t_HeatedAir.getValue()  	+ "C"+ '\t' + t_HeatedAir.getSlope() + " C/min";
	verboseDbln(text);
	text = "* t_Delta: " + (String)t_Delta.getValue() 		+ "C"+ '\t' + t_Delta.getSlope() + " C/min";
	verboseDbln(text);
	text = "* Inside:  " + (String)t_Inside.getValue()  	+ "C"+ '\t' + t_Inside.getSlope() + " C/min";
	verboseDbln(text);
	text = "* Voltage: " + (String)voltage.getValue()  		+ "V"+ '\t' + voltage.getSlope() + " V/min";
	verboseDbln(text);
	text = "* Current: " + (String)current.getValue()  		+ "A"+ '\t' + current.getSlope() + " A/min";
	verboseDbln(text);
	text = "****** ";
	verboseDbln(text);
	#endif
}

void thingspeakLog() {
	#ifdef THINGSPEAK
		unsigned long start = millis();
		verboseDb("Sending to Thingspeak...");
		ThingSpeak.setField(1,t_Outside.getValue());
		ThingSpeak.setField(2,t_Panel.getValue());
		ThingSpeak.setField(3,t_HeatedAir.getValue());
		ThingSpeak.setField(4,t_Inside.getValue());
		ThingSpeak.setField(5,voltage.getValue());
		ThingSpeak.setField(6,current.getValue());
		ThingSpeak.setField(7,motor1.getSpeed());
		ThingSpeak.setField(8,t_Delta.getSlope());
		
		String arduinoStatus = "Mode: " + (String)mode + ". ";
		arduinoStatus += getErrorsOneline(false, true);

		// Only update status on change
		if (arduinoStatus != errorstr_last) {
			if (arduinoStatus.length() < 255) ThingSpeak.setStatus(arduinoStatus);
			else ThingSpeak.setStatus("Status update failed, error string too long");
			errorstr_last = arduinoStatus;
			// static unsigned long 
		}
		
		int status = ThingSpeak.writeFields(CHANNEL_ID,CHANNEL_API_KEY);

		// Print error to nextion and serial
		if(status != 200) {
			verboseDb("ERROR - ");
			createError(ERR_THINKSPEAK, (String)status);
		}
		else { 
			verboseDbln("DONE");
			clearError(ERR_THINKSPEAK);
		}

		verboseDbln("Thingspeak upload took " + (String)((millis()-start)/1000) + "seconds");

	#endif
}

#ifdef NEXTION
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
	mode = HEATING;
	DBPRINT("Enabled Heating on display. Mode = ");
	DBPRINT_LN(mode);
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
		mode = MANUAL;
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
		dismissVoltageError();
		mode = MANUAL;					// Turn off heating
	}
	void ignorePopCallback(void *ptr){
		dismissVoltageError();
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
#endif

//**********************************************   SETUP     ********************************************//
void setup() {													
	Serial.begin(9600);
	Serial2.begin(9600);
	delay(400);
	DBPRINT_LN("Starting Setup...");
	SD_Card_INIT();
	readEEPROM();
	RtcInit();
	pinMode(MOTOR1, OUTPUT);           	// set motor as output
	pinMode(MOTOR2, OUTPUT);           	// set motor as output
	init_sensors();
	WifiInit();
	
	#ifdef NEXTION
	DBPRINT("Initializing nextion display...");
	bool isInitialized = nexInit();					// Initialize nextion display
	nextion_goToPage("ardu_restart");				// Notify the display that arduino was restarted
	nextion_update("settings_2.v_err.val=", (e_voltageThr*10));
	if(isInitialized)DBPRINT_LN("DONE");
	else DBPRINT_LN("ERROR");


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
	
	// Update nextion start values
	NEXtempThrUpdate();
	NEXsensor_maxUpdate();
	#endif

	DBPRINT_LN("Setup Done.");
} // END OF SETUP


//**********************************************   MAIN     ********************************************//
void loop() {
	/**
	* TODO:
	* - Periodically run the motor for a specified time automatically. 
	* - What happens if RTC card fails?
	* - What happens if SD card fails?
	* - Add total Motor time (EEPROM)
	*/
	
	DateTime now = rtc.now();
	String date = String(now.day()) + "." + String(now.month()) + "." + String(now.year());
	String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
	if (date == "165.165.2165") {
		createError(ERR_RTC);
	} else clearError(ERR_RTC);

	// static unsigned long loop_timer_1s;
	static unsigned long loop_timer_5s;
	static unsigned long loop_timer_1min;
	static unsigned long loop_timer_1day;
	
	// Every cycle:
	#ifdef NEXTION
		nexLoop(nex_listen_list);
	#endif

	// When changing mode, stop motors
	if (mode != prevMode) { motor1.setSpeed(0); motor2.setSpeed(0); prevMode = mode; verboseDbln("Mode changed to " + String(mode)); }
	// Turn off motors after a predifined time in manual mode
	if (mode == MANUAL) {
		if(motor1.isRunning && motor1.getTimeOn() > MOTOR_TIMEOUT) { motor1.setSpeed(0); verboseDbln("Motor1 timeout"); }
		if(motor2.isRunning && motor2.getTimeOn() > MOTOR_TIMEOUT) { motor2.setSpeed(0); verboseDbln("Motor2 timeout"); }
	}

	// Once every 5 seconds
	if((millis() - loop_timer_5s) > FIVE_SEC) {
		loop_timer_5s = millis();	// Reset timer

		readSensors();
		heating();
		sysValUpdate();
		serialMonLog();	
		telemetryLog();
	}
	
	// once every minute
	if(millis() - loop_timer_1min > ONE_MIN) {
		loop_timer_1min = millis();

		SD_log(date, time);
		thingspeakLog();
		errorPrint(getErrors());
	}

	static int prevDay;
	//Once every day
	// If RTC clock is not working, use millis() instead
	bool isNewDay = false;
	if (isActiveErrorType(ERR_RTC)) {
		if (millis() - loop_timer_1day > ONE_DAY) {
			isNewDay = true;
		}
	}
	else if (now.day() != prevDay) {
		isNewDay = true;
	}

	if(isNewDay) {
		#ifdef NEXTION
			// Update nextion clock:
			// TODO //nextion_update("rtc0.val=", now.year());
			nextion_update("rtc1=", now.month());
			nextion_update("rtc2=", now.day());
			nextion_update("rtc3=", now.hour());
			nextion_update("rtc4=", now.minute());
		#endif
		String date = String(now.day()) + "." + String(now.month()) + "." + String(now.year());
		SD_DayReport(date, motor1.getTimeOn(), getCreatedErrorCount());
		resetCreatedErrorCount();
		storeEEPROM();

		// Reset logic
		prevDay = now.day();			
		loop_timer_1day = millis();
	}
}
