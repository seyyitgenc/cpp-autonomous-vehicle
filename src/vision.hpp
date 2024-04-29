#ifndef __VISION_HPP__
#define __VISION_HPP__

#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <opencv2/opencv.hpp>

class LaneDetector
{
public:
    LaneDetector(int width, int height) 
    : _width(width), _height(height), _cut_height(_height / _height_scale), _cut_y(_height - _cut_height) {
    };
    ~LaneDetector() = default;

    int calc_err(cv::Mat gray_img);
    cv::Mat preprocess(cv::Mat input);

private:
    int _height;
    int _width;
    int _cut_height;
    int _cut_y;
    int distance_from_line = 120;
    static constexpr float _height_scale = 4.0f;
};

// int calc_err(cv::Mat gray_img);
// cv::Mat preprocess(cv::Mat input);
void testLaneDetector(const std::string& name);
#endif