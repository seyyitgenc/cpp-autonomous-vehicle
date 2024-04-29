#pragma once
#include <opencv2/opencv.hpp>

class SignDetector
{
public:
    SignDetector()  = default;
    ~SignDetector() = default;
    
    cv::Mat preprocessBlue(const cv::Mat &img);

    cv::Mat preprocessSign(const cv::Mat& img);

    cv::Mat threshold(const cv::Mat &img, int thresh = 150);

    std::vector<cv::Mat> findContour(const cv::Mat &img);
    // NOTE: this could be wrong
    std::vector<cv::Point> findBiggestContour(const std::vector<cv::Mat> &contours);
    // NOTE: due to that this could be wrong aswell
    cv::Mat boundaryBox(const cv::Mat &img, const std::vector<cv::Point> &contour);
private:
};
