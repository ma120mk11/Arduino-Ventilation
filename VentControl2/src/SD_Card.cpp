#include <Arduino.h>
#include <SD.h>
#include "nextionDisp.h"
#include "settings.h"
#include <RTClib.h>

extern int M1Speed;
extern float temp0C, temp1C, temp2C, temp3C, voltage, current;

bool sd_errorFlag = 0;
bool unmountedFlag = 0;

// Headers to the CSV file:
String headers = "date,time,outside,panel,heated-air,room,voltage,current,motorSpeed";


void SD_Card_INIT(){
	Serial.print("Initializing SD card...");

	// see if the card is present and can be initialized:
	if (!SD.begin(chipSelect)) {
		Serial.println("Card failed, or not present");
		// Send error message to nextion:
		SD_Card_Error("SD card failed, or not present");
		nextion_update("sd_card_sett.sdStatus.txt=", "Could not find SD card");
		nextion_update("data.sd_status.val=", 0);		// Update sd status
		return;
	}

	Serial.println("card initialized.");
	
	bool exists = SD.exists("datalog.csv");				// Check if file already exists

	File dataFile = SD.open("datalog.csv", FILE_WRITE);	// Open file

	// if the file is available, write to it:
	if (dataFile) {
		if(!exists){dataFile.println(headers);}			// Create headers only if the file was just created
		dataFile.close();
		nextion_update("sd_card_sett.sdStatus.txt=", "Working");
		nextion_update("data.sd_status.val=", 1);		// Update sd status
	}
	// if the file isn't open, pop up an error:
	else {
		Serial.println("error opening datalog.csv");
		SD_Card_Error("error opening datalog.csv");
		nextion_update("sd_card_sett.sdStatus.txt=", "Could not open CSV file");
		nextion_update("data.sd_status.val=", 0);		// Update sd status
	}
	unmountedFlag = 0;
}

void SD_log(String date, String time){
	// Don't try to open and write to the file if card was unmounted
	if(unmountedFlag==0){	
		// Used Examples->datalogger.ino as reference
		
		String dataString = "";
		
		dataString += date;
		dataString += ",";
		dataString += time;
		dataString += ",";
		dataString += String(temp0C);
		dataString += ",";
		dataString += String(temp1C);
		dataString += ",";
		dataString += String(temp2C);
		dataString += ",";
		dataString += String(temp3C);
		dataString += ",";
		dataString += String(voltage);
		dataString += ",";
		dataString += String(current);
		dataString += ",";
		dataString += String(M1Speed);
	
		
		// open the file. note that only one file can be open at a time,
		// so you have to close this one before opening another.
		File dataFile = SD.open("datalog.csv", FILE_WRITE);

		// if the file is available, write to it:
		if (dataFile) {
			dataFile.println(dataString);
			dataFile.close();
			sd_errorFlag = 0;		// Reset error flag
		}

		// if the file isn't open, pop up an error:
		else {
			if(sd_errorFlag == 0){
				Serial.println("error opening datalog.csv");
				SD_Card_Error("Could not write to csv file");
				nextion_update("sd_card_sett.sdStatus.txt=", "Could not write to csv file");
				nextion_update("data.sd_status.val=", 0);		// Update sd status
				// Set to 1 so we dont get an error every cycle if no card is present
				sd_errorFlag = 1;			
			}
		} 
	}
}

void SD_unmount(){
	SD.end();
	unmountedFlag = 1;		// Notify that the card was unmounted
	nextion_update("sd_card_sett.sdStatus.txt=", "Unmounted");
	Serial.println("Unmounted SD card");
	nextion_update("data.sd_status.val=", 0);		// Update sd status
}