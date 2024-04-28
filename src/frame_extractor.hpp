#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include "sign_detection.hpp"
#include "utils/filesystem.hpp"

void frameExtractor(){
    std::string path = "/home/seyyit/dataset/0/";
    // Open the video file
    cv::VideoCapture cap(FileSystem::getPath("DataSetVideos/stopSign.mp4"));

    if (!cap.isOpened()) {
        std::cerr << "Error opening video file\n";
        return;
    }

    int frameNum = 0;
    cv::Mat frame, processed;

    while (true) {
        // Read the next frame
        cap >> frame;
        processed = preprocessSign(frame);
        auto redness = returnRedness(frame);
        try
        {
            auto contours = findContour(processed);
            auto big = findBiggestContour(contours);

            if (big.size() <= 0)
                continue;

            if (cv::contourArea(big) > 3000)
            {
                auto sign =  boundaryBox(frame, big);
                // auto result = model.prepareInputTensorAndPredict(sign);
                // resultToLabel(&result);
                        // Construct the filename
                cv::resize(sign, sign, {32,32});

                std::string filename = "frame" + std::to_string(frameNum) + ".jpg";
                std::string full_path = path + filename;
                // Save the frame as an image file
                cv::imwrite(full_path, sign);
                frameNum++;
                cv::imshow("sign",sign);
            }

        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        // If the frame is empty, then we have reached the end of the video
        if (frame.empty()) {
            break;
        }
    }

    // Release the video file
    cap.release();
}