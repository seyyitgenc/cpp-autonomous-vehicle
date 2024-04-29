#include "sign_detection.hpp"

cv::Mat SignDetector::preprocessBlue(const cv::Mat &img){
    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    // Blue color range
    cv::Scalar lower_blue = cv::Scalar(100, 150, 0);
    cv::Scalar upper_blue = cv::Scalar(130, 255, 255);

    cv::Mat mask;
    cv::inRange(hsv, lower_blue, upper_blue, mask);

    cv::Mat result;
    cv::bitwise_and(img, img, result, mask);
    cv::cvtColor(result,result, cv::COLOR_BGR2GRAY);
    return result;
}

cv::Mat SignDetector::preprocessSign(const cv::Mat& img){
    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    // Lower red range
    static cv::Scalar lower_red1 = cv::Scalar(0, 100, 100);
    static cv::Scalar upper_red1 = cv::Scalar(10, 255, 255);

    // Upper red range
    static cv::Scalar lower_red2 = cv::Scalar(160, 120, 70);
    static cv::Scalar upper_red2 = cv::Scalar(180, 255, 255);

    // // Blue color range
    // static cv::Scalar lower_blue = cv::Scalar(100, 150, 0);
    // static cv::Scalar upper_blue = cv::Scalar(130, 255, 255);


    cv::Mat mask1, mask2, mask3, mask;
    cv::inRange(hsv, lower_red1, upper_red1, mask1);
    cv::inRange(hsv, lower_red2, upper_red2, mask2);
    // cv::inRange(hsv, lower_blue, upper_blue, mask3);
    
    cv::bitwise_or(mask1, mask2, mask);
    // cv::bitwise_or(mask, mask3, mask);
    static cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(mask, mask, element);
    cv::GaussianBlur(mask, mask, cv::Size(9, 9), 3, 3);
    cv::imshow("testwindow", mask);
    return mask;
}

cv::Mat SignDetector::threshold(const cv::Mat &img, int thresh){
    cv::Mat output;
    cv::threshold(img, output, thresh, 255, cv::THRESH_BINARY);
    return output.clone();
}

std::vector<cv::Mat> SignDetector::findContour(const cv::Mat &img){
    std::vector<cv::Mat> contours;
    cv::findContours(img, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    return contours;
}

// NOTE: this could be wrong
std::vector<cv::Point> SignDetector::findBiggestContour(const std::vector<cv::Mat> &contours){
    double maxArea = 0;
    std::vector<cv::Point> maxAreaContour;
    for (size_t i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea)
        {
            maxArea = area;
            maxAreaContour = contours[i];
        }
    }
    return maxAreaContour;
}
// NOTE: due to that this could be wrong aswell
cv::Mat SignDetector::boundaryBox(const cv::Mat &img, const std::vector<cv::Point> &contour){
    auto rect = cv::boundingRect(contour);
    cv::rectangle(img, rect, {0,255,0}, 2);
    cv::Mat roi = img(rect);
    return roi.clone();
}