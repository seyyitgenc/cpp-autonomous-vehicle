#ifndef _MOTOR_DRIVER_H_
#define _MOTOR_DRIVER_H_

#include "pins.h"
#include <iostream>

// TODO: test this library out to make sure everything works fine  
enum struct Motor{
    LEFT,
    RIGHT
};

class MotorDriver
{
public:
    MotorDriver(){
        // if (wiringPiSetup() == -1)
        //     std::cout << "error initializing wiring pi" << std::endl;
        // pwmSetRange(1024);
    }
    void attachMotor(Motor type) {};
    void forward(Motor type,  int speed){};
    void reverse(Motor type, int speed){};
    void stop(Motor type){};
private:
    bool _isMotorLeftAttached;
    bool _isMotorRightAttached;
private:
};
#endif // _MOTOR_DRIVER_H_