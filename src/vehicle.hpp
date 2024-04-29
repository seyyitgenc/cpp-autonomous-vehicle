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
    ~Vehicle();

private:
    ModelHandler model;
};

inline constexpr int speed = 100;
// for error calculations
inline constexpr double k = 0.32;

// #include "vision.hpp"

float threshold_time = 1.0f;
float total_time = 0.0f;

void stop(){
    // std::this_thread::sleep_for(std::chrono::seconds(10));
    // Stopwatch<> t1;
    // float total_time = 0;
    // while (total_time < 10)
    //     total_time += t1.getElapsedTime<float>();
    // std::cout << "i sleeped 10 second" << std::endl;
}

void act_normal(){

}

void vehicle_behaviour(SIGN type, int distance){
    static int stop_threshold = 40;
    switch (type)
    {
    case SIGN::NONE:
        act_normal();
        break;
    case SIGN::STOP:
        if (distance < stop_threshold){
            // TODO: add stop here
            stop();
        }
        break;
    default:
        break;
    }
}
std::stack<SIGN> instructions;

void testVehicle(const std::string &model_path,const std::string &video_path){
    ModelHandler model(model_path);
    cv::VideoCapture source(0);
    LaneDetector detector(source.get(cv::CAP_PROP_FRAME_WIDTH), source.get(cv::CAP_PROP_FRAME_HEIGHT));
    cv::Mat frame, cut_gray;
    int error = 0;
    int vel1 = 0, vel2 = 0;
    double k = 0.32;
    int speed = 100;

    Stopwatch<> timer1;
    // Known parameters
    double knownWidth = 7; // Actual width of the object (in meters)
    double focalLength = 500; // Focal length of the camera (in pixels)
    while (true)
    {
        source >> frame;
        if (frame.empty())
            break;

        auto processed = preprocessSign(frame);
        try
        {
            stop();
            auto contours = findContour(processed);
            // auto top3 = findTopThreeContour(processed);
            // std::cout << contours.size() << std::endl;
            // for (size_t i = 0; i < top3.size(); i++)
            // {
            //     auto sign =  boundaryBox(frame, top3);
            // }
            
            auto big = findBiggestContour(contours);

            if (big.size() <= 0)
                continue;

            if (cv::contourArea(big) > 3000 && total_time >= threshold_time)
            {
                total_time = 0.0f;
                std::cout << "1 sec passed and object detected" << std::endl;                
                auto sign =  boundaryBox(frame, big);
                cv::imshow("traffic sign",sign);
                double objectWidthPixels = sign.cols;
                // Calculate the distance to the object
                double distance = (knownWidth * focalLength) / objectWidthPixels;
                std::cout << "distance in : " << distance << std::endl;
                if (distance < 40){
                    auto result = model.prepareInputTensorAndPredict(sign);
                    SIGN sign_type = resultToLType(&result);
                    // if (instructions.top() != sign_type || !instructions.empty()){
                    //     instructions.push(sign_type);
                    // }
                }
            }            
            // vehicle_behaviour()
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        cut_gray = detector.preprocess(frame);
        
        error = detector.calc_err(cut_gray) - 640;

        vel1 = speed + (error * k);
        vel2 = (speed - (error * k));

        // std::cout << "vel1:" << vel1 << ',' << "vel2:" << vel2 << ", error: " << error << std::endl;

        cv::imshow("frame", frame);
        if (cv::waitKey(10) >= 0)
            break;
        total_time += timer1.getElapsedTime<float>();
        // std::cout << timer1.getElapsedTime<float>() << std::endl;
        timer1.reset();
    }
}