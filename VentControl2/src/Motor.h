#ifndef MOTOR_H
#define MOTOR_H

/**
 * Speed value 0-5.
 * Calls sysValUpdate() to update display
 * It increases or decreases the motor speed gradually until it reaches the value.  
*/

class Motor{
    private:
		int speed;
		int PWM;
		int dly = 30;               // How fast the motor goes from one value to another
		int output_pin = 9;
		int max = 0;
        unsigned long startMillis;

        // PWM SETTINGS :
        static const int s1 = 70;				// motor PWM value at setting 1
        static const int s2 = 100;				// motor PWM value at setting 2
        static const int s3 = 150;				// motor PWM value at setting 3
        static const int s4 = 210;				// motor PWM value at setting 4
        static const int s5 = 255;				// motor PWM value at setting 5

    public:
        Motor(int pin);         // Constructor
        /**
         * @param speed Range 0 - 5
         */
        void setSpeed(int speed);     // 
        int getSpeed();         // Returns the current motor speed (int)
        void setPin(int pin);
        int getMax();

        /**
         * TODO: millis overflow logic
         * @return motor on time in minutes
         */
        int getTimeOn();
        bool isRunning = 0;

};

#endif