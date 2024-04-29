#include <iostream>

#include "utils/filesystem.hpp"
#include "vision.hpp"
#include "onnx_model_handler.hpp"
#include "sign_detection.hpp"
#include "vehicle.hpp"
#include "frame_extractor.hpp"

int main(){
    // auto source = cv::VideoCapture(FileSystem::getPath("videos/test2.mp4"));
    // frameExtractor();

    testLaneDetector("/home/seyyit/projects/cpp-autonomous-vehicle/videos/test10.mp4");
    // testLaneFollower("/home`/seyyit/projects/cpp-autonomous-vehicle/videos/res.mp4");
    // testVehicle(FileSystem::getPath("src/model/sign.onnx"),FileSystem::getPath("videos/test10.mp4"));
    // model
    // Vehicle car;

    // ModelHandler model(FileSystem::getPath("src/model/sign.onnx"));
    // cv::VideoCapture source(FileSystem::getPath("videos/leftturn.mp4"));
    // // // cv::VideoCapture source(0);
    // cv::Mat frame, processed;

    // while (true)
    // {
    //     source >> frame;
    //     // frame = preprocessingImageToClassifier(frame);
    //     processed = preprocessSign(frame);
    //     cv::imshow("processed ", processed);
    //     // auto redness = returnRedness(frame);
    //     try
    //     {
    //         auto contours = findContour(processed);
    //         auto big = findBiggestContour(contours);

    //         if (big.size() <= 0)
    //             continue;

    //         if (cv::contourArea(big) > 3000)
    //         {
    //             auto sign =  boundaryBox(frame, big);

    //             auto result = model.prepareInputTensorAndPredict(sign);
    //             resultToLabel(&result);
    //         }

    //     }
    //     catch(const std::exception& e)
    //     {
    //         std::cerr << e.what() << '\n';
    //     }
    //     cv::imshow("frame", frame);
    //     if (cv::waitKey(1) >= 0)
    //         break;
    // }
    // return 0;
}