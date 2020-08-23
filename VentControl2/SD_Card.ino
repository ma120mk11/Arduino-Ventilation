/*
https://lastminuteengineers.com/arduino-micro-sd-card-module-tutorial/

Pinout:			MOSI	MISO	SCK		CS
Arduino Mega	51		50		52		53
Arduino Uno		11		12		13		10
Arduino Nano	11		12		13		10

*/

void SD_Card_INIT(){
	Serial.print("Initializing SD card...");

	// see if the card is present and can be initialized:
	if (!SD.begin(chipSelect)) {
		Serial.println("Card failed, or not present");
		// Send error message to nextion:
		SD_Card_Error("Card failed, or not present");
	}
	Serial.println("card initialized.");
	
	// Write Headers to the CSV file:
	String headers = "date,time,outside,panel,inside,voltage,current,motorSpeed";
	
	File dataFile = SD.open("datalog.csv", FILE_WRITE);

	// if the file is available, write to it:
	if (dataFile) {
		dataFile.println(headers);
		dataFile.close();
	}
	// if the file isn't open, pop up an error:
	else {
		Serial.println("error opening datalog.csv");
		SD_Card_Error("error opening datalog.csv");
	}

}

void SD_log(){
	
	// Used Examples->datalogger.ino as reference
	
	// TODO 
	String date = "23.5.20";
	String time = "16:49:12";
	
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
	}
	// if the file isn't open, pop up an error:
	else {
		Serial.println("error opening datalog.csv");
		SD_Card_Error("error opening datalog.csv");
	} 

}
