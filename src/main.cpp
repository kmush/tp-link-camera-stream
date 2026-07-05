#include <opencv2/opencv.hpp>
#include <mosquitto.h>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <ctime>
#include <fstream>

std::ofstream logFile;
void writeLog(const std::string& message) {
    auto now = std::time(nullptr);
    logFile << std::ctime(&now) << " " << message << std::endl;
    logFile.flush();
}

std::atomic<bool> running{true};

void on_connect(struct mosquitto* mosq, void* obj, int rc) {
    std::cout << "MQTT Connected!" << std::endl;
}

int main(int argc, char** argv) {
    logFile.open("gateway.log", std::ios::app);
    writeLog("=== Program Started ===");
    std::string url = "rtsp://admin:adminadmin@192.168.1.101/stream1";
    bool record = false;

    if (argc > 1) url = argv[1];
    if (argc > 2 && std::string(argv[2]) == "--record") record = true;

    // MQTT Setup
    mosquitto_lib_init();
    struct mosquitto* mosq = mosquitto_new("vehicle_gateway", true, nullptr);
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_connect(mosq, "test.mosquitto.org", 1883, 60);  // Public test broker

    std::cout << "Connecting to camera: " << url << std::endl;

    cv::VideoCapture cap(url, cv::CAP_FFMPEG);
    if (!cap.isOpened()) {
        writeLog("Failed to open camera");
        std::cout << "Failed to open camera." << std::endl;
        return -1;
    }

    writeLog("Camera connected successfully");

    std::cout << "✅ Gateway running! 'q'=quit, 'r'=record" << std::endl;

    cv::Mat frame;
    cv::VideoWriter writer;
    std::string videoFile;
    //bool recording = false;

    while (running) {
        cap >> frame;
        if (frame.empty()) break;

        cv::imshow("Vehicle Camera Gateway", frame);

        char key = (char)cv::waitKey(1);
        if (key == 'q') break;
        if (key == 'r') {
            record = !record;
            writeLog(record ? "Recording STARTED" : "Recording STOPPED");
            if (record) {
                videoFile = "rec_" + std::to_string(std::time(nullptr)) + ".avi";
                writer.open(videoFile, cv::VideoWriter::fourcc('M','J','P','G'), 25, frame.size());
                std::cout << "Recording started." << std::endl;
            } else {
                writer.release();
                std::cout << "Recording stopped." << std::endl;
            }
        }

        if (record && writer.isOpened()) writer.write(frame);

        // Publish status to MQTT every 3 seconds (simplified)
        static int counter = 0;
        if (++counter % 90 == 0) {  // ~3 seconds
            std::string payload = "{\"status\":\"connected\",\"fps\":25,\"timestamp\":" + std::to_string(std::time(nullptr)) + "}";
            mosquitto_publish(mosq, nullptr, "vehicle/camera/status", payload.size(), payload.c_str(), 0, false);
        }
    }

    writeLog("=== Program Terminated ===");
    logFile.close();

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    cap.release();
    if (writer.isOpened()) writer.release();
    return 0;
}