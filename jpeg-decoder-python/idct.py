import math
import numpy as np

base = []
zigzag = [[]]
idct_precision = 0
idct_table = []

def initialize():
    """
    Initializes the required variables.
    """
    global base, zigzag, idct_precision, idct_table
    base = [0] * 64
    zigzag = [
        [0, 1, 5, 6, 14, 15, 27, 28],
        [2, 4, 7, 13, 16, 26, 29, 42],
        [3, 8, 12, 17, 25, 30, 41, 43],
        [9, 11, 18, 24, 31, 40, 44, 53],
        [10, 19, 23, 32, 39, 45, 52, 54],
        [20, 22, 33, 38, 46, 51, 55, 60],
        [21, 34, 37, 47, 50, 56, 59, 61],
        [35, 36, 48, 49, 57, 58, 62, 63],
    ]
    idct_precision = 8
    idct_table = []

    for u in range(idct_precision):
        row = []
        for x in range(idct_precision):
            value = NormCoeff(u) * math.cos(((2.0 * x + 1.0) * u * math.pi) / 16.0)
            row.append(value)
        idct_table.append(row)

def NormCoeff(n):
    """
    helper function for the IDCT calculation
    """
    if n == 0:
        return 1.0 / math.sqrt(2.0)
    else:
        return 1.0

def rearrange_using_zigzag():
    """
    rearrange the MCU matrix by undoing the zigzag encoding
    """
    global zigzag
    for x in range(8):
        for y in range(8):
            zigzag[x][y] = base[zigzag[x][y]]
    return zigzag

def perform_IDCT():
    """
    undo the Discrete Cosine Transformation
    """
    global base
    out = []
    for i in range(8):
        out.append([])
        for j in range(8):
            out[i].append(0)

    for x in range(8):
        for y in range(8):
            local_sum = 0
            for u in range(idct_precision):
                for v in range(idct_precision):
                    local_sum += (
                            zigzag[v][u]
                            * idct_table[u][x]
                            * idct_table[v][y]
                    )
            out[y][x] = local_sum // 4
    base = out
