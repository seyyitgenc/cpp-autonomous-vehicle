#include <iostream>
#include "utils/filesystem.hpp"
#include "vision.hpp"
#include "onnx_model_handler.hpp"
#include "sign_detection.hpp"
#include "vehicle.hpp"
#include "utils/frame_extractor.hpp"
#include "utils/stopwatch.hpp"

#include "lccv.hpp"

inline void stop(){
    // TODO: stop motors here
    
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << " i slept 10sec" << std::endl;
}

bool ignore = false;

int main(){
    std::string model_path = FileSystem::getPath("src/model/sign.onnx");
    std::string video_path = FileSystem::getPath("videos/test12.mp4");
    ModelHandler model(model_path);

    Vehicle car;
    
    lccv::PiCamera cam;
    cam.options->video_width = 640;
    cam.options->video_height = 480;
    cam.options->framerate = 60;
    cam.options->verbose = false;
    cam.startVideo();
    LaneDetector detector(640,480);
    
    SignDetector sdetector;
    
    cv::Mat frame, cut_gray, canny_edges;
    
    int error = 0;
    int vel1 = 0, vel2 = 0;
    double k = 0.32;
    int speed = 100;

    Stopwatch<> timer1;
    Stopwatch<> timer2;
    float result2  = 0.0f;
    // Known parameters
    double knownWidth = 7; // Actual width of the object (in meters)
    double focalLength = 300; // Focal length of the camera (in pixels)
    while (true)
    {
        if (!cam.getVideoFrame(frame, 1000))
            continue;

        auto processed =  sdetector.preprocessSign(frame);
        try
        {
            auto contours = sdetector.findContour(processed);
            
            auto big = sdetector.findBiggestContour(contours);

            if (big.size() <= 0)
                goto SKIP;
            if (cv::contourArea(big) > 3000 && total_time >= threshold_time)
            {
                total_time = 0.0f;
                std::cout << "1 sec passed and object detected" << std::endl;                
                auto sign =  sdetector.boundaryBox(frame, big);
                cv::imshow("traffic sign",sign);
                double objectWidthPixels = sign.cols;
                // Calculate the distance to the object
                double distance = (knownWidth * focalLength) / objectWidthPixels;
                std::cout << "distance in : " << distance << std::endl;
                if (distance < 30 && !ignore){
                    // auto result = model.prepareInputTensorAndPredict(sign);
                    // SIGN sign_type = resultToType(&result);
                    // if (sign_type == SIGN::STOP)
                    // {
                    //     // stop();
                    //     ignore = true;
                    // }

                    // // cut_gray
                    // if (result2 < 2)
                    //     ignore = false;
                    
                    // if (instructions.top() != sign_type || !instructions.empty()){
                    //     instructions.push(sign_type);
                    // }
                }
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        cut_gray = detector.preprocess(frame);
        cv::Canny(cut_gray, canny_edges,100,200);
        // cv::imshow("canny edges", canny_edges);
        error = detector.calc_err(cut_gray) - 320;

        vel1 = speed + (error * k);
        vel2 = speed - (error * k);
        // car.auto_drive(vel1, vel2);
        // std::cout << "vel1:" << vel1 << ',' << "vel2:" << vel2 << ", error: " << error << std::endl;

        result2 += timer2.getElapsedTime<float>();
        total_time += timer1.getElapsedTime<float>();
        // std::cout << timer1.getElapsedTime<float>() << std::endl;
        timer1.reset();
        timer2.reset();
    SKIP:
        cv::imshow("frame", frame);
        if (cv::waitKey(1) == 27)
            break;
    }
    cam.stopVideo();
    cv::destroyAllWindows();
    return 0;
}