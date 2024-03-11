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

// Define a function to decode Huffman
void decodeHuffman(const std::vector<uint8_t>& chunk) {
    std::cout<<"decodeHuffman reached";
    // Your implementation here
}

// Define a function to define quantization tables
void DefineQuantizationTables(const std::vector<uint8_t>& chunk) {
    // Your implementation here
}

// Define a function for Baseline DCT
void BaselineDCT(const std::vector<uint8_t>& chunk) {
    // Your implementation here
}

// Define a function for Start of Scan
int StartOfScan(const std::vector<uint8_t>& data, int len_chunk) {
    // Your implementation here
    return len_chunk;
}

void decodeImage(std::vector<std::vector<int> >& output, const std::vector<uint8_t>& img_data){

    // Variables:
    //For the stream class:
    std::vector<unsigned char> dataStream;
    int pos = 0;
    //For the huffmanTable class:
    std::vector<std::vector<int> > hfTables;

    std::vector<uint8_t> data = img_data;  // image data
    while (!data.empty()) {
        uint16_t marker = (data[0] << 8) + data[1];
        if (marker == 0xFFD8) {
            size_t lenchunk = 2;
            data.erase(data.begin(), data.begin() + lenchunk);
        } else if (marker == 0xFFD9) {
            break;
        } else {
            uint16_t len_chunk = (data[2] << 8) + data[3];
            len_chunk += 2;
            std::vector<uint8_t> chunk(data.begin() + 4, data.begin() + len_chunk);
            if (marker == 0xFFC4) {
                decodeHuffman(chunk);
            } else if (marker == 0xFFDB) {
                DefineQuantizationTables(chunk);
            } else if (marker == 0xFFC0) {
                BaselineDCT(chunk);
            } else if (marker == 0xFFDA) {
                len_chunk = StartOfScan(data, len_chunk);
            }
            data.erase(data.begin(), data.begin() + len_chunk);
        }
    }
}