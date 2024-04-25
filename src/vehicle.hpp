#pragma once

#include "onnxmodelhandler.hpp"
#include <memory>

inline constexpr int speed = 100;
// for error calculations
inline constexpr int error = 320;
inline constexpr double k = 0.32;

// #include "vision.hpp"

class Vehicle
{
public:
    Vehicle(ModelHandler* model) : _model(model){};
    ~Vehicle() = default;

private:
    std::unique_ptr<ModelHandler> _model; // handler pointer
};