cmake_minimum_required(VERSION 3.14)
project(VehicleConnectivityGateway)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_path(HTTPLIB_INCLUDE_DIR httplib.h)
if(NOT HTTPLIB_INCLUDE_DIR)
    message(FATAL_ERROR "cpp-httplib header not found. Install httplib.h or add it to the include path.")
endif()

add_executable(gateway src/main.cpp)
target_include_directories(gateway PRIVATE ${HTTPLIB_INCLUDE_DIR})

target_link_libraries(gateway 
    ${OpenCV_LIBS}
    mosquitto
    Threads::Threads
)