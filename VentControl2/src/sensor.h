class Sensor{
    public:
        float value;    // The latest sensor reading
        float max;
        float min;
        float offset;
        int   pin;

        //String unit;
        virtual void read();
        float getValue();          // Returns the last read value of the sensor
        void setPin(int);
        void resetMinMax();
       // float direction();

    protected:
        void newValue(float);
};

class AnalogSensor: public Sensor{
    public:
        int sDly;
        int smoothing;
    protected:
        int doAdc(int);
};

class DigitalTemp: public Sensor{
    public:
        DigitalTemp();
        DigitalTemp(int);
        void setIndex(int);
        void read();
};

class AnalogTemp: public AnalogSensor{
    public:
        AnalogTemp();
        AnalogTemp(int);

        bool opAmp;
        float gain;

        void read();
};

class VoltageSensor: public Sensor{
    public:
        VoltageSensor();
        VoltageSensor(int);

        void read();
};


class CurrentSensor: public Sensor{
	/* 	Example code:
		https://wiki.dfrobot.com/20A_Current_Sensor_SKU_SEN0214#More */
	// The motor controller board cunsumes 65mA in standby.
    public:
        CurrentSensor();
        CurrentSensor(int); // 
        void read();    // reads sensor and stores it in value
};