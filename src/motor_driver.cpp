#include "motor_driver.hpp"
// #include <>

void MotorDriver::attachMotor(Motor type){
    switch (type)
    {
    case Motor::LEFT:
        // pinMode(PWMA,OUTPUT);
        // pinMode(AIN1,OUTPUT);
        // pinMode(AIN2,OUTPUT);
        _isMotorLeftAttached = true;
        break;
    case Motor::RIGHT:
        // pinMode(PWMB,OUTPUT);
        // pinMode(BIN1,OUTPUT);
        // pinMode(BIN2,OUTPUT);
        _isMotorRightAttached = true;
        break;
    default:
        break;
    }
}

void MotorDriver::forward(Motor type, int speed){
    switch (type){
    case Motor::LEFT:
        if (_isMotorLeftAttached){
            // digitalWrite(AIN1, HIGH);
            // digitalWrite(AIN2, LOW);
            // analogWrite(PWMA, speed);
        }
        break;
    case Motor::RIGHT:
        if (_isMotorRightAttached){
            // digitalWrite(BIN1, HIGH);
            // digitalWrite(BIN2, LOW);
            // analogWrite(PWMB, speed);
        }
        break;
    default:
        // Serial.println("undefined motor type");
        break;
    }
}

void MotorDriver::reverse(Motor type, int speed){
    switch (type)
    {
    case Motor::LEFT:
        // digitalWrite(AIN1, LOW);
        // digitalWrite(AIN2, HIGH);
        // analogWrite(PWMA, speed);
        break;
    // case Motor::RIGHT:
    //     digitalWrite(BIN1, LOW);
    //     digitalWrite(BIN2, HIGH);
    //     analogWrite(PWMB, speed);
        break;
    default:
        // Serial.println("undefined motor type");
        break;
    }
}

void MotorDriver::stop(Motor type){
    switch (type)
    {
    case Motor::LEFT:
        // digitalWrite(AIN1, LOW);
        // digitalWrite(AIN2, LOW);
        break;
    case Motor::RIGHT:
        // digitalWrite(BIN1, LOW);
        // digitalWrite(BIN2, LOW);
        break;
    default:
        // Serial.println("undefined motor type");
        break;
    }
}
