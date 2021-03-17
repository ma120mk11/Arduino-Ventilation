
/*  
Speed value 0-5.
Calls sysValUpdate() to update display
It increases or decreases the motor speed gradually until it reaches the value.
*/

class Motor{
    public:
        Motor();
        Motor(int pin);
        void setSpeed(int); // 
        int getSpeed();        // Returns the current motor speed (int)
        void setPin(int);   
        int getMax();       // @TODO: Not implemented
    private:

};
