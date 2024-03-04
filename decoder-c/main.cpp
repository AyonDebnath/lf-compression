#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <opencv2/opencv.hpp>

int main() {
    std::string input_image_path;
    std::cout << "Enter the input image file path: ";
    std::cin >> input_image_path;

    std::string converted_image_path = "converted_image.jpeg";
    std::string pixelCoordinate;
    std::cout << "Enter x and y coordinate of the image pixel to decode separated by a space: ";
    std::cin >> pixelCoordinate;

    // Converting the coordinates of the pixel to a pair of integers
//    std::istringstream iss(pixelCoordinate);
//    int xPixelCoordinate, yPixelCoordinate;
//    iss >> xPixelCoordinate >> yPixelCoordinate;

    return 0;
}
