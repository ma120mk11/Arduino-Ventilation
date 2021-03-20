#ifndef SENSOR_H
#define SENSOR_H
#define pinReference 5

#include <Arduino.h>

class Sensor {
    public:
		float value = 0;    // The latest sensor reading
		float max 	= 0;
		float min 	= 0;
		float offset= 0;
		int pin 	= 0;

		String unit = "";
		//virtual void read() =0;
        ~Sensor();

        void read();
        float getValue();          // Returns the last read value of the sensor
        void setPin(int);
        void resetMinMax();
       // float direction();

    protected:
        void newValue(float);
    private:
        float values[10];    
};

class AnalogSensor: public Sensor {
	public:
		int sDly 		= 20;		// Smoothing delay
		int smoothing	= 10;
        int doAdc(int);
};

class DigitalTemp: public Sensor {
    public:
        DigitalTemp();
        DigitalTemp(int);
        void setIndex(int);
        void read();
    private:
        int index = 0;
};

class AnalogTemp: public AnalogSensor {
    public:
        AnalogTemp();
        AnalogTemp(int);
        bool opAmp = false;
		float gain = 10.9395;		// For sensors with opAmps
        void read();
    private:

};

class VoltageSensor: public AnalogSensor {
    public:
        VoltageSensor(int);
        void read();
};

class CurrentSensor: public AnalogSensor {
	/* 	Example code:
		https://wiki.dfrobot.com/20A_Current_Sensor_SKU_SEN0214#More */
	// The motor controller board cunsumes 65mA in standby.
    public:
        CurrentSensor();
        CurrentSensor(int);     // 
        void read();            // reads sensor and stores it in value

    private:
        static const int mVperAmp = 10;		// 20A module
};

#endif