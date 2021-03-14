class Sensor{
    public:
        float value;
        float max;
        float min;
        float offset;
        int   pin;

        String unit;
        virtual void read();
        void setPin(int);
        void resetMinMax();
        float direction();

    protected:
        void newValue(float);
};

class AnalogSensor{
    public:
        int sDly;
        int smoothing;
    protected:
        int doAdc(int);
};

class DigitalTemp{
    public:
        DigitalTemp();
        DigitalTemp(int);
        void setIndex(int);
        void read();
};
class AnalogTemp{
    public:
        AnalogTemp();
        AnalogTemp(int);

        bool opAmp;
        float gain;

        void read();
};

class VoltageSensor{
    VoltageSensor();
    VoltageSensor(int);

    void read();
};


class CurrentSensor{
	/* 	Example code:
		https://wiki.dfrobot.com/20A_Current_Sensor_SKU_SEN0214#More */
	// The motor controller board cunsumes 65mA in standby.
    public:
        void read();
};