#include "sensor.h"


void Sensor::setPin(int rPin) { pin = rPin; }
float Sensor::getValue() { return value; }
void Sensor::resetMinMax(){ 
	min = value; 
	max = value; 
}


void Sensor::newValue(float val) {
	value = val;
	int lenght = 10;
	for (int i = lenght; i > 0; i--) {
		values[i] = values[i-1];
	}
	values[0] = val;

	// Update min and max value
	if (val > max) max = val;
	if (val < min) min = val;
}


int AnalogSensor::doAdc(int pin) {
	int n = smoothing;
	float raw = 0;
	while (n > 0) {
		raw += analogRead(pin);
		n--;
		delay(sDly);
	}
	n = smoothing;			// Restore n value
	raw = raw / n;			// Calculate mean value
	return raw;
}    



DigitalTemp::DigitalTemp() {
	unit = "Celsius";
}
DigitalTemp::DigitalTemp(int rPin) { 
	pin = rPin; 
	unit = "Celsius";
}

void DigitalTemp::setIndex(int rIndex) { index = rIndex; }

void DigitalTemp::read(){
	OneWire oneWirePin(pin);
	DallasTemperature sensors(&oneWirePin);	
	sensors.begin();
	sensors.requestTemperatures();
	newValue(sensors.requestTemperaturesByIndex(index));
}



//AnalogTemp::AnalogTemp();
AnalogTemp::AnalogTemp(int rPin) { pin = rPin; }

void AnalogTemp::read() {
	int adc = doAdc(pin);
	float temp = 0;
	// convert to celsius:
	if (!opAmp) {
		temp = ((adc * pinReference / 1024.0 ) / 0.01 ) + offset;
	}
	else {
		temp = ((adc * pinReference / 1024.0 ) / 0.01 ) / gain + offset;
	}

	// Round to 1 decimal. round() rounds decimal number to whole number. 
	temp = round(temp * 10.0) / 10.0;
	newValue(temp);
}



VoltageSensor::VoltageSensor(int rPin) { pin = rPin; }

void VoltageSensor::read() {
	int adc = doAdc(pin);
	// Convert to voltage
	float temp = (adc * pinReference / 1024.0) * 5.0;
	temp = round(temp * 10.0) / 10.0;
	newValue(temp);
}

CurrentSensor::CurrentSensor(int rPin) { pin = rPin; }

void CurrentSensor::read() {
	int adc = doAdc(pin);
	float temp = (adc * pinReference / 1024.0);
	temp = temp - pinReference / 2.0;		// Adjust for 0A
	temp = temp * mVperAmp;

	newValue(temp);
	//current = c 	//round(c * 10.0) / 10.0;
}

/*		
//	********************** LIGHT SENSOR **************************

	light = (l0 / light_max_Reading) * 100;



//	*********************** SERIAL PRINT ***************************
//  Message is formatted for use with Telemetry viewer. Use a layout2 in Telemetry folder.
// TODO: add light sensor

if (enableSerialPrint == 1){
	
	char outside_text[30];
	char panel_text[30];
	char air_text[30];
	char room_text[30];
	char filtered_text[30];
	char voltage_text[30];
	char current_text[30];
	char tdelta_text[30];
	char m1speed_text[30];

	dtostrf(temp0C, 10, 10, outside_text);
	dtostrf(temp1C, 10, 10, panel_text);
	dtostrf(temp2C, 10, 10, air_text);
	dtostrf(temp3C, 10, 10, room_text);
	dtostrf(filteredSignal, 10, 10, filtered_text);
	dtostrf(voltage, 10, 10, voltage_text);
	dtostrf(current, 10, 10, current_text);
	dtostrf(tempDelta, 10, 10, tdelta_text);
	dtostrf(M1Speed, 10, 10, m1speed_text);

	char text[280];
	snprintf(text, 280, "%s,%s,%s,%s,%s,%s,%s,%s,%s", outside_text, panel_text, air_text, room_text, voltage_text, current_text,filtered_text,tdelta_text,m1speed_text);
	Serial.println(text);
}	
*/