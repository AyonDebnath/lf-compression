//
// Created by Ayon Debnath on 2024-03-09.
//
#include <iostream>
#include <vector>

int GetBit(std::vector<unsigned char>& data, int& pos) {
    int b = data[pos >> 3];
    int s = 7 - (pos & 0x7);
    pos++;
    return (b >> s) & 1;
}

int GetBitN(int l, std::vector<unsigned char>& data, int& pos) {
    int val = 0;
    for (int i = 0; i < l; ++i) {
        val = (val << 1) + GetBit(data, pos);
    }
    return val;
}