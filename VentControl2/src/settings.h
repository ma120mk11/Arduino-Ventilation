//************************    PINS    *********************************

// USE TX2 for Nextion display communication

#define Motor1 4            // motor1 pin HEATED AIR
#define Motor2 3			// motor2 pin DIRECT AIR

#define TempSensor0 A4		// LM35 temp sensor0 pin	OUTSIDE TEMP
#define TempSensor1 A1   	// LM35 temp sensor1 pin  	PANEL TEMP
#define TempSensor2 A2   	// LM35 temp sensor2 pin	HOT AIR
#define TempSensor3 A3   	// LM35 temp sensor3 pin	LIVING ROOM
#define TempSensor4 A5 
#define CurrentSensor A7    // Current sensor pin
#define VoltageSensor A8    // Voltage sensor pin

#define chipSelect 53		// pin for SD card chip select (see SD_card.h)
#define pinReference 5      // The measured value of +5V pin
