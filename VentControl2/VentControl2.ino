#include "Nextion.h"
#include <SPI.h>
#include <SD.h>

// USE TX2 for Nextion display communication

//*********************** PINS ******************************
const int Motor1 = 9;          	// motor1 pin HEATED AIR
const int Motor2 = 10;         	// motor2 pin DIRECT AIR

const int TempSensor0 = A4;		  // LM35 temp sensor0 		OUTSIDE TEMP
const int TempSensor1 = A1;   	// LM35 temp sensor1 pin  	PANEL TEMP
const int TempSensor2 = A2;   	// LM35 temp sensor2 pin	HOT AIR
const int TempSensor3 = A3;   	// LM35 temp sensor3 pin	LIVING ROOM
const int TempSensor4 = A5;   	// LM35 temp sensor4 pin

const int CurrentSensor = A7;  	// Current sensor pin
const int VoltageSensor = A8;  	// Voltage sensor pin


// See SD_Card.ino for more info:
const int chipSelect = 53;		// Pin for SD card

File myFile;					// For SD Card logging

//*********************** SETTINGS ***************************
bool enableSerialPrint = 1;		// Turn on/off printing sensor values to serial 1
bool sendToNextion 	= 1;		  // Turn on/off sending sensor values to nextion (serial 2)
bool allow2motors 	= 0;		  // Nextion setting, set default value here
bool enableHeating 	= 0;		  // Enable AUTO setting for motor 1
bool enableMotorStep = 0;		  // In auto mode, on -> use motor speeds 0,3,4,5. Off -> on/off

float pinReference = 5;			  // 

int tempUpper = 40;				    // At which temperature to start motor
int tempLower = 30;				    // At which temp to stop motor

int autoCyckle = 20;			    // Number of times to read value over threshold before action

//********************** ERROR SETTINGS ***************************
float e_voltageThr = 10.7;    // If voltage goes under this value, send error message




// ********************** VARIABLES **************************

int M1PWM = 0;            		// motor1 PWM speed (0-255)
int M2PWM = 0;               	// motor2 PWM speed (0-255)
int M1Speed = 0;             	// motor1 Speed (0-5)
int M2Speed = 0;             	// motor2 Speed (0-5)
bool m1Running = 0;				    // motor 1 state

float temp0C;					  // TempSensor0 value in degrees
float temp1C;					  // TempSensor1 value in degrees
float temp2C;					  // TempSensor2 value in degrees
float temp3C;					  // TempSensor3
float temp4C = 5.4;		  // TempSensor4
float voltage;

float v0_1 = 0;
float v0_2 = 0;

float current;					// Voltage sensor current reading

float i0_1 = 0;					// last value
float i0_2 = 0;					// second last value

float filteredSignal =0;		// For debugging and comparison

float temp0C_max = 0;			// Sensor top value
float temp1C_max = 0;			
float temp2C_max = 0;			
float temp3C_max = 0;			
float temp4C_max = 0;
float voltage_min = 50;			// High value so it gets overwritten at start		
float voltage_max = 0;
float current_max;
float tempDelta_max;

float tempDelta;				// How much the temperature is increased when flowing through panel

int nextionPage;				// the page that is currently active
int nextionMode;				// the active mode

int voltageErrorCount = 0;		// 
bool errorPending = 0;			//

int nexUpload = 0;

int n = autoCyckle;
int k = autoCyckle;

// ***************** SENSOR CALIBRATION **********************
float t0sensorOffset = 0;
float t1sensorOffset = 1;
float t2sensorOffset = 1.5;
float t3sensorOffset = 2;
float voltageOffset = 0;
float currentOffset = 0;


//*********************** FUNCTIONS  *******************************

void SD_Card_INIT();
void nexButtons_INIT();
void m1SetSpeed(int s);
void m2SetSpeed(int s);
void sensorRead();
void sensorCheck();
void heating();
void sysValUpdate();			                          // Updates sensor values on NEXTION
void sensor_maxReset();
void NEXsensor_maxUpdate();
void NEXtempThrUpdate();
float filter(float, float, float, float);
String SD_Card_Error(String);
void SD_log();
void nextion_update(String object, float value);    // sends message to nextion
void nextion_update(String object, int value);
void nextion_goToPage(String page);
void nextion_update(String object, String message);

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
//	NexButton to_sleep = NexButton(0,4,"to_sleep");
	  NexButton springHeat = NexButton(0,3,"springHeat");
	
