cmake_minimum_required(VERSION 3.10)
project(TpLinkCamera)

find_package(OpenCV REQUIRED)

add_executable(camera main.cpp)
target_link_libraries(camera ${OpenCV_LIBS})