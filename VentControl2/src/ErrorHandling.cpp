#include "ErrorHandling.h"

struct Error {
    bool active = 0;              // Indicates that this error is active
    String errorText = "";      
    String msg = "";            
    int count = 0;
};

Error setError[30];

int numOfCreatedErrors = 0;

int getCreatedErrorCount(){
    return numOfCreatedErrors;
}

void resetCreatedErrorCount(){
    numOfCreatedErrors = 0;
}

String getErrors(bool verbose = true) {
    int count = 0;
    String text = "--Error log--\n";
    for(int i = 0; i < 30; i++) {
        if (setError[i].active) {
            count++;
            if (verbose) text += " -> (" + (String)setError[i].count + ") " + "Type: " + (String)i  + " " + setError[i].errorText + ": " + setError[i].msg + " \n";
            else text += " -> (" + (String)setError[i].count + ") " + "Type: " + (String)i  + " \n";
        }
    }
    if (count == 0) return "0 Errors.";
    else {
        return (String)count + " active errors.\n" + text;
    }
}

String getErrorsOneline(bool verbose = true, bool ignoreThingspeak = false) {
    int count = 0;
    String text = "";
    for(int i = 0; i < 30; i++) {
        if (setError[i].active) {
            // 
            if (ignoreThingspeak && i == ERR_THINKSPEAK) continue;

            count++;
            // Print comma if more than one error
            if (count > 1) text += ", ";
            if (verbose) text += (String)i + " (" + getErrorTypeString(i) + ")" + " " + setError[i].errorText + ": " + setError[i].msg;
            else text += (String)i + "(" + getErrorTypeString(i) + ")";
        }
    }
    if (count == 0) return "0 Errors.";
    else {
        return (String)count + " active errors. " + text;
    }
}

String getErrorTypeString(int type) {
    String text = "";
    switch (type){
        case 1:
            text = "voltage";
            break;
        case 2:
            text = "current";
            break;
        case 3:
            text = "motor";
            break;
        case 4:
            text = "voltage sensor";
            break;
        case 5:
            text = "wifi";
            break;
        case 6:
            text = "SD";
            break;
        case 7:
            text = "RTC";
            break;
        case 8:
            text = "nextion";
            break;
        case 9:
            text = "sensor outside";
            break;
        case 10:
            text = "sensor panel";
            break;
        case 11:
            text = "sensor heated air";
            break;
        case 12:
            text = "sensor inside";
            break;
        case 13:
            text = "thingspeak";
            break;
        default:
            text = "unknown";
            break;
    }    
    return text;
}

void createError(int ErrorType, String msg = "") {
    setError[ErrorType].count++;
    // Don't generate an error if that error is still active
    if (setError[ErrorType].active) return;

    DateTime now = rtc.now();
    String date = String(now.day())  + "." + String(now.month())  + "." + String(now.year());
	String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    String errorText = "";
    
    switch (ErrorType)
    {
    case ERR_VOLTAGE:
        nextion_goToPage("voltage_error");
        errorText = "Voltage too low";
        break;
    case ERR_VOLTAGE_SENSOR:
        errorText = "Voltage sensor";
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
        nextion_update("sd_card_sett.sdStatus.txt=", "Error");
	    nextion_update("data.sd_status.val=", 0);		        // Update sd status
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

    numOfCreatedErrors++;

    setError[ErrorType].errorText = errorText;
    setError[ErrorType].msg = msg;
    setError[ErrorType].active = true;

    String message = "Error: " + (String)ErrorType + " " + errorText + " " + msg;
    errorPrint(message);

    // Avoid loop logging sdcard error to sdcard
    if(ErrorType != ERR_SD){
        SD_errorlog(ErrorType, msg, setError[ErrorType].count, date, time);
    }
}

bool isActiveErrorType(int type) {
    return setError[type].active;
}

void clearError(int ErrorType){
    if (setError[ErrorType].active) {
        verboseDbln("Resetting error " + (String)ErrorType);
        setError[ErrorType].active = false;
        setError[ErrorType].count = 0;
    }
}