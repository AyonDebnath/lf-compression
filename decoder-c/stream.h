//
// Created by Ayon Debnath on 2024-03-09.
//
#include <iostream>
#ifndef DECODER_C_STREAM_H
#define DECODER_C_STREAM_H

int GetBit(std::vector<unsigned char>& data, int& pos);
int GetBitN(int l, std::vector<unsigned char>& data, int& pos);

#endif //DECODER_C_STREAM_H
