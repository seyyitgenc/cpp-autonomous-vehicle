#pragma once

#include "motor_driver.hpp"
#include <memory>
#include <stack>
#include <thread>

class Vehicle
{
public:
    Vehicle(int speed = 255) : _speed(speed) {
        _mDriver.attachMotor(Motor::LEFT);
        _mDriver.attachMotor(Motor::RIGHT);
    }
    void setSpeed(int speed){ _speed = speed; }
    void auto_drive(float vel1, float vel2){
        _mDriver.forward(Motor::LEFT, _speed + vel1);
        _mDriver.forward(Motor::RIGHT, _speed - vel2);
        delay(30);
    };
    ~Vehicle() = default;

private:
    MotorDriver _mDriver;
    int _speed;
};

inline float threshold_time = 1.0f;
inline float total_time = 0.0f;