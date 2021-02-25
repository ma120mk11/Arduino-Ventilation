#pragma once

#include "Nextion.h"
#include "nextionDisp.h"
#include "SD_Card.h"
#include "sensor.h"
#include "motor_speed.h"
#include "heating.h"
#include "settings.h"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

Motor motor1;

//*********************** SETTINGS ***************************
bool enableSerialPrint = 0;		// Turn on/off printing sensor values to serial 1
bool sendToNextion 	= 1;		// Turn on/off sending sensor values to nextion (serial 2)
bool allow2motors 	= 0;		// Nextion setting, set default value here
bool enableHeating 	= 1;		// Enable AUTO setting for motor 1
bool enableMotorStep = 0;		// In auto mode, on -> use motor speeds 0,3,4,5. Off -> on/off

int tempUpper = 40;				// At which temperature to start motor
int tempLower = 30;				// At which temp to stop motor

int autoCyckle = 20;			// Number of times to read value over threshold before action

//********************** ERROR SETTINGS ***************************
float e_voltageThr = 10.7;    // If voltage goes under this value, send error message


// ********************** VARIABLES **************************

int M1PWM = 0;            		// motor1 PWM speed (0-255)
int M2PWM = 0;               	// motor2 PWM speed (0-255)
int M1Speed = 0;             	// motor1 Speed (0-5)
int M2Speed = 0;             	// motor2 Speed (0-5)
bool m1Running = 0;				// motor 1 state

float temp0C;					// TempSensor0 value in degrees
float temp1C;					// TempSensor1 value in degrees
float temp2C;					// TempSensor2 value in degrees
float temp3C;					// TempSensor3
float temp4C = 5.4;		  		// TempSensor4
float voltage;					// Voltage sensor reading
float current;					// 
int light;
float derivate;					// The derivate of panel temperature


float v0_1 = 0;					// Previous voltage reading
float v0_2 = 0;					// Second last voltage reading

float i0_1 = 0;					// Previous current reading
float i0_2 = 0;					// second last current reading

float filteredSignal = 0;		// For debugging and comparison

float temp0C_max = 0;			// Sensor top value
float temp1C_max = 0;			
float temp2C_max = 0;			
float temp3C_max = 0;			
float temp4C_max = 0;
float voltage_min = 50;			// Set to high value so it gets overwritten at start		
float voltage_max = 0;			
float current_max;				
float tempDelta_max;			

float tempDelta;				// How much the temperature is increased when flowing through panel

int nextionPage;				// the page that is currently active
int nextionMode;				// the active mode

int voltageErrorCount = 0;		// 
bool errorPending = 0;			//

int nexUpload = 0;				//

int n = autoCyckle;
int k = autoCyckle;



// Initialize rtc object
DS1302 rtc(rtc_RST, rtc_SCL, rtc_IO);


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
		Serial.print("Enabled Heating on display. enableHeating = ");
		Serial.println(enableHeating);
	}

  void setTimePopCallback(void *ptr){
    /**
     * TODO:
     * Get the values that are set on the display and update RTC.
     */
    int setDay, setMonth, setYear, setHour, setMinute;
	



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
//**********************************************   SETUP     ********************************************//
void setup() {													
	Serial.begin(9600);
	Serial2.begin(9600);
	nexInit();							// Initialize nextion display
	nextion_goToPage("ardu_restart");	// Notify the display that arduino was restarted
	pinMode(Motor1, OUTPUT);           	// set motor as output
	pinMode(Motor2, OUTPUT);           	// set motor as output
	sensorRead();
	

	rtc.begin();
	if(!rtc.isrunning()){
		Serial.print("RTC is not running. ");
		rtc.adjust(DateTime(__DATE__,__TIME__));
		Serial.println("Time and date adjusted");
	}


	SD_Card_INIT();
	

	springHeat.attachPop(springHeatPopCallback, &springHeat); // MENU

	setTime.attachPop(setTimePopCallback, &setTime);  // Page 3

	
	bMS1.attachPop(bMS1PopCallback, &bMS1);       // Page 4
	bMS2.attachPop(bMS2PopCallback, &bMS2);
	bMS3.attachPop(bMS3PopCallback, &bMS3);
	bMS4.attachPop(bMS4PopCallback, &bMS4);
	bMS5.attachPop(bMS5PopCallback, &bMS5);
	bMS0.attachPop(bMS0PopCallback, &bMS0);
	bM2.attachPop(bM2PopCallback, &bM2);
	
	bM2S1.attachPop(bM2S1PopCallback, &bM2S1);      // Page 5
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
	ignore.attachPop(ignorePopCallback, &ignore);
	
	v_errDec.attachPop(v_errDecPopCallback, &v_errDec);     // page 15 - settings 2
	v_errInc.attachPop(v_errIncPopCallback, &v_errInc);
	// Page 21
	sd_init.attachPop(sd_initPopCallback, &sd_init);
	sd_unmount.attachPop(sd_unmountPopCallback, &sd_unmount);

	// *** Update NEXTION start values ***
	NEXtempThrUpdate();
	NEXsensor_maxUpdate();
	
	// Update voltage thr
	int thr = e_voltageThr*10;
	nextion_update("settings_2.v_err.val=", thr);


} // END OF SETUP

int prevDay;
unsigned long loop_timer_1s;
unsigned long loop_timer_3s;
unsigned long loop_timer_1min;

//##############################################      MAIN       ############################################################
void loop() {
  /**
   * TODO:
   * - Periodically run the motor for a specified time automatically. 
   * - Send a "ok" package to display every minute. If display doesn't get the "ok" message, display error message
   * - Set display time once a day. To prevent drifts.
   * - Store settings in EEPROM
	*/
   
   	#define ONE_SEC 1000
	#define THREE_SEC 3000
	#define ONE_MIN 60000

	DateTime now = rtc.now();
	String date = String(now.day()) + "." + String(now.month()) + "." + String(now.year());
	String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

	// Ecery cycle::
	nexLoop(nex_listen_list);


	// Once every second:
	if((millis() - loop_timer_1s) > ONE_SEC){




		loop_timer_1s = millis();		// Reset timer
	}



	// Once every 3 seconds
	if((millis() - loop_timer_3s) > THREE_SEC){
		sensorRead();
		heating();
		sysValUpdate();


		loop_timer_3s = millis();		// Reset timer
	}

	// once every minute
	if(millis() - loop_timer_1min > ONE_MIN){

		SD_log(date, time);



		loop_timer_1min = millis();
	}

	// Once every day
	if(now.day() != prevDay){
		
		/**
		 * TODO:
		 * Create a day summary and save it in a day report .csv file
		*/

		// Update nextion clock:
		nextion_update("rtc2=", now.day());
		nextion_update("rtc1=", now.month());
		// TODO //nextion_update("rtc0.val=", now.year());
		nextion_update("rtc3=", now.hour());
		nextion_update("rtc4=", now.minute());

		prevDay = now.day();			// Reset logic
	}
}
