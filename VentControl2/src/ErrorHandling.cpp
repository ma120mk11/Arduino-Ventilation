#include "ErrorHandling.h"

struct Error {
    bool flag = 0;              // Indicates that this error is active
    String errorText = "";      
    String msg = "";            
    int count = 0;
};

Error setError[30];

String getErrors() {
    int count = 0;
    String text = "--Error log--\n";
    for(int i = 0; i < 30; i++) {
        if (setError[i].flag) {
            count++;
            text += " -> (" + (String)setError[i].count + ") " + setError[i].errorText + ": " + setError[i].msg + " \n";
        }
    }
    if (count == 0) return "0 Errors.";
    else {
        return count + " active errors.\n" + text;
    }
}



void createError(int ErrorType, String msg = "")
{
    setError[ErrorType].count++;
    // Don't generate an error if that error is still active
    if (setError[ErrorType].flag) return;

    DateTime now = rtc.now();
    String date = String(now.day()) + "." + String(now.month()) + "." + String(now.year());
	String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    String errorText = "";
    
    switch (ErrorType)
    {
    case ERR_VOLTAGE:
        nextion_goToPage("voltage_error");
        errorText = "Voltage too low";
        break;
    case ERR_CURRENT:
        errorText = "Current";
        break;
    case ERR_MOTOR:
        errorText = "Motor didn't start";
        break;
    case ERR_WFIFI:
        errorText = "Wifi";
        break;
    case ERR_SD:
        errorText = "SD";
        break;
    case ERR_RTC:
        errorText = "RTC";
        break;
    case ERR_NEXTION:
        errorText = "Nextion";
        break;
    case ERR_TEMP_OUTSIDE:
        errorText = "Temperature sensor outside";
        break;
    case ERR_TEMP_PANEL:
        errorText = "Temperature sensor panel";
        break;
    case ERR_TEMP_AIR:
        errorText = "Temperature sensor heated air";
        break;
    case ERR_TEMP_INSIDE:
        errorText = "Temperature sensor inside";
        break;
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
        errorText = "Unknown error";
        break;
    }

    setError[ErrorType].errorText = errorText;
    setError[ErrorType].msg = msg;
    setError[ErrorType].flag = true;
    

    String message = "Error: " + (String)ErrorType + " " + errorText + " " + msg;
    errorPrint(message);

    // Avoid loop loggong sdcard error to sdcard
    if(ErrorType != ERR_SD){
        SD_errorlog(ErrorType, msg, setError[ErrorType].count, date, time);
    }
    
}


void clearError(int ErrorType){
    if (!setError[ErrorType].flag) {
        verboseDbln("Reset error " + ErrorType);
        setError[ErrorType].flag = false;
        setError[ErrorType].count = 0;
    }
}