#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <ctime>

int main(int argc, char** argv) {
    std::string url = "rtsp://admin:adminadmin@192.168.1.101/stream1";
    bool record = false;

    if (argc > 1) url = argv[1];
    if (argc > 2 && std::string(argv[2]) == "--record") record = true;

    std::cout << "Connecting to camera: " << url << std::endl;
    if (record) std::cout << "Recording mode enabled.\n";

    cv::VideoCapture cap(url, cv::CAP_FFMPEG);
    if (!cap.isOpened()) {
        std::cout << "Failed. Trying fallback..." << std::endl;
        cap.open(url);
    }

    if (!cap.isOpened()) {
        std::cout << "❌ Cannot open camera. Check IP, credentials and network." << std::endl;
        return -1;
    }

    std::cout << "✅ Camera connected successfully!" << std::endl;
    std::cout << "Press 'q' to quit, 'r' to toggle recording." << std::endl;

    cv::Mat frame;
    cv::VideoWriter writer;
    std::string videoFile;

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cout << "Empty frame received." << std::endl;
            break;
        }

        cv::imshow("Vehicle Camera Feed", frame);

        char key = (char)cv::waitKey(1);

        if (key == 'q') break;

        if (key == 'r') {
            record = !record;
            if (record) {
                videoFile = "recording_" + std::to_string(std::time(nullptr)) + ".avi";
                writer.open(videoFile, cv::VideoWriter::fourcc('M','J','P','G'), 25, frame.size());
                std::cout << "🔴 Recording started: " << videoFile << std::endl;
            } else {
                writer.release();
                std::cout << "⏹️  Recording stopped." << std::endl;
            }
        }

        if (record && writer.isOpened()) {
            writer.write(frame);
        }
    }

    cap.release();
    if (writer.isOpened()) writer.release();
    cv::destroyAllWindows();

    std::cout << "Program terminated." << std::endl;
    return 0;
}