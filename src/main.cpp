#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::string url = "rtsp://admin:adminadmin@192.168.1.101/stream1";
    bool record = false;

    // Simple command line argument parsing
    if (argc > 1) url = argv[1];
    if (argc > 2 && std::string(argv[2]) == "--record") record = true;

    std::cout << "Connecting to: " << url << std::endl;
    if (record) std::cout << "Recording enabled (press 'r' to stop)\n";

    cv::VideoCapture cap(url, cv::CAP_FFMPEG);
    if (!cap.isOpened()) {
        std::cout << "Failed to open stream. Trying fallback..." << std::endl;
        cap.open(url);
    }

    if (!cap.isOpened()) {
        std::cout << "❌ Cannot open camera!" << std::endl;
        return -1;
    }

    std::cout << "✅ Connected successfully! Press 'q' to quit, 'r' to toggle recording." << std::endl;

    cv::Mat frame;
    cv::VideoWriter writer;
    std::string videoFile = "recording_" + std::to_string(time(0)) + ".avi";

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cout << "Empty frame" << std::endl;
            break;
        }

        cv::imshow("Vehicle Camera Feed", frame);

        char key = (char)cv::waitKey(1);
        if (key == 'q') break;

        if (key == 'r') {
            record = !record;
            if (record) {
                writer.open(videoFile, cv::VideoWriter::fourcc('M','J','P','G'), 25, frame.size());
                std::cout << "Recording started: " << videoFile << std::endl;
            } else {
                writer.release();
                std::cout << "Recording stopped." << std::endl;
            }
        }

        if (record && writer.isOpened()) {
            writer.write(frame);
        }
    }

    cap.release();
    if (writer.isOpened()) writer.release();
    cv::destroyAllWindows();
    return 0;
}