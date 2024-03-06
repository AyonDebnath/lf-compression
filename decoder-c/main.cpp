#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <stdio.h>
#include <opencv2/opencv.hpp>

void convertImageWithSamplingFactor(const std::string& input_image_path, const std::string& converted_image_path, const std::string& sampling_factor);
void initialize(const std::string& converted_image_path, std::vector<std::vector<int>>& output, int scaling_factor, std::pair<int, int> blockCoordinate, std::pair<int, int> pixelCoordinate);
void decode(std::vector<std::vector<int>>& output);
void saveImage(const std::string& output_image_path, const std::vector<std::vector<int>>& output);


int main() {
    std::string input_image_path;
    std::cout << "Enter the input image file path: ";
    std::cin >> input_image_path;

    std::string converted_image_name = "converted_image.jpeg";
    std::string pixelCoordinate;
    std::cout << "Enter x and y coordinate of the image pixel to decode separated by a space: ";
    int pixelCoordinateX, pixelCoordinateY;
    std::cin >> pixelCoordinate;

    // Converting the coordinates of the pixel to a pair of integers
    std::istringstream iss(pixelCoordinate);
    int xPixelCoordinate, yPixelCoordinate;
    iss >> xPixelCoordinate >> yPixelCoordinate;

    // Opening the image
    cv::Mat input_image = cv::imread(input_image_path);

    // Print the RGB value at the given pixel coordinate
    cv::Vec3b pixel = input_image.at<cv::Vec3b>(yPixelCoordinate, xPixelCoordinate);
    std::cout << "The RGB value at x = " << xPixelCoordinate << " and y = " << yPixelCoordinate << " is: ("
              << static_cast<int>(pixel[2]) << ", " << static_cast<int>(pixel[1]) << ", " << static_cast<int>(pixel[0]) << ")" << std::endl;

    int xBlockCoordinate = xBlockCoordinate / 8;
    int yBlockCoordinate = yPixelCoordinate / 8;
    std::pair<int, int> blockCoordinate(xBlockCoordinate, yBlockCoordinate);
    int scaling_factor = 1;

    // Convert the input image to Y, Cr, Cb format
    //  TODO  convertImageWithSamplingFactor(input_image_path, converted_image_path, "4:4:4");

    // Read the converted image
    std::string converted_image_path = "../" + converted_image_name;
    cv::Mat converted_image = cv::imread(converted_image_path);
    int width = converted_image.cols;
    int height = converted_image.rows;

    // Vector that will store the decoded image
    std::vector<std::vector<int>> output(height * scaling_factor, std::vector<int>(width * scaling_factor));

    // Initialize and decode the image
    // TODO initialize(converted_image_path, output, scaling_factor, blockCoordinate, pixelCoordinate);
    // TODO decode(output);

    //TODO DELETE temporary output
    output = {
            {255, 0, 255, 0},
            {0, 255, 0, 255},
            {255, 0, 255, 0},
            {0, 255, 0, 255}
    };

    // Save the decoded image
    saveImage("../Images/" + input_image_path.substr(input_image_path.find_last_of('/') + 1, input_image_path.find_last_of('.') - input_image_path.find_last_of('/') - 1) + "_output.png", output);

    std::cout << "Output has been saved in the Images Directory." << std::endl;
    return 0;
}

// Function to save a 2D array as an image
void saveImage(const std::string& filename, const std::vector<std::vector<int>>& array) {
    // Create a CV Mat object to hold the image data
    cv::Mat image(array.size(), array[0].size(), CV_8UC1);

    // Copy the array data to the image
    for (int i = 0; i < array.size(); ++i) {
        for (int j = 0; j < array[i].size(); ++j) {
            image.at<uchar>(i, j) = static_cast<uchar>(array[i][j]);
        }
    }

    // Save the image
    cv::imwrite(filename, image);
}
