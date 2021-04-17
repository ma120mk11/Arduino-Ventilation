#include "SD_Card.h"

// bool sd_errorFlag = 0;
bool isUnmounted = true;


// Headers to the CSV files:
String headers = "date,time,outside,panel,heated-air,room,voltage,current,motorSpeed";		// datalog.csv
String errorHeaders = "date,time,errorType,msg,errortypecount";
String reportHeaders = "date,ErrorsCreated,MotorOnTime";

void SD_Card_INIT() {
	DBPRINT("Initializing SD card...");

	// see if the card is present and can be initialized:
	if (!SD.begin(chipSelect)) {
		DBPRINT_LN("card failed, or not present.");
		// Send error message to nextion:
		SD_Card_Error("SD card failed, or not present");
		nextion_update("sd_card_sett.sdStatus.txt=", "Could not find SD card");
		nextion_update("data.sd_status.val=", 0);		// Update sd status
		isUnmounted = 1;
		return;
	}
	else DBPRINT_LN("DONE.");
	
	// DATALOG
	
	bool exists = SD.exists("datalog.csv");				// Check if file already exists
	File dataFile = SD.open("datalog.csv", FILE_WRITE);	// Open file

	// if the file is available, write to it:
	if (dataFile) {
		if(!exists){ dataFile.println(headers); }			// Create headers only if the file was just created
		
		nextion_update("sd_card_sett.sdStatus.txt=", "Working");
		nextion_update("data.sd_status.val=", 1);		// Update sd status
		isUnmounted = 0;
	}
	else { // if the file isn't open, pop up an error:
		DBPRINT_LN("error opening datalog.csv");
		SD_Card_Error("error opening datalog.csv");
		nextion_update("sd_card_sett.sdStatus.txt=", "Could not open CSV file");
		nextion_update("data.sd_status.val=", 0);		// Update sd status
	}

	dataFile.close();

	// ERROR LOG
	exists = SD.exists("errorlog.csv");						// Check if file exists
	File errorList = SD.open("errorlog.csv", FILE_WRITE);	// Open file
	// if the file is available, write to it:
	if (errorList) {
		if (!exists) { errorList.println(errorHeaders); }
		
		isUnmounted = 0;
	}
	else {
		DBPRINT_LN("error opening errorlog.csv");
		SD_Card_Error("error opening errorLog.csv");
		nextion_update("sd_card_sett.sdStatus.txt=", "Could not open errorLog CSV file");
		nextion_update("data.sd_status.val=", 0);			// Update sd status
	}
	errorList.close();


	// DAY REPORT
	exists = SD.exists("dayreport.csv");					// Check if file exists
	File dayReport = SD.open("dayreport.csv", FILE_WRITE);	// Open file
	// if the file is available, write to it:
	if (dayReport) {
		if (!exists) { dayReport.println(reportHeaders); }
		
		isUnmounted = 0;
	}
	else {
		DBPRINT_LN("error opening dayreport.csv");
		SD_Card_Error("error opening dayreport.csv");
		nextion_update("sd_card_sett.sdStatus.txt=", "Could not open dayreport CSV file");
		nextion_update("data.sd_status.val=", 0);			// Update sd status
	}
	dayReport.close();
}


void SD_log(String date = "", String time = "") {
	if (isUnmounted) verboseDbln("SD: Unmounted");
	
	// Don't try to open and write to the file if card was unmounted
	if(isUnmounted==0){	
		// Used Examples->datalogger.ino as reference
		verboseDb("SD: Logging...");

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
			verboseDbln("DONE");
			clearError(ERR_SD);
		}
		// if the file isn't open, pop up an error:
		else {
			verboseDbln("ERROR");
			createError(ERR_SD, "Could not write to csv file");
		}
		dataFile.close();
	}
}

void SD_errorlog(int ErrorType, String msg, int count, String date = "", String time = "") {
	// Don't try to open and write to the file if card was unmounted
	if(isUnmounted==0) {	
		
		String dataString = "";
		
		dataString += date 		+ ",";
		dataString += time 		+ ",";
		dataString += (String)ErrorType + ",";
		dataString += (String)msg 		+ ",";
		dataString += (String)count;
		
		// open the file. note that only one file can be open at a time,
		// so you have to close this one before opening another.
		File dataFile = SD.open("errorlog.csv", FILE_WRITE);

		// if the file is available, write to it:
		if (dataFile) {
			dataFile.println(dataString);
			clearError(ERR_SD);
		}
		// if the file isn't open, pop up an error:
		else {
			createError(ERR_SD, "Could not write to csv file");
		} 
		dataFile.close();
	}
}


void SD_DayReport(String date, int mtrOnTime, int errorcount){
	if (!isUnmounted){
		String dataString = date 	+ ",";
		dataString += errorcount	+ ",";
		dataString += mtrOnTime;

		File dataFile = SD.open("dayreport.csv", FILE_WRITE);
		if (dataFile) {
			dataFile.println(dataString);
			clearError(ERR_SD);
		}
		else {
			createError(ERR_SD, "Could not write to file");
		}
		dataFile.close();
	}
}



void SD_unmount() {
	SD.end();
	isUnmounted = true;		// Notify that the card was unmounted
	nextion_update("sd_card_sett.sdStatus.txt=", "Unmounted");
	nextion_update("data.sd_status.val=", 0);		// Update sd status
	DBPRINT_LN("Unmounted SD card");
}