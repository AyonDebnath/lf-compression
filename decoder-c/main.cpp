#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <string>
#include <stdio.h>
#include <fstream>
#include "jpeg_decoder.h"
//#include <opencv2/opencv.hpp>

void saveImage(const std::string& output_image_path, const std::vector<std::vector<int> >& output);
void getJpegDimensions(FILE* infile, int& width, int& height);
std::vector<uint8_t> readImageBinary(const std::string& filename);

int main() {
    std::string input_image_path;
    std::cout << "Enter the input image file path: ";
    std::cin >> input_image_path;

//    std::string converted_image_name = "converted_image.jpeg";
    std::cout << "Enter x and y coordinate of the image pixel to decode separated by a space: ";
    int pixelCoordinateX, pixelCoordinateY;
    std::cin >> pixelCoordinateX;
    std::cin>> pixelCoordinateY;

    std::pair<int, int> pixelCoordinate(pixelCoordinateX, pixelCoordinateY);

    int xBlockCoordinate = pixelCoordinateX / 8;
    int yBlockCoordinate = pixelCoordinateY / 8;
    std::pair<int, int> blockCoordinate(xBlockCoordinate, yBlockCoordinate);
    int scaling_factor = 1;

    // Convert the input image to Y, Cr, Cb format
    char *converted_image_path = "../converted_image.jpeg" ;
    convertImageWithSamplingFactor(input_image_path, converted_image_path, "4:4:4");

    // Read the converted image
    FILE* file = fopen(converted_image_path, "rb");

    if (file == nullptr) {
        std::cerr << "Error: Failed to open the image file." << std::endl;
        return 1;
    }

    //getting the height and width of the image
    int width, height;
    getJpegDimensions(file, width, height);
    std::cout << width;
    std::cout << height << std::endl;

    // Vector that will store the decoded image
    std::vector<std::vector<std::tuple<int, int, int>> > output(height * scaling_factor, std::vector<std::tuple<int, int, int>>(width * scaling_factor));

    // Initialize and decode the image
    std::vector<uint8_t> imageData = readImageBinary(converted_image_path);
    decodeImage(output, imageData, scaling_factor, blockCoordinate, pixelCoordinate);

    // TODO initialize(converted_image_path, output, scaling_factor, blockCoordinate, pixelCoordinate);
    // TODO decode(output);

    //TODO DELETE temporary output
//    output = {
//            {255, 0, 255, 0},
//            {0, 255, 0, 255},
//            {255, 0, 255, 0},
//            {0, 255, 0, 255}
//    };

    // Save the decoded image
//    saveImage("../Images/" + input_image_path.substr(input_image_path.find_last_of('/') + 1, input_image_path.find_last_of('.') - input_image_path.find_last_of('/') - 1) + "_output.png", output);

//    std::cout << "Output has been saved in the Images Directory." << std::endl;
    return 0;
}

// Function to save a 2D array as an image // TODO will delete it , not necessary
//void saveImage(const std::string& filename, const std::vector<std::vector<int>>& array) {
//    // Create a CV Mat object to hold the image data
//    cv::Mat image(array.size(), array[0].size(), CV_8UC1);
//
//    // Copy the array data to the image
//    for (int i = 0; i < array.size(); ++i) {
//        for (int j = 0; j < array[i].size(); ++j) {
//            image.at<uchar>(i, j) = static_cast<uchar>(array[i][j]);
//        }
//    }
//
//    // Save the image
//    cv::imwrite(filename, image);
//}


void getJpegDimensions(FILE* infile, int& width, int& height) {

    // JPEG file format marker bytes
    const uint8_t SOI_marker[] = {0xFF, 0xD8};  // Start of Image
    const uint8_t APP0_marker[] = {0xFF, 0xE0}; // APP0 marker
    const uint8_t SOF0_marker[] = {0xFF, 0xC0}; // Start of Frame (baseline DCT)

    // Read the file byte by byte to find the image dimensions
    uint8_t buffer[4];
    while (!feof(infile)) {
        fread(buffer, sizeof(uint8_t), 2, infile);
        if (memcmp(buffer, SOI_marker, sizeof(SOI_marker)) == 0) {
            // Found Start of Image marker
            // Search for Start of Frame (SOF) marker (width and height are contained in it)
            while (!feof(infile)) {
                fread(buffer, sizeof(uint8_t), 2, infile);
                if (memcmp(buffer, APP0_marker, sizeof(APP0_marker)) == 0) {
                    // Found APP0 marker, skip APP0 segment length field
                    fread(buffer, sizeof(uint8_t), 2, infile);
                    uint16_t app0_segment_length = (buffer[0] << 8) + buffer[1];
                    fseek(infile, app0_segment_length - 2, SEEK_CUR);
                } else if (memcmp(buffer, SOF0_marker, sizeof(SOF0_marker)) == 0) {
                    // Found SOF0 marker (Start of Frame)
                    // Read the image height and width
                    fseek(infile, 3, SEEK_CUR); // Skip length and precision bytes
                    fread(buffer, sizeof(uint8_t), 4, infile);
                    height = (buffer[0] << 8) + buffer[1];
                    width = (buffer[2] << 8) + buffer[3];
                    break;
                } else {
                    // Not the marker we are interested in, move to the next byte
                    fseek(infile, -1, SEEK_CUR);
                }
            }
            break;
        }
    }

    fclose(infile);
}

std::vector<uint8_t> readImageBinary(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return std::vector<uint8_t>(); // Return an empty vector if file opening fails
    }

    // Determine the size of the file
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the entire file into a vector
    std::vector<uint8_t> imageData(fileSize);
    file.read(reinterpret_cast<char*>(imageData.data()), fileSize);
    file.close();

    return imageData;
}