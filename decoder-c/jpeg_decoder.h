#include <cstdlib>
#include <string>

#ifndef DECODER_C_JPEG_DECODER_H
#define DECODER_C_JPEG_DECODER_H

void convertImageWithSamplingFactor(const std::string& input_image_path,
                                    const std::string& converted_image_path,
                                    const std::string& sampling_factor);
void decodeImage(std::vector<std::vector<std::tuple<int, int, int>>>& output, const std::vector<uint8_t>& img_data, int& scaling_factor, std::pair<int, int>& blockCoordinate, std::pair<int, int>& pixelCoordinate);

#endif //DECODER_C_JPEG_DECODER_H
