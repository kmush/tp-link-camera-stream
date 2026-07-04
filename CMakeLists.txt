cmake_minimum_required(VERSION 3.10)
project(VehicleCameraConnectivity)

find_package(OpenCV REQUIRED)

add_executable(camera src/main.cpp)

target_link_libraries(camera ${OpenCV_LIBS})
target_compile_features(camera PRIVATE cxx_std_17)