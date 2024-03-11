cmake_minimum_required(VERSION 3.27)
project(decoder_c)

set(CMAKE_CXX_STANDARD 17)

#find_library(OpenCV)
#find_package(OpenCV)

#include_directories(${OpenCV_INCLUDE_DIRS})
#set(CMAKE_CXX_STANDARD 17) commented out to use std::variant

add_executable(decoder_c main.cpp
        jpeg_decoder.cpp
        jpeg_decoder.h
        huffmanTable.cpp
        stream.cpp
        stream.h
        linkedList.cpp)

#target_link_libraries(decoder_c ${OpenCV_LIBS})
