#include "vision.hpp"

cv::Mat LaneDetector::preprocess(cv::Mat input){
    cv::Mat hsv, mask, result;

    static cv::Scalar lower_yellow = cv::Scalar(20, 55, 55);
    static cv::Scalar upper_yellow = cv::Scalar(60, 255, 255);

    cv::cvtColor(input, hsv,cv::COLOR_BGR2HSV);
    cv::inRange(hsv, lower_yellow, upper_yellow, mask);
    cv::Mat masked_image;
    cv::bitwise_and(input, input, masked_image, mask);
    // cv::imshow("hsv frame", hsv);
    // cv::imshow("filtered frame", masked_image);
    cv::cvtColor(masked_image, masked_image, cv::COLOR_BGR2GRAY);
    // cv::imshow("after conversion", masked_image);
    // cut_gray = preprocess(frame);
    cv::GaussianBlur(masked_image, masked_image, cv::Size(9, 9), 3, 3);
    // cv::imshow("after blur", masked_image);
    // cv::threshold(cut_gray, cut_gray, 230, 255, cv::THRESH_BINARY);
    cv::morphologyEx(masked_image, masked_image, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(9, 9)));
    // cv::imshow("after morph", masked_image);
    return masked_image(cv::Rect(0, _cut_y, _width, _cut_height));
}

int LaneDetector::calc_err(cv::Mat gray_img) {
    cv::Mat labels, stats, centroids;
    int cnt = cv::connectedComponentsWithStats(gray_img, labels, stats, centroids);

    static cv::Point center(_width / 2, _cut_height / 2);
    // Center point

    double min_norm_l = INTMAX_MAX;
    double min_norm_r = INTMAX_MAX;

    // cv::drawMarker(gray_img, center, cv::Scalar(128,128,255));
        
    // static cv::Rect min_left_rect;
    // static cv::Rect min_right_rect;
    // static cv::Point min_left_rect_center;
    // static cv::Point min_right_rect_center;
    
    // for (int i = 1; i < cnt; i++)
    // {
    //     int *p = stats.ptr<int>(i);
    //     cv::Rect r(p[0], p[1], p[2], p[3]);
    //     // cv::rectangle(gray_img, r, cv::Scalar(150, 150, 150), 1, 8);

    //     cv::Point rect_center = (r.br() + r.tl()) * 0.5;

    //     // cv::rectangle(gray_img, r, cv::Scalar(150, 150, 150), 1, 8);
    //     // cv::drawMarker(gray_img, rect_center, cv::Scalar(128,128,255));
    //     // right rect
    //     if (rect_center.x > center.x){
    //         double distance = cv::norm(center - rect_center);
    //         if (distance < distance_from_line)
    //             continue;
    //         if (min_norm_r > distance){
    //             min_norm_r = distance;
    //             min_right_rect = r;
    //             min_right_rect_center = rect_center;
    //         }
    //     }
    //     // left rect
    //     else{
    //         // int diff_length = cv::norm(center_variable_l - r_center);
    //         double distance = cv::norm(rect_center - center);
    //         if (min_norm_l > distance){
    //             min_norm_l = distance;
    //             min_left_rect = r;
    //             min_left_rect_center = rect_center;
    //         }
    //     }
    // }
    // // double center_line = cv::norm((min_right_rect_center + min_left_rect_center));

    // cv::rectangle(gray_img, min_left_rect, cv::Scalar(150, 150, 150), 1, 8);
    // cv::rectangle(gray_img, min_right_rect, cv::Scalar(150, 150, 150), 1, 8);
    // cv::line(gray_img, min_left_rect_center, min_right_rect_center, cv::Scalar(150, 150, 150),5);
    // std::cout << "distance of left " << min_norm_l << " distance of right " << min_norm_r << std::endl;
    // cv::Point pt_err((center_variable_l.x + center_variable_r.x) / 2, (center_variable_l.y + center_variable_r.y) / 2);

    static cv::Point center_variable_l(_width / 4, _cut_y / _height_scale);
    static cv::Point center_variable_r(3 * _width / 4, _cut_y / _height_scale);

    cv::Rect sel_l;
    cv::Rect sel_r;
    for (int i = 1; i < cnt; i++) {
        int* p = stats.ptr<int>(i);
        cv::Rect r(p[0], p[1], p[2], p[3]);
        cv::rectangle(gray_img, r, cv::Scalar(150, 150, 150), 1, 8);

        cv::Point r_center = (r.br() + r.tl()) * 0.5;
        int diff_length = cv::norm(center_variable_l - r_center);
        cv::drawMarker(gray_img, r_center, cv::Scalar(0, 0, 255), 0, 10, 2, 8);
        //L
        if (min_norm_l > diff_length && p[1] + p[3] > 60 && diff_length < distance_from_line) {
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
        cv::drawMarker(gray_img, r_center, cv::Scalar(0, 0, 255), 0, 10, 2, 8);
        //R
        if (min_norm_r > diff_length && p[1] + p[3] > 60 && diff_length < distance_from_line) {
            sel_r = r;
            min_norm_r = diff_length;
            center_variable_r = r_center;
        }
    }

    if (!sel_l.empty()) cv::rectangle(gray_img, sel_l, cv::Scalar(0, 255, 255), 2);
    if (!sel_r.empty()) cv::rectangle(gray_img, sel_r, cv::Scalar(0, 255, 255), 2);
    cv::drawMarker(gray_img, center_variable_l, cv::Scalar(255, 128, 255), 3, 10, 2, 8);
    cv::drawMarker(gray_img, center_variable_r, cv::Scalar(255, 128, 255), 3, 10, 2, 8);
    cv::line(gray_img, center_variable_l, center_variable_r, cv::Scalar(255, 0, 128), 2);
    
    cv::Point pt_err((center_variable_l.x + center_variable_r.x) / 2, (center_variable_l.y + center_variable_r.y) / 2);
    cv::drawMarker(gray_img, pt_err, cv::Scalar(200, 0, 0), 4, 10, 2, 8);
    cv::imshow("processed_image", gray_img);
    return pt_err.x;  
}

int errconstant = 0;
void testLaneDetector(const std::string& path){
    cv::VideoCapture source(path);
    // cv::VideoCapture source(0);
    LaneDetector detector(source.get(cv::CAP_PROP_FRAME_WIDTH), source.get(cv::CAP_PROP_FRAME_HEIGHT));
    cv::Mat frame, cut_gray;

    double fps = source.get(cv::CAP_PROP_FPS);
    // int delay = 1000 / fps;

    // for motor calculations
    int error = 0;
    int vel1 = 0, vel2 = 0;
    double k = 0.32;
    int speed = 100;

    while (true)
    {
        source >> frame;
        // errconstant = frame.rows / 2;
        if (frame.empty())
            break;
        cut_gray = detector.preprocess(frame);
        
        error = detector.calc_err(cut_gray) - 640;
        
        vel1 = speed + (error * k);
        vel2 = -(speed - (error * k));

        std::cout << "vel1:" << vel1 << ',' << "vel2:" << vel2 << ", error: " << error << std::endl;

        cv::imshow("frame", frame);
        if (cv::waitKey(30) >= 0)
            break;
        
    }
    cv::destroyAllWindows();
}