# Vehicle Camera Connectivity Demo in C++

A real-time RTSP camera streaming application built in **C++** using OpenCV. This project demonstrates network-based video processing and serves as a practical example of handling real-time data streams — relevant to **vehicle camera systems** and **connectivity functions** in modern automotive Bordnetz architectures.

## 🎯 Project Relevance to Automotive Connectivity

This project showcases skills directly transferable to **Connectivity Bordnetz** development:
- Real-time network data streaming (RTSP protocol)
- Media processing pipeline similar to camera data handling in ADAS / surround-view systems
- Robust error handling and multi-backend support
- Clean CMake build system for cross-platform development
- Foundation for working with high-bandwidth vehicle data streams (e.g. cameras over Automotive Ethernet)

## Features

- Real-time RTSP video streaming from IP cameras
- Multiple backend fallback (`CAP_FFMPEG`, `CAP_MSMF`, `CAP_ANY`)
- Command-line configuration support (URL, credentials)
- Live video display with keyboard controls
- Clean, modular, and well-commented C++17 code
- Cross-platform CMake build system

## Technologies Used

- **C++17**
- **OpenCV 4** (video capture & processing)
- **CMake** (build system)
- **RTSP / FFmpeg** backend
- Git + modern development practices

## Build & Run (Linux)

```bash
mkdir build && cd build
cmake ..
make -j4
./camera "rtsp://admin:password@camera-ip/stream1"