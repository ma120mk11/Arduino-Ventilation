#ifndef SENSOR_H
#define SENSOR_H
#define pinReference 5

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>
// #include "ErrorHandling.h"

class Sensor {
    public:
		float value;    // The latest sensor reading
		float max;
		float min;
		float offset;
		int pin;
		String unit;
        
        /**
         * Reads sensor and store it in value. 
         * To get latest reading use getValue().
         * @return Nothing.
         */
        bool read();

        /**
         * @return the last read value of the sensor in correct unit.
         */
        float getValue();
        void setPin(int);
        void resetMinMax();

        /**
         * https://classroom.synonym.com/calculate-trendline-2709.html
         * 
         * @return Slope of the trendline in unit/min
         */
        float getSlope();
       ~Sensor();
        void newValue(float);
    protected:
        bool checkValue(float, String);        
        bool isInRange(float,float,float);
    private:

        static const int arrayLenght = 30;  //12
        float values[arrayLenght];    
};

class AnalogSensor: public Sensor {
	public:
		int sDly 		= 20;		// Smoothing delay
		int smoothing	= 10;
    protected:
        int doAdc(int);
};

class DigitalTemp: public Sensor {
    
    public:
        /**
         * @param rPin Sensor bus pin
         */
        DigitalTemp(int rPin);
        
        /**
        * @param rPin Sensor digital pin
        * @param rIndex Sensor index
        */
        DigitalTemp(int rPin, int rIndex);
        void setIndex(int);
        static void tempInit();
        bool read();
        OneWire oneWirePin;
	    DallasTemperature sensors;	
    private:
        int index;  
};

class AnalogTemp: public AnalogSensor {
    public:
        AnalogTemp();
        AnalogTemp(int);
        bool opAmp = false;
		float gain = 10.9395;		// For sensors with opAmps
        bool read();
};

class VoltageSensor: public AnalogSensor {
    public:
        VoltageSensor(int);
        bool read();
};

class CurrentSensor: public AnalogSensor {
	/* 	Example code:
		https://wiki.dfrobot.com/20A_Current_Sensor_SKU_SEN0214#More
	    - The motor controller board cunsumes 65mA in standby.
    */
    public:
        CurrentSensor();
        CurrentSensor(int);     // 
        bool read();            // reads sensor and stores it in value

    private:
        static const int mVperAmp = 10;		// 20A module
};

#endif