// Sleep page
//	NexButton t1 = NexButton(13,2,"t1");

//  MANUAL MOTOR 1 - Page 4

    NexButton bMS1 = NexButton(4,1,"bMS1");                 // Button 1
    NexButton bMS2 = NexButton(4,2,"bMS2");                 // Button 2
    NexButton bMS3 = NexButton(4,3,"bMS3");                 // Button 3
    NexButton bMS4 = NexButton(4,4,"bMS4");                 // Button 4
    NexButton bMS5 = NexButton(4,5,"bMS5");                 // Button 5
    NexButton bMS0 = NexButton(4,6,"bMS0");                 // Button OFF
	  NexButton bM2  = NexButton(4,12,"bM2");				  	// Change to motor 2 (page 5)

//  MANUAL MOTOR 2 - Page 5

    NexButton bM2S1 = NexButton(5,1,"bM2S1");           // Button 1
    NexButton bM2S2 = NexButton(5,2,"bM2S2");           // Button 2
    NexButton bM2S3 = NexButton(5,3,"bM2S3");           // Button 3
    NexButton bM2S4 = NexButton(5,4,"bM2S4");           // Button 4
    NexButton bM2S5 = NexButton(5,5,"bM2S5");           // Button 5
    NexButton bM2S0 = NexButton(5,6,"bM2S0");           // Button OFF
	  NexButton bM1	= NexButton(5,11,"bM1");			// Change to motor 1 (page 4)

// Spring Heating MODE
	NexButton springExit = NexButton(6,1,"springExit");		// Exit spring mode


//	spring_sett1 - SPRING SETTINGS
	NexButton Dec_Utemp = NexButton(7,5,"Dec_Utemp");	// Upper temp DECREASE
	NexButton Inc_Utemp = NexButton(7,7,"Inc_Utemp");	// Upper temp INCREASE
	NexButton Dec_Ltemp = NexButton(7,6,"Dec_Ltemp");
	NexButton Inc_Ltemp = NexButton(7,8,"Inc_Ltemp");

	//NexDSButton btallow2m = NexDSButton(10,13,"btallow2m");	  // Allow 2 motors att the same time?
	
// 9 - Sensor_data
	
	NexButton Measure = NexButton(9,22,"Measure");
	
	
// 10 - sensor_top
	NexButton Reset = NexButton(10,22,"Reset");			// Reset top_values
	NexButton Update = NexButton(10,25,"Update");

// 14 Voltage Error
	NexButton v_err_exit = NexButton(14,6,"v_err_exit");			// Exit mode
	NexButton ignore = NexButton(14,3,"ignore");		// Ignore error	

// 15 - Settings 2

	NexButton v_errDec = NexButton(15,3,"v_errDec");
	NexButton v_errInc = NexButton(15,4,"v_errInc");

// ************* Register button objects to the touch event list. *****************
NexTouch *nex_listen_list[] = {
   // Page 0 menu
      &springHeat,
//	  &to_sleep,
//	  &t1,
	  
   // Page 1

   // Page 2
  
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
    NULL
};


//*****************************    DISPLAY BUTTON FUNCTIONS   ********************************************

// ********** MENU **********
	void springHeatPopCallback(void *ptr){
		enableHeating = 1;
	}

/*
	void to_sleepPopCallback(void *ptr){
		sendToNextion=0;
	}
	
	void t1PopCallback(void *ptr){
		sendToNextion=1;
	}
*/	
// ************MANUAL - page 4 **************

	// 1-button release function
		void bMS1PopCallback(void *ptr){
		  m1SetSpeed(1);                		// Sets motor speed
		  dbSerialPrintln("Page4-Button-1");
		}
		
	// 2-button release function
		void bMS2PopCallback(void *ptr){
		m1SetSpeed(2);
	   
		}
	// 3-button release function
		void bMS3PopCallback(void *ptr){
		m1SetSpeed(3); 
	   
		}
	// 4-button release function
		void bMS4PopCallback(void *ptr){
		m1SetSpeed(4);  
	  
		}
	// 5-button release function
		void bMS5PopCallback(void *ptr){
		m1SetSpeed(5);
	  
		}
	// OFF-button release function
		void bMS0PopCallback(void *ptr){
		m1SetSpeed(0); 

		}
	// Motor 2 button
		void bM2PopCallback(void *ptr){
			if(allow2motors == 0){
				m1SetSpeed(0);
			}	
		}
// ************   MANUAL Motor 2 - page 5 **************

	// 1-button release function
		void bM2S1PopCallback(void *ptr){
		  m2SetSpeed(1);                	// Sets motor speed
		  dbSerialPrintln("Page5-Button-1");
		}
		
	// 2-button release function
		void bM2S2PopCallback(void *ptr){
		m2SetSpeed(2);
	   
		}
	// 3-button release function
		void bM2S3PopCallback(void *ptr){
		m2SetSpeed(3); 
	   
		}
	// 4-button release function
		void bM2S4PopCallback(void *ptr){
		m2SetSpeed(4);  
	  
		}
	// 5-button release function
		void bM2S5PopCallback(void *ptr){
		m2SetSpeed(5);
	  
		}
	// OFF-button release function
		void bM2S0PopCallback(void *ptr){
		m2SetSpeed(0); 

		}
	// Motor 1 button
		void bM1PopCallback(void *ptr){		// When switching page, check if both can be on at the same time, if not -> off
			if(allow2motors == 0){
				m2SetSpeed(0);
			}
		}
	
	
	// PAGE 6 SPRING HEATING
	
		void springExitPopCallback(void *ptr){
			enableHeating = 0;
			m1SetSpeed(0);
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
			sensor_maxReset();
			NEXsensor_maxUpdate();
		}
	// Top values update button
		void UpdatePopCallback(void *ptr){
			NEXsensor_maxUpdate();
		}
		
	// Update sensors
		void MeasurePopCallback(void *ptr){
			sensorRead();
			sysValUpdate();
		}
	// 14 Voltage error
		void v_err_exitPopCallback(void *ptr){
			errorPending=0;
			enableHeating=0;
		}
		void ignorePopCallback(void *ptr){
			errorPending=0;
		}
	
	// 15 - settings 2
		void v_errDecPopCallback(void *ptr){
			e_voltageThr -= 0.1;
			int thr = e_voltageThr*10;
			Serial2.print("settings_2.v_err.val=");
			Serial2.print(thr);  
			Serial2.write(0xff);
			Serial2.write(0xff);
			Serial2.write(0xff);
		}
		
		void v_errIncPopCallback(void *ptr){
			e_voltageThr += 0.1;
			int thr = e_voltageThr*10;
			
      nextion_update("settings_2.v_err.val=", thr);
		}

//**********************************************   SETUP     ********************************************//
void setup() {													
  //analogReference(INTERNAL2V56);		// ADC Voltage reference, pinReference in settings should be the same
	Serial.begin(9600);
	nexInit();
  nexButtons_INIT();                 // attach pop callback
	pinMode(Motor1, OUTPUT);           // set motor as output
	pinMode(Motor2, OUTPUT);           // set motor as output
	sensorRead();
	SD_Card_INIT();
} // END OF SETUP



//##############################################      MAIN         ############################################################
void loop() {
  /**
   * TODO
   * - Periodically run the motor for a specified time automatically. 
   * - Send a "ok" package to display every minute. If display doesn't get the "ok" message, display error message
   * - Set display time once a day. To prevent drifts.
   * 
   */
   
	nexLoop(nex_listen_list);
	sensorRead();
	SD_log();

 // We dont need to update nextion values every cycle. 
	if(nexUpload >= 10){
		sysValUpdate();
		nexUpload = 0;
	}
	nexUpload++;

  
	heating();
	
	delay(100);		//500
}
