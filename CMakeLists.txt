cmake_minimum_required(VERSION 3.14)
project(VehicleConnectivityGateway)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(gateway src/main.cpp)

target_link_libraries(gateway 
    ${OpenCV_LIBS}
    mosquitto
)