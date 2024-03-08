#include <cstdlib>
#include <string>

#ifndef DECODER_C_JPEG_DECODER_H
#define DECODER_C_JPEG_DECODER_H

void convertImageWithSamplingFactor(const std::string& input_image_path,
                                    const std::string& converted_image_path,
                                    const std::string& sampling_factor);

#endif //DECODER_C_JPEG_DECODER_H
