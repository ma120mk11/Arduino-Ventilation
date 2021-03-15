#pragma once

#include "filter.h"
#include "settings.h"
#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>


// Global Variables:
//extern bool enableSerialPrint, m1Running;
//extern float temp0C, temp1C, temp2C, temp3C, temp4C, voltage, current, tempDelta, derivate;
//extern float v0_1, v0_2, i0_1, i0_2;
//extern float temp0C_max, temp1C_max, temp2C_max, temp3C_max, temp4C_max, voltage_min, voltage_max, current_max, tempDelta_max;
//extern int M1Speed, light;
//extern float filteredSignal;

class Sensor {
	public:
		float value = 0;
		float max 	= 0;
		float min 	= 0;
		float offset= 0;
		int pin 	= 0;


		String unit = "";
		virtual void read() =0;

		void setPin(int rPin) { pin = rPin; }
		float value() { return value; }
		void resetMinMax(){ 
			min = value; 
			max = value; 
		}

		float direction(){
			
		}

	protected:

		void newValue(float val) {
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

	private:
		float values[10];

		
};


class AnalogSensor : public Sensor {
	public:
		int sDly 		= 20;		// Smoothing delay
		int smoothing	= 10;
	protected:
		
		int doAdc(int pin) {
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
};




class DigitalTemp : public Sensor {


	public:
		DigitalTemp() {
			unit = "Celsius";
		}
		DigitalTemp(int rPin) { 
			pin = rPin; 
			unit = "Celsius";
		}

		void setIndex(int rIndex) { index = rIndex; }

		void read(){
			OneWire oneWirePin(pin);
			DallasTemperature sensors(&oneWirePin);	
			sensors.begin();
			sensors.requestTemperatures();
			newValue(sensors.requestTemperaturesByIndex(index));
		}

	private:
		int index = 0;
};


class AnalogTemp : public AnalogSensor {
	public:
		AnalogTemp();
		AnalogTemp(int rPin) { pin = rPin; }

		bool opAmp = false;
		float gain = 10.9395;		// For sensors with opAmps

		void read() {
			int adc = doAdc(pin);
			float temp = 0;
			// convert to celsius:
			if (!opAmp) {
				float temp = ((adc * pinReference / 1024.0 ) / 0.01 ) + offset;
			}
			else {
				float temp = ((adc * pinReference / 1024.0 ) / 0.01 ) / gain + offset;
			}

			// Round to 1 decimal. round() rounds decimal number to whole number. 
			temp = round(temp * 10.0) / 10.0;
			newValue(temp);
		}
};


class VoltageSensor: public AnalogSensor {
	public:
		VoltageSensor();
		VoltageSensor(int rPin) { pin = rPin; }
		
		void read() {
			int adc = doAdc(pin);
			// Convert to voltage
			float temp = (adc * pinReference / 1024.0) * 5.0;
			temp = round(temp * 10.0) / 10.0;
			newValue(temp);
		}
};

class CurrentSensor: public AnalogSensor {
	
	public:
		CurrentSensor();
		CurrentSensor(int rPin) { pin = rPin; }

		void read() {
			int adc = doAdc(pin);
			float temp = (adc * pinReference / 1024.0);
			temp = temp - pinReference / 2.0;		// Adjust for 0A
			temp = temp * mVperAmp;

			newValue(temp);
			//current = c 	//round(c * 10.0) / 10.0;
		}
	private:
		static const int mVperAmp = 10;		// 20A module
};

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