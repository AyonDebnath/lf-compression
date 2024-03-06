cmake_minimum_required(VERSION 3.27)
project(decoder_c)

set(CMAKE_CXX_STANDARD 17)

#find_library(OpenCV)
find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})
set(CMAKE_CXX_STANDARD 11)

add_executable(decoder_c main.cpp)

target_link_libraries(decoder_c ${OpenCV_LIBS})
