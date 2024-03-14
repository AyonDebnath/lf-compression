#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>
#include "huffmanTable.h"

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
    size_t offset = 0;

    while (offset < chunk.size()) {
        uint8_t header = chunk[offset++];
        std::vector<uint8_t> lengths;
        for (int i = 0; i < 16; ++i) {
            lengths.push_back(chunk[offset++]);
        }

        std::vector<uint8_t> elements;
        for (auto length : lengths) {
            for (int i = 0; i < length; ++i) {
                elements.push_back(chunk[offset++]);
            }
        }

        // Assuming HuffmanTable is a class with method initialize() and GetHuffmanBits()
        std::vector<std::pair<Tree, std::vector<uint8_t>>> hfTables;
        hfTables.push_back(std::make_pair(Tree(), std::vector<uint8_t>()));
        GetHuffmanBits(lengths, elements, hfTables);
        hfTables.resize(header + 1);
        hfTables[header] = std::make_pair(hfTables.back().first, hfTables.back().second);
    }
    return;
    // Your implementation here
}

// Define a function to define quantization tables
void DefineQuantizationTables(const std::vector<uint8_t>& chunk, std::unordered_map<int, std::vector<uint8_t>>& quant) {
    size_t pos = 0;
    while (pos < chunk.size()) {
        uint8_t hdr = chunk[pos];
        std::vector<uint8_t> quantData(chunk.begin() + pos + 1, chunk.begin() + pos + 65);
        quant[hdr] = quantData;
        pos += 65;
    }
}

// Define a function for Baseline DCT
void BaselineDCT(const std::vector<uint8_t>& chunk, std::vector<int>& quantMapping, int& height, int& width) {

    std::tuple<uint8_t, uint16_t, uint16_t, uint8_t> header;
    std::memcpy(&header, &chunk[0], sizeof(header));
    uint8_t hdr = std::get<0>(header);
    height = std::get<1>(header);
    width = std::get<2>(header);
    uint8_t components = std::get<3>(header);

    for (size_t i = 0; i < components; ++i) {
        uint8_t id = chunk[6 + i * 3];
        uint8_t samp = chunk[7 + i * 3];
        uint8_t QtbId = chunk[8 + i * 3];
        quantMapping.push_back(QtbId);
    }
}

// Define a function for Start of Scan
int StartOfScan(const std::vector<uint8_t>& data, int len_chunk) {
    // Your implementation here
    return len_chunk;
}

void decodeImage(std::vector<std::vector<int> >& output, const std::vector<uint8_t>& img_data){

    // Variables:
    int height, width;
    std::vector<int> quantMapping;
    std::unordered_map<int, std::vector<uint8_t>> quant;
    //For the stream class:
    std::vector<unsigned char> dataStream;

    int pos = 0;
    std::vector<uint8_t> data = img_data;  // image data

    while (!data.empty()) {
        uint16_t marker = (data[0] << 8) + data[1];
        if (marker == 0xFFD8) {
            size_t lenchunk = 2;
            data.erase(data.begin(), data.begin() + lenchunk);
        } else if (marker == 0xFFD9) {
            break;
        } else {

            if (data.size() < 4) {
                // Not enough data to read the length field
                // might want to handle this case accordingly
                // Added to fix corrupted Memory
                break;
            }

            uint16_t len_chunk = (data[2] << 8) + data[3];

            if (len_chunk < 2) {
                // Invalid length, maybe corrupted data
                // might want to handle this case accordingly
                // Added to fix corrupted Memory
                break;
            }

            len_chunk += 2;

            if (data.size() < len_chunk) {
                // Not enough data to read the entire chunk
                // might want to handle this case accordingly
                // Added to fix corrupted Memory
                break;
            }

            std::vector<uint8_t> chunk(data.begin() + 4, data.begin() + len_chunk);

            if (marker == 0xFFC4) {
                decodeHuffman(chunk);
            }
            else if (marker == 0xFFDB) {
                DefineQuantizationTables(chunk, quant);
            } else if (marker == 0xFFC0) {
                BaselineDCT(chunk, quantMapping, height, width);
            } else if (marker == 0xFFDA) {
                len_chunk = StartOfScan(data, len_chunk);
            }
            data.erase(data.begin(), data.begin() + len_chunk);
        }
    }
}