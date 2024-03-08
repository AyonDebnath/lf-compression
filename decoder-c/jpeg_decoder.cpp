#include <iostream>
#include <cstdlib>
#include <string>

void convertImageWithSamplingFactor(const std::string& input_image_path,
                                    const std::string& converted_image_path,
                                    const std::string& sampling_factor) {
    std::string command = "convert " + input_image_path +
                          " -sampling-factor " + sampling_factor +
                          " " + converted_image_path;

    int result = std::system(command.c_str());

    if (result == 0) {
        std::cout << "Image converted with sampling factor " << sampling_factor << ".\n";
    } else {
        std::cerr << "Error: Failed to convert image.\n";
    }
}