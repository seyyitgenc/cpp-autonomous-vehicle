#pragma once

#include "onnx_model_handler.hpp"
#include "motor_driver.hpp"
#include <memory>
#include "vision.hpp"
#include <stack>
#include "utils/stopwatch.hpp"
#include <thread>

class Vehicle
{
public:
    Vehicle(const std::string &model_path) : model(model_path){

    }
    ~Vehicle() = default;

private:
    ModelHandler model;
};

inline constexpr int speed = 100;
// for error calculations
inline constexpr double k = 0.32;

// #include "vision.hpp"

float threshold_time = 1.0f;
float total_time = 0.0f;