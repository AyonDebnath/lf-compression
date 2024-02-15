from struct import unpack
import huffmanTable
import main
import idct as idct
import stream

"""
JPEG class for decoding a baseline encoded JPEG image
"""

huffman_tables = {}
quant = {}
quantMapping = []
output = []
scaling_factor = 0
coordinate = 0
img_data = ''
height = 0
width = 0

def initialize(image_file, output_param, scaling_factor_param, coordinate_param):
    global output, scaling_factor, coordinate, img_data
    output = output_param
    scaling_factor = scaling_factor_param
    coordinate = coordinate_param
    with open(image_file, "rb") as f:
        img_data = f.read()

def DefineQuantizationTables(data):
    global quant
    while (len(data) > 0):
        (hdr,) = unpack("B", data[0:1])
        quant[hdr] = main.GetArray("B", data[1: 1 + 64], 64)
        data = data[65:]

def BuildMatrix(idx, quant, olddccoeff):
    global huffman_tables
    idct.initialize()

    code = huffmanTable.GetCode(huffman_tables[0 + idx][0], huffman_tables[0 + idx][1])
    bits = stream.GetBitN(code)
    dccoeff = main.DecodeNumber(code, bits) + olddccoeff

    idct.base[0] = (dccoeff) * quant[0]
    l = 1
    while l < 64:
        code = huffmanTable.GetCode(huffman_tables[16 + idx][0], huffman_tables[16 + idx][1])
        if code == 0:
            break

        # The first part of the AC key_len
        # is the number of leading zeros
        if code > 15:
            l += code >> 4
            code = code & 0x0F

        bits = stream.GetBitN(code)

        if l < 64:
            coeff = main.DecodeNumber(code, bits)
            idct.base[l] = coeff * quant[l]
            l += 1

    idct.rearrange_using_zigzag()
    idct.perform_IDCT()

    return idct.base, dccoeff

def StartOfScan(data, hdrlen):
    global height, width, quantMapping, quant, img_data, output, scaling_factor
    data, lenchunk = main.RemoveFF00(data[hdrlen:])

    st = stream.initialize(data)
    oldlumdccoeff, oldCbdccoeff, oldCrdccoeff = 0, 0, 0
    for y in range(height // 8):
        for x in range(width // 8):

            matL_base, oldlumdccoeff = BuildMatrix(
                0, quant[quantMapping[0]], oldlumdccoeff
            )
            matCr_base, oldCrdccoeff = BuildMatrix(
                 1, quant[quantMapping[1]], oldCrdccoeff
            )
            matCb_base, oldCbdccoeff = BuildMatrix(
                1, quant[quantMapping[2]], oldCbdccoeff
            )
            if(x == coordinate.getX() and y - coordinate.getY()):
                # continue
                main.DrawCompressed(x, y, img_data, output, scaling_factor)
            main.DrawMatrix(x, y, matL_base, matCb_base, matCr_base, output, scaling_factor)
    return lenchunk + hdrlen

def BaselineDCT(data):
    global height, width, quantMapping
    hdr, height, width, components = unpack(">BHHB", data[0:6])
    # print("size %ix%i" % (self.width,  self.height))

    for i in range(components):
        id, samp, QtbId = unpack("BBB", data[6 + i * 3: 9 + i * 3])
        quantMapping.append(QtbId)

def decodeHuffman(data):
    global huffman_tables
    while (len(data) > 0):
        offset = 0
        (header,) = unpack("B", data[offset: offset + 1])
        # print(header, header & 0x0F, (header >> 4) & 0x0F)
        offset += 1

        lengths = main.GetArray("B", data[offset: offset + 16], 16)
        offset += 16

        elements = []
        for i in lengths:
            elements += main.GetArray("B", data[offset: offset + i], i)
            offset += i

        huffmanTable.initialize()
        huffmanTable.GetHuffmanBits(lengths, elements)
        huffman_tables[header] = huffmanTable.hfTables[-1]
        data = data[offset:]

def decode():

    data = img_data
    while True:
        (marker,) = unpack(">H", data[0:2])
        print(hex(marker))
        # print(marker_mapping.get(marker))
        if marker == 0xFFD8:
            lenchunk = 2
            data = data[lenchunk:]
        elif marker == 0xFFD9:
            break
        else:
            (len_chunk,) = unpack(">H", data[2:4])
            len_chunk += 2
            chunk = data[4:len_chunk]
            if marker == 0xFFC4:
                decodeHuffman(chunk)
            elif marker == 0xFFDB:
                DefineQuantizationTables(chunk)
            elif marker == 0xFFC0:
                BaselineDCT(chunk)
            elif marker == 0xFFDA:
                len_chunk = StartOfScan(data, len_chunk)
            data = data[len_chunk:]
        if len(data) == 0:
            break
