#include <opencv2/opencv.hpp>
#include <mosquitto.h>
#include <httplib.h>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <ctime>
#include <fstream>
#include <mutex>
#include <vector>

std::ofstream logFile;
void writeLog(const std::string& message) {
    auto now = std::time(nullptr);
    logFile << std::ctime(&now) << " " << message << std::endl;
    logFile.flush();
}

std::atomic<bool> running{true};
std::atomic<bool> recording{false};
std::atomic<bool> recordRequested{false};
std::atomic<bool> cameraConnected{false};
std::mutex frameMutex;
std::condition_variable frameReady;
std::vector<unsigned char> latestJpeg;
std::uint64_t frameSequence = 0;

void on_connect(struct mosquitto* mosq, void* obj, int rc) {
    std::cout << "MQTT Connected!" << std::endl;
}

int main(int argc, char** argv) {
    logFile.open("gateway.log", std::ios::app);
    writeLog("=== Program Started ===");
    std::string url = "rtsp://admin:adminadmin@192.168.1.101/stream1";
    bool record = false;

    if (argc > 1) url = argv[1];
    if (argc > 2 && std::string(argv[2]) == "--record") recordRequested.store(true);

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
    cameraConnected.store(true);

    httplib::Server server;
    server.Get("/health", [](const httplib::Request&, httplib::Response& response) {
        response.set_content("{\"ok\":true}", "application/json");
    });
    server.Get("/status", [](const httplib::Request&, httplib::Response& response) {
        const std::string payload = "{\"connected\":" +
            std::string(cameraConnected.load() ? "true" : "false") +
            ",\"recording\":" + std::string(recording.load() ? "true" : "false") +
            ",\"timestamp\":" + std::to_string(std::time(nullptr)) + "}";
        response.set_content(payload, "application/json");
    });
    server.Post("/record", [](const httplib::Request& request, httplib::Response& response) {
        if (request.body == "on") {
            recordRequested.store(true);
        } else if (request.body == "off") {
            recordRequested.store(false);
        } else {
            response.status = 400;
            response.set_content("{\"error\":\"Body must be 'on' or 'off'\"}", "application/json");
            return;
        }
        response.set_content("{\"accepted\":true}", "application/json");
    });
    server.Get("/stream", [](const httplib::Request&, httplib::Response& response) {
        response.set_header("Cache-Control", "no-store");
        response.set_chunked_content_provider(
            "multipart/x-mixed-replace; boundary=frame",
            [lastSequence = std::uint64_t{0}](size_t, httplib::DataSink& sink) mutable {
                std::vector<unsigned char> jpeg;
                std::uint64_t currentSequence = 0;
                {
                    std::unique_lock<std::mutex> lock(frameMutex);
                    frameReady.wait_for(lock, std::chrono::seconds(1), [&] {
                        return frameSequence != lastSequence || !running.load();
                    });
                    if (!running.load()) return false;
                    currentSequence = frameSequence;
                    if (currentSequence == 0 || currentSequence == lastSequence) return true;
                    jpeg = latestJpeg;
                }

                const std::string header = "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: " +
                    std::to_string(jpeg.size()) + "\r\n\r\n";
                if (!sink.write(header.data(), header.size()) ||
                    !sink.write(reinterpret_cast<const char*>(jpeg.data()), jpeg.size()) ||
                    !sink.write("\r\n", 2)) {
                    return false;
                }
                lastSequence = currentSequence;
                return true;
            });
    });
    std::thread httpThread([&server] {
        if (!server.listen("0.0.0.0", 8080)) {
            std::cerr << "HTTP server failed to listen on port 8080" << std::endl;
        }
    });

    std::cout << "✅ Gateway running! 'q'=quit, 'r'=record" << std::endl;

    cv::Mat frame;
    cv::VideoWriter writer;
    std::string videoFile;
    //bool recording = false;

    while (running) {
        cap >> frame;
        if (frame.empty()) break;

        std::vector<unsigned char> encodedFrame;
        if (cv::imencode(".jpg", frame, encodedFrame, {cv::IMWRITE_JPEG_QUALITY, 80})) {
            {
                std::lock_guard<std::mutex> lock(frameMutex);
                latestJpeg = std::move(encodedFrame);
                ++frameSequence;
            }
            frameReady.notify_all();
        }

        cv::imshow("Vehicle Camera Gateway", frame);

        char key = (char)cv::waitKey(1);
        if (key == 'q') break;
        if (key == 'r') recordRequested.store(!record);

        const bool requestedRecording = recordRequested.load();
        if (requestedRecording != record) {
            record = requestedRecording;
            writeLog(record ? "Recording STARTED" : "Recording STOPPED");
            if (record) {
                videoFile = "rec_" + std::to_string(std::time(nullptr)) + ".avi";
                writer.open(videoFile, cv::VideoWriter::fourcc('M','J','P','G'), 25, frame.size());
                if (writer.isOpened()) {
                    recording.store(true);
                    std::cout << "Recording started." << std::endl;
                } else {
                    record = false;
                    recordRequested.store(false);
                    writeLog("Failed to open recording file");
                    std::cerr << "Failed to open recording file." << std::endl;
                }
            } else {
                writer.release();
                recording.store(false);
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

    running.store(false);
    cameraConnected.store(false);
    frameReady.notify_all();
    server.stop();
    if (httpThread.joinable()) httpThread.join();

    writeLog("=== Program Terminated ===");
    logFile.close();

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    cap.release();
    if (writer.isOpened()) writer.release();
    return 0;
}