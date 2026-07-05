# Vehicle Connectivity Gateway Demo

**C++ project simulating a smart camera gateway for connected vehicles.**

Demonstrates real-time camera streaming + MQTT data publishing — key technologies in modern automotive connectivity (Bordnetz / V2C communication).

## Features
- RTSP camera streaming with OpenCV
- MQTT publishing of camera status (simulating vehicle telemetry)
- Video recording capability
- Clean CMake build system
- Multi-threading ready architecture

## Relevance to Automotive Connectivity
- Real-time video processing (similar to surround-view / ADAS cameras)
- MQTT for cloud connectivity (widely used in modern vehicles)
- Network protocol handling
- Structured embedded Linux development (Raspberry Pi compatible)

## Build (Linux)

```bash
mkdir build && cd build
cmake ..
make
./gateway "rtsp://admin:pass@192.168.1.101/stream1" --record
```

## Logging & Commands
- The gateway writes structured logs to [logs/mqtt.log](logs/mqtt.log) and [logs/events.log](logs/events.log).
- It publishes status to `vehicle/camera/status` and alerts to `vehicle/camera/alert`.
- Subscribe to `vehicle/camera/status` to observe status messages, or publish simple commands to `vehicle/camera/cmd` to control behavior (e.g., `record_on` / `record_off`).

Example: monitor MQTT status with mosquitto_sub

```bash
mosquitto_sub -h test.mosquitto.org -t "vehicle/camera/status"
```
## Requirements for Linux

## Prerequisites (Linux)

```bash
sudo apt update
sudo apt install build-essential cmake git pkg-config \
    libopencv-dev libmosquitto-dev mosquitto-clients
```