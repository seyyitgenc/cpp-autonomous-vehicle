#include <iostream>

#include <opencv2/opencv.hpp>
#include "utils/filesystem.hpp"
#include "vision.hpp"
#include "onnxmodelhandler.hpp"

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


int main(){
    // model
    ModelHandler model(FileSystem::getPath("src/model/sign.onnx"));
    
    // testONNXMOdel(FileSystem::getPath("src/model/sign.onnx"));
    // testModelHandler(FileSystem::getPath("src/model/sign.onnx"));
    
    cv::VideoCapture source(0);
    cv::Mat frame;

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
    }
    return 0;
}