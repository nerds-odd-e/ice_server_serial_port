cmake_minimum_required(VERSION 3.14)
project(ice_server_serial_port)

set(CMAKE_CXX_STANDARD 98)

include_directories("/opt/Ice-3.1/include")
include_directories(".")
link_directories("/opt/Ice-3.1/lib64")

add_library(stub_device device.c)
add_executable(ice_server_serial_port main.cpp Server.cpp)
target_link_libraries(ice_server_serial_port Ice IceStorm stub_device)
