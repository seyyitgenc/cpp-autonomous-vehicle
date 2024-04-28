#pragma once

#include "onnx_model_handler.hpp"
#include "motor_driver.hpp"
#include <memory>
#include "vision.hpp"

inline constexpr int speed = 100;
// for error calculations
inline constexpr double k = 0.32;

// #include "vision.hpp"

class Vehicle
{
public:
    Vehicle(){
        driver.attachMotor(Motor::LEFT);
        driver.attachMotor(Motor::RIGHT);
    };
    void control(cv::Mat img){
        int error = calc_err(img) - 320;
        vel1 = speed + (error * k);
        vel2 = speed - (error * k);
        std::cout << "vel1:" << vel1 << ',' << "vel2:" << vel2 << ", error: " << error << std::endl;

        driver.forward(Motor::LEFT, vel1);
        driver.forward(Motor::RIGHT, vel2);
    }
    ~Vehicle() = default;
private:
    MotorDriver driver;
    int vel1, vel2;
};

void testVehicle(const std::string &model_path,const std::string &video_path){
    ModelHandler model(model_path);
    cv::VideoCapture source(video_path);
    cv::Mat frame, cut_gray;
    int error = 0;
    int vel1 = 0, vel2 = 0;
    double k = 0.32;
    int speed = 100;
    while (true)
    {
        source >> frame;
        // frame = preprocessingImageToClassifier(frame);
        auto redness = returnRedness(frame);
        auto red_region = threshold(redness);
        try
        {
            auto contours = findContour(red_region);
            auto big = findBiggestContour(contours);

            if (big.size() <= 0)
                continue;

            if (cv::contourArea(big) > 3000)
            {
                auto sign =  boundaryBox(frame, big);
                auto result = model.prepareInputTensorAndPredict(sign);
                resultToLabel(&result);
                // cv::imshow("sign",sign);
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        cv::imshow("frame", frame);
        if (cv::waitKey(1) >= 0)
            break;
        if (frame.empty())
            break;
        cut_gray = preprocess(frame);
        
        error = calc_err(cut_gray) - 640;
        
        vel1 = speed + (error * k);
        vel2 = -(speed - (error * k));

        std::cout << "vel1:" << vel1 << ',' << "vel2:" << vel2 << ", error: " << error << std::endl;

        cv::imshow("frame", frame);
        if (cv::waitKey(30) >= 0)
            break;
    }
        
}