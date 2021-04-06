#include "sensor.h"

//#include "settings.h"


void Sensor::setPin(int rPin) { pin = rPin; }
float Sensor::getValue() { return value; }
void Sensor::resetMinMax(){ 
	min = value; 
	max = value; 
}


void Sensor::newValue(float val) {
	value = val;
	
	for (int i = arrayLenght-1; i > 0; i--) {
		values[i] = values[i-1];
	}
	values[0] = val;

	// Update min and max value
	if (val > max) max = val;
	if (val < min) min = val;
}

float Sensor::getSlope() {
	/** https://classroom.synonym.com/f-value-statistics-6039.html
	 *  
	 * The array has the latest read value att position [0].
	 * In this case we want it in the opposite direction 
	*/
	float valArray[arrayLenght];
	
	// invert array
	for(int i=0, n = arrayLenght-1; i < arrayLenght; i++, n--){
		valArray[i] = values[n];
	}

	int n = arrayLenght;			// How many values to make the trendline slope for
	double A = 0;
	double B = 0;
	double C = 0;
	double D = 0;
	double Bx = 0;
	double By = 0;
	double slope = 0;

	for(int i = 0, x=1; i <= n-1; i++, x++) {
		A += valArray[i] * x;
		Bx += x;
		By += valArray[i];
		C += x*x;
		D += x;
	}
	A *= n;
	B = Bx * By;
	C = n * C;
	D = D * D;
	slope = (A - B) / (C - D);
	return (float)slope;
}

bool Sensor::isInRange(float value, float a, float b) {
	if (value >= a && value <= b) return true;
	else return false;
}


bool Sensor::checkValue(float valToCheck, String type) {
	bool inRange;
	if (type == "voltage") { inRange = isInRange(valToCheck, -1, 20); }
	if (type == "current") { inRange = isInRange(valToCheck, -1, 20); }
	if (type == "temp") { inRange = isInRange(valToCheck, -50, 100); }

	if(abs(values[1] - valToCheck) < 15 && inRange) return true;
	else return false;
}

Sensor::~Sensor(){}

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

// Create object
DigitalTemp::DigitalTemp(int rPin, int rIndex) {
	pin = rPin;
	index = rIndex;
	unit = "Celsius";
}

DigitalTemp::DigitalTemp(int rPin) { 
	pin = rPin; 
	unit = "Celsius";
}

void DigitalTemp::tempInit(){
	extern DallasTemperature sensors;
	sensors.begin();
}

void DigitalTemp::setIndex(int rIndex) { index = rIndex; }

bool DigitalTemp::read() {
	extern DallasTemperature sensors;
	sensors.requestTemperaturesByIndex(index);
	float temp = sensors.getTempCByIndex(index);
	newValue(temp);
	return checkValue(temp, "temp");
}




AnalogTemp::AnalogTemp(int rPin) { pin = rPin; }

bool AnalogTemp::read() {
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
	return checkValue(temp, "temp");
}



VoltageSensor::VoltageSensor(int rPin) { pin = rPin; }

bool VoltageSensor::read() {
	int adc = doAdc(pin);
	// Convert to voltage
	float temp = (adc * pinReference / 1024.0) * 5.0;
	temp = round(temp * 10.0) / 10.0;
	newValue(temp);
	return checkValue(temp, "voltage");
}

CurrentSensor::CurrentSensor(int rPin) { pin = rPin; }

bool CurrentSensor::read() {
	int adc = doAdc(pin);
	float temp = (adc * pinReference / 1024.0);
	temp = temp - pinReference / 2.0;		// Adjust for 0A
	temp = temp * mVperAmp;
	temp = round(temp * 10.0) / 10.0;
	newValue(temp);
	return checkValue(temp, "current");
}