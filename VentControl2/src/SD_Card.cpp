#include "SD_Card.h"

bool sd_errorFlag = 0;
bool unmountedFlag = 0;


// Headers to the CSV files:
String headers = "date,time,outside,panel,heated-air,room,voltage,current,motorSpeed";		// datalog.csv
String errorHeaders = "date,time,errorType,msg,errortypecount";
String reportHeaders = "date,xxx,xxx,xxx";

void SD_Card_INIT() {
	DBPRINT("Initializing SD card...");

	// see if the card is present and can be initialized:
	if (!SD.begin(chipSelect)) {
		DBPRINT_LN("card failed, or not present.");
		// Send error message to nextion:
		SD_Card_Error("SD card failed, or not present");
		nextion_update("sd_card_sett.sdStatus.txt=", "Could not find SD card");
		nextion_update("data.sd_status.val=", 0);		// Update sd status
		unmountedFlag = 1;
		return;
	}
	else DBPRINT_LN("DONE.");
	
	// DATALOG
	
	bool exists = SD.exists("datalog.csv");				// Check if file already exists
	File dataFile = SD.open("datalog.csv", FILE_WRITE);	// Open file

	// if the file is available, write to it:
	if (dataFile) {
		if(!exists){dataFile.println(headers);}			// Create headers only if the file was just created
		dataFile.close();
		nextion_update("sd_card_sett.sdStatus.txt=", "Working");
		nextion_update("data.sd_status.val=", 1);		// Update sd status
		unmountedFlag = 0;
	}
	else { // if the file isn't open, pop up an error:
		DBPRINT_LN("error opening datalog.csv");
		SD_Card_Error("error opening datalog.csv");
		nextion_update("sd_card_sett.sdStatus.txt=", "Could not open CSV file");
		nextion_update("data.sd_status.val=", 0);		// Update sd status
	}

	// ERROR LIST
	exists = SD.exists("errorlog.csv");						// Check if file exists
	File errorList = SD.open("errorlog.csv", FILE_WRITE);	// Open file
	// if the file is available, write to it:
	if(errorList){
		if(!exists){errorList.println(errorHeaders);}
		errorList.close();
		unmountedFlag = 0;
	}
	else {
		DBPRINT_LN("error opening errorlog.csv");
		SD_Card_Error("error opening errorLog.csv");
		nextion_update("sd_card_sett.sdStatus.txt=", "Could not open errorLog CSV file");
		nextion_update("data.sd_status.val=", 0);			// Update sd status
	}

	// unmountedFlag = 0;
}

void SD_log(String date, String time){
	verboseDbln("SD: UnmountedFlag = " + (String)unmountedFlag);
	// Don't try to open and write to the file if card was unmounted
	if(unmountedFlag==0){	
		// Used Examples->datalogger.ino as reference
		
		String dataString = "";
		
		dataString += date;
		dataString += ",";
		dataString += time;
		dataString += ",";
		dataString += String(t_Outside.value);
		dataString += ",";
		dataString += String(t_Panel.value);
		dataString += ",";
		dataString += String(t_HeatedAir.value);
		dataString += ",";
		dataString += String(t_Inside.value);
		dataString += ",";
		dataString += String(voltage.value);
		dataString += ",";
		dataString += String(current.value);
		dataString += ",";
		dataString += String(motor1.getSpeed());
	
		
		// open the file. note that only one file can be open at a time,
		// so you have to close this one before opening another.
		File dataFile = SD.open("datalog.csv", FILE_WRITE);

		// if the file is available, write to it:
		if (dataFile) {
			dataFile.println(dataString);
			dataFile.close();
			// sd_errorFlag = 0;		// Reset error flag
			clearError(ERR_SD);
		}
		// if the file isn't open, pop up an error:
		else {
			createError(ERR_SD, "Could not write to csv file");
			// if(sd_errorFlag == 0){
			// 	DBPRINT_LN("error opening datalog.csv");
			// 	SD_Card_Error("Could not write to csv file");
			// 	nextion_update("sd_card_sett.sdStatus.txt=", "Could not write to csv file");
			// 	nextion_update("data.sd_status.val=", 0);		// Update sd status
			// 	// Set to 1 so we dont get an error every cycle if no card is present
			// 	sd_errorFlag = 1;			
			// }
		} 
	}
}


void SD_errorlog(int ErrorType, String msg, int count, String date = "", String time = ""){
	// Don't try to open and write to the file if card was unmounted
	if(unmountedFlag==0){	
		
		String dataString = "";
		
		dataString += date;
		dataString += ",";
		dataString += time;
		dataString += ",";
		dataString += ErrorType;
		dataString += msg;
		dataString += count;
		
		// open the file. note that only one file can be open at a time,
		// so you have to close this one before opening another.
		File dataFile = SD.open("errorlog.csv", FILE_WRITE);

		// if the file is available, write to it:
		if (dataFile) {
			dataFile.println(dataString);
			dataFile.close();
			clearError(ERR_SD);
		}

		// if the file isn't open, pop up an error:
		else {
			createError(ERR_SD, "Could not write to csv file");
		} 
	}
}


void SD_unmount() {
	SD.end();
	unmountedFlag = 1;		// Notify that the card was unmounted
	nextion_update("sd_card_sett.sdStatus.txt=", "Unmounted");
	DBPRINT_LN("Unmounted SD card");
	nextion_update("data.sd_status.val=", 0);		// Update sd status
}