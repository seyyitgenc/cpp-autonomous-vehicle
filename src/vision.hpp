#ifndef __VISION_HPP__
#define __VISION_HPP__

#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <opencv2/opencv.hpp>

int calc_err(cv::Mat gray_img);
cv::Mat preprocess(cv::Mat input);
void testLaneFollower(const std::string& name);
#endif