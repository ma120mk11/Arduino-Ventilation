#include "ErrorHandling.h"

struct Error {
    bool flag = 0;
    String msg = "";
    int count = 0;
};

Error setError[30];

void createError(int ErrorType, String msg = "")
{
    DateTime now = rtc.now();
    // String date = String(now.day()) + "." + String(now.month()) + "." + String(now.year());
	// String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    String errorText = "";
    
    switch (ErrorType)
    {
    case ERR_VOLTAGE:
        nextion_goToPage("voltage_error");
        break;
    case ERR_CURRENT:
        break;
    case ERR_MOTOR:
    case ERR_WFIFI:
    case ERR_SD:
    case ERR_RTC:
    case ERR_NEXTION:
    case ERR_TEMP_OUTSIDE:
    case ERR_TEMP_PANEL:
    case ERR_TEMP_AIR:
    case ERR_TEMP_INSIDE:

    case ERR_THINKSPEAK:
        switch (msg.toInt())
        {
        case 404:
            errorText= "incorrect API key";
            break;
        case -101:
            errorText = "value out of range or string is too long";
            break;
        case -210:
            errorText ="setField() was not called before writeFields()";
            break;
        case -301:
            errorText ="Failed to connect to ThingSpeak";
            break;
        case -302:
            errorText ="Unexpected failure during write to ThingSpeak";
            break;
        case -303:
            errorText ="Unable to parse response";
            break;
        case -304:
            errorText = "Timeout waiting for server to respond";
            break;
        case -401:
            errorText= "Point was not inserted (rate lilit of 15s?)";
            break;
        default:
            errorText = "unknown error";
            break;
        }
        nextion_update("wifi_error.message.txt", errorText);
        nextion_goToPage("wifi_error");
        break;
  
    default:

        break;
    }

    setError[ErrorType].flag = true;
    setError[ErrorType].count++;

    String message = "Error: " + (String)ErrorType + " " + errorText + " " + msg;
    errorPrint(message);

    // Avoid loop loggong sdcard error to sdcard
    if(ErrorType != ERR_SD){
        SD_errorlog(ErrorType, msg, setError[ErrorType].count);
    }
    
}


void clearError(int ErrorType){
    verboseDbln("Reset error " + ErrorType);
    setError[ErrorType].flag = false;
    setError[ErrorType].count = 0;
}