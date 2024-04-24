#include "vision.hpp"

cv::Mat preprocess(cv::Mat input){
    cv::Mat cut_gray;
    cv::cvtColor(input(cv::Rect(0, 270, 640, 90)), cut_gray, cv::COLOR_BGR2GRAY);
    cut_gray = cut_gray + (cv::Scalar(190) - cv::mean(cut_gray));
    cv::GaussianBlur(cut_gray, cut_gray, cv::Size(9, 9), 3, 3);

    cv::threshold(cut_gray, cut_gray, 235, 256, cv::THRESH_BINARY);
    cv::morphologyEx(cut_gray, cut_gray, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(9, 9)));
    
    return cut_gray;
}

int calc_err(cv::Mat gray_img) {
    cv::Mat labels, stats, centroids;
    int cnt = cv::connectedComponentsWithStats(gray_img, labels, stats, centroids);

    cv::Mat dst;
    cvtColor(gray_img, dst, cv::COLOR_GRAY2BGR);
    static cv::Point center_variable_l(160, 45);
    static cv::Point center_variable_r(160+320, 45);

    int min_norm_l = INT_MAX;
    int min_norm_r = INT_MAX;

    cv::Rect sel_l;
    cv::Rect sel_r;

    for (int i = 1; i < cnt; i++) {
        int* p = stats.ptr<int>(i);
        cv::Rect r(p[0], p[1], p[2], p[3]);
        cv::rectangle(dst, r, cv::Scalar(150, 150, 150), 1, 8);

        cv::Point r_center = (r.br() + r.tl()) * 0.5;
        int diff_length = cv::norm(center_variable_l - r_center);
        cv::drawMarker(dst, r_center, cv::Scalar(0, 0, 255), 0, 10, 2, 8);
        //L
        if (min_norm_l > diff_length && p[1] + p[3] > 60 && diff_length < 120) {
            sel_l = r;
            min_norm_l = diff_length;
            center_variable_l = r_center;
        }
    }

    for (int i = 1; i < cnt; i++) {
        int* p = stats.ptr<int>(i);
        cv::Rect r(p[0], p[1], p[2], p[3]);

        cv::Point r_center = (r.br() + r.tl()) * 0.5;
        int diff_length = cv::norm(center_variable_r - r_center);
        cv::drawMarker(dst, r_center, cv::Scalar(0, 0, 255), 0, 10, 2, 8);
        //R
        if (min_norm_r > diff_length && p[1] + p[3] > 60 && diff_length < 120) {
            sel_r = r;
            min_norm_r = diff_length;
            center_variable_r = r_center;
        }
    }

    if (!sel_l.empty()) cv::rectangle(dst, sel_l, cv::Scalar(0, 255, 255), 2);
    if (!sel_r.empty()) cv::rectangle(dst, sel_r, cv::Scalar(0, 255, 255), 2);
    cv::drawMarker(dst, center_variable_l, cv::Scalar(255, 128, 255), 3, 10, 2, 8);
    cv::drawMarker(dst, center_variable_r, cv::Scalar(255, 128, 255), 3, 10, 2, 8);
    cv::line(dst, center_variable_l, center_variable_r, cv::Scalar(255, 0, 128), 2);

    cv::Point pt_err((center_variable_l.x+center_variable_r.x)/2, (center_variable_l.y+center_variable_r.y)/2);
    cv::drawMarker(dst, pt_err, cv::Scalar(200, 0, 0), 4, 10, 2, 8);
    cv::imshow("processed_image", dst);
    cv::waitKey(1);
    return pt_err.x;
}

void testLaneFollower(const std::string& path){
    cv::VideoCapture source(path);
    // cv::VideoCapture source(0);

    cv::Mat frame, cut_gray;

    double fps = source.get(cv::CAP_PROP_FPS);

    int delay = 1000 / fps;

    // for motor calculations
    int error = 0;
    int vel1 = 0, vel2 = 0;
    double k = 0.32;
    int speed = 100;

    while (true)
    {
        source >> frame;

        if (frame.empty())
            break;
        cut_gray = preprocess(frame);
        
        error = calc_err(cut_gray) - 320;
        
        vel1 = speed + (error * k);
        vel2 = speed - (error * k);

        std::cout << "vel1:" << vel1 << ',' << "vel2:" << vel2 << ", error: " << error << std::endl;

        cv::imshow("frame", frame);
        if (cv::waitKey(delay) >= 0)
            break;
        
    }
    cv::destroyAllWindows();
}