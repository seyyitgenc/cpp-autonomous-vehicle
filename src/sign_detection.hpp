#pragma once
#include <opencv2/opencv.hpp>

class SignDetector
{
public:
    SignDetector()  = default;
    ~SignDetector() = default;
    
private:
};

cv::Mat returnRedness(const cv::Mat &img){
    cv::Mat yuv; 
    cv::cvtColor(img, yuv, cv::COLOR_BGR2YUV);
    std::vector<cv::Mat> components{};
    cv::split(yuv, components);
    return components[2];
}

cv::Mat returnBlueness(const cv::Mat &img){
    cv::Mat yuv{}; 
    cv::cvtColor(img, yuv, cv::COLOR_BGR2YUV);
    std::vector<cv::Mat> components{};
    cv::split(yuv, components);
    return components[1];
}

cv::Mat preprocessBlue(const cv::Mat &img){
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

cv::Mat preprocessSign(const cv::Mat& img){
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

cv::Mat threshold(const cv::Mat &img, int thresh = 150){
    cv::Mat output;
    cv::threshold(img, output, thresh, 255, cv::THRESH_BINARY);
    return output.clone();
}

std::vector<cv::Mat> findContour(const cv::Mat &img){
    std::vector<cv::Mat> contours;
    cv::findContours(img, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    return contours;
}

// NOTE: this could be wrong
std::vector<cv::Point> findBiggestContour(const std::vector<cv::Mat> &contours){
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
cv::Mat boundaryBox(const cv::Mat &img, const std::vector<cv::Point> &contour){
    auto rect = cv::boundingRect(contour);
    cv::rectangle(img, rect, {0,255,0}, 2);
    cv::Mat roi = img(rect);
    return roi.clone();
}

cv::Mat preprocessingImageToClassifier(const cv::Mat &img, int imgSize = 32){
    cv::Mat gray_img, resized_img, equalized_img, normalized_img; 
    cv::cvtColor(img, gray_img, cv::COLOR_RGB2GRAY);

    // cv::resize(gray_img,resized_img,{imgSize, imgSize}, 2, 2);
    // cv::equalizeHist(resized_img, equalized_img);
    // cv::normalize(equalized_img, normalized_img, 0, 1, cv::NORM_MINMAX, CV_32F);

    return gray_img;   
}
