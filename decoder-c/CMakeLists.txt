cmake_minimum_required(VERSION 3.27)
project(decoder_c)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})
set(CMAKE_CXX_STANDARD 17)

add_executable(decoder_c main.cpp
        jpeg_decoder.cpp
        huffmanTable.cpp
        stream.cpp
        stream.h
        huffmanTable.h
)

target_link_libraries(decoder_c ${OpenCV_LIBS})
