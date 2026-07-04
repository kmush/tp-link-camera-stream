#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    std::string url = "rtsp://admin:adminadmin@192.168.1.101/stream1";
    
    std::cout << "Trying to open: " << url << std::endl;

    cv::VideoCapture cap(url, cv::CAP_FFMPEG);

    if (!cap.isOpened()) {
        std::cout << "Failed to open camera!" << std::endl;
        return -1;
    }

    std::cout << "✅ Camera opened successfully! Press 'q' to quit." << std::endl;

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;
        
        cv::imshow("TP-Link Camera", frame);
        if (cv::waitKey(30) == 'q') break;
    }
    return 0;
}