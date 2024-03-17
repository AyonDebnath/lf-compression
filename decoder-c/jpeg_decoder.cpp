#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>
#include "huffmanTable.h"
#include "stream.h"

std::vector<std::pair<Tree, std::vector<uint8_t>>> hfTables;

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

double NormCoeff(int n) {
    if (n == 0) {
        return 1.0 / std::sqrt(2.0);
    } else {
        return 1.0;
    }
}

int DecodeNumber(int code, int bits) {
    int l = pow(2, code - 1);
    if (bits >= l) {
        return bits;
    } else {
        return bits - (2 * l - 1);
    }
}

std::vector<std::vector<uint8_t>> perform_IDCT(std::vector<uint8_t>& base, int& idct_precision, std::vector<std::vector<uint8_t>>& zigzag, std::vector<std::vector<double>>& idct_table) {
    std::vector<std::vector<uint8_t>> out(8, std::vector<uint8_t>(8));

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            int local_sum = 0;
            for (int u = 0; u < idct_precision; ++u) {
                for (int v = 0; v < idct_precision; ++v) {
                    local_sum += zigzag[v][u] * idct_table[u][x] * idct_table[v][y];
                }
            }
            out[y][x] = local_sum / 4;
        }
    }

    return out;
}

std::tuple<std::vector<std::vector<uint8_t>>, int> BuildMatrix(int idx, std::vector<uint8_t>& quant, int olddccoeff,
                                                           std::vector<uint8_t>& data, int& pos) {
    int idct_precision = 8;
    std::vector<uint8_t> base ={0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0};

    std::vector<std::vector<double>> idct_table;

    std::vector<std::vector<uint8_t>> zigzag = {
            {0, 1, 5, 6, 14, 15, 27, 28},
            {2, 4, 7, 13, 16, 26, 29, 42},
            {3, 8, 12, 17, 25, 30, 41, 43},
            {9, 11, 18, 24, 31, 40, 44, 53},
            {10, 19, 23, 32, 39, 45, 52, 54},
            {20, 22, 33, 38, 46, 51, 55, 60},
            {21, 34, 37, 47, 50, 56, 59, 61},
            {35, 36, 48, 49, 57, 58, 62, 63}
    };

    for (int u = 0; u < idct_precision; ++u) {
        std::vector<double> row;
        for (int x = 0; x < idct_precision; ++x) {
            double value = NormCoeff(u) * cos(((2.0 * x + 1.0) * u * M_PI) / 16.0);
            row.push_back(value);
        }
        idct_table.push_back(row);

    }

    int code = GetRoot(hfTables[16+idx].first, data, pos);
    int bits = GetBitN(code, data, pos);
    int dccoeff = DecodeNumber(code, bits) + olddccoeff;

    base[0] = dccoeff * quant[0];
    int l = 1;
    while(l < 64){
        code = GetRoot(hfTables[16+idx].first, data, pos);
        if(code == 0){
            break;
        }

        if (code > 15) {
            l += code >> 4;
            code = code & 0x0F;
        }

        bits = GetBitN(code, data, pos);

        if(l < 64){
            int coeff = DecodeNumber(code, bits);

            base[l] = coeff * quant[l];

            l+=1;
        }
    }

//    rearrange_using_zigzag
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            zigzag[x][y] = base[zigzag[x][y]];
        }
    }


    return std::make_tuple(perform_IDCT(base, idct_precision, zigzag, idct_table), dccoeff);
}

std::tuple<std::vector<uint8_t>, int> RemoveFF00(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> datapro;
    int i = 0;
    while (true) {
        uint16_t b = data[i];
        uint16_t bnext = data[i + 1];
        if (b == 0xFF) {
            if (bnext != 0) {
                break;
            }
            datapro.push_back(data[i]);
            i += 2;
        } else {
            datapro.push_back(data[i]);
            i += 1;
        }
    }
    return std::make_tuple(datapro, i);
}

uint8_t Clamp(int col) {
    // Clamp the value between 0 and 255
    return static_cast<uint8_t>(std::min(std::max(col, 0), 255));
}

std::tuple<uint8_t, uint8_t, uint8_t> ColorConversion(int Y, int Cr, int Cb) {
    // Constants for color conversion
    const double kR = 2 - 2 * 0.299;
    const double kB = 2 - 2 * 0.114;
    const double kG = 0.587;

    // Calculate RGB values
    double R = Cr * kR + Y;
    double B = Cb * kB + Y;
    double G = (Y - 0.114 * B - 0.299 * R) / kG;

    // Clamp the values and convert to uint8_t
    uint8_t red = Clamp(R + 128);
    uint8_t green = Clamp(G + 128);
    uint8_t blue = Clamp(B + 128);

    return std::make_tuple(red, green, blue);
}

void WriteDecodedMatrix(int x, int y, std::vector<std::vector<uint8_t>>& matL,
                        std::vector<std::vector<uint8_t>>& matCb, std::vector<std::vector<uint8_t>>& matCr,
                        std::vector<std::vector<std::tuple<int, int, int>>>& output, int scaling_factor, std::pair<int, int>& pixelCoordinate) {
    // Iterate over each pixel in the 8x8 MCU block
    for (int yy = 0; yy < 8; ++yy) {
        for (int xx = 0; xx < 8; ++xx) {
            // Calculate the actual coordinates in the output image
            int x1 = (x * 8 + xx) * scaling_factor;
            int y1 = (y * 8 + yy) * scaling_factor;

            // Fill the entire block with color
            for (int i = 0; i < scaling_factor; ++i) {
                for (int j = 0; j < scaling_factor; ++j) {
                    // Assuming ColorConversion returns an uint8_t value
                    output[y1 + i][x1 + j] = ColorConversion(
                            matL[yy][xx], matCb[yy][xx], matCr[yy][xx]
                    );

                    // Check if this pixel matches the specified coordinate
                    if (x1 == pixelCoordinate.first && y1 == pixelCoordinate.second) {
                        //RGB values:
                        int R = std::get<0>(output[y1 + i][x1 + j]);
                        int G = std::get<1>(output[y1 + i][x1 + j]);
                        int B = std::get<2>(output[y1 + i][x1 + j]);
                        // Print decoded RGB value
                        std::cout << "The decoded RGB value at x = " << pixelCoordinate.first
                                  << " and y = " << pixelCoordinate.second << " is: "
                                  << "("<< R<<","<<G<<","<<B<<")" << std::endl;
                    }
                }
            }
        }
    }
}

// Define a function for Start of Scan
uint16_t StartOfScan(std::vector<uint8_t>& data, uint16_t& hdrlen, std::vector<int>& quantMapping, int& height, int& width,
                std::unordered_map<int, std::vector<uint8_t>>& quant, std::vector<uint8_t>& img_data,
                     std::vector<std::vector<std::tuple<int, int, int>>>& output, std::vector<uint8_t>& dataStream, int& pos, int& scaling_factor, std::pair<int, int>& blockCoordinate, std::pair<int, int>& pixelCoordinate) {

    auto [datapro, lenchunk] = RemoveFF00(std::vector<uint8_t>(data.begin() + hdrlen, data.end()));
    dataStream = datapro; // TODO This line should deepcopy. It might not deepcopy over here.
    int oldlumdccoeff = 0, oldCbdccoeff = 0, oldCrdccoeff = 0;

    for (int y = 0; y < height / 8; ++y) {
        for (int x = 0; x < width / 8; ++x) {

            auto [matL_base, newlumdccoeff] = BuildMatrix(0,
                                                          quant[quantMapping[0]], oldlumdccoeff, dataStream, pos);
            auto [matCr_base, newCrdccoeff] = BuildMatrix(0,
                                                          quant[quantMapping[1]], oldCbdccoeff, dataStream, pos);
            auto [matCb_base, newCbdccoeff] = BuildMatrix(0,
                                                          quant[quantMapping[2]], oldCbdccoeff, dataStream, pos);



            if (x == blockCoordinate.first && y == blockCoordinate.second) {
                WriteDecodedMatrix(x, y, matL_base, matCb_base, matCr_base, output, scaling_factor, pixelCoordinate);
            }
//            WriteCompressedMatrix(x, y, img_data, output, scaling_factor);
//
            oldlumdccoeff = newlumdccoeff;
            oldCrdccoeff = newCrdccoeff;
            oldCbdccoeff = newCbdccoeff;
        }
    }

    return lenchunk+hdrlen;
}

void decodeImage(std::vector<std::vector<std::tuple<int, int, int>>>& output, const std::vector<uint8_t>& img_data, int& scaling_factor, std::pair<int, int>& blockCoordinate, std::pair<int, int>& pixelCoordinate){

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
                len_chunk = StartOfScan(data, len_chunk, quantMapping, height, width, quant, data, output, dataStream, pos, scaling_factor, blockCoordinate, pixelCoordinate);
            }
            data.erase(data.begin(), data.begin() + len_chunk);
        }
    }
}