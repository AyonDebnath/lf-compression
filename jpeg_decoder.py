import huffmanTable
import main
import idct as idct
import stream
import subprocess
from io import BytesIO
from PIL import Image
from struct import unpack

"""
JPEG class for decoding a baseline encoded JPEG image
"""

huffman_tables = {}
quant = {}
quantMapping = []
output = []
scaling_factor = 0
blockCoordinate = (0, 0)
img_data = ''
height = 0
width = 0


marker_mapping = {
    0xFFD8: "Start of Image",
    0xFFE0: "Application Default Header",
    0xFFDB: "Quantization Table",
    0xFFC0: "Start of Frame",
    0xFFC4: "Huffman Table",
    0xFFDA: "Start of Scan",
    0xFFD9: "End of Image",
}


def initialize(image_file, output_param, scaling_factor_param, blockCoordinate_param):
    """
    Initializes the required global variables.
    """
    global output, scaling_factor, blockCoordinate, img_data
    output = output_param
    scaling_factor = scaling_factor_param
    blockCoordinate = blockCoordinate_param
    with open(image_file, "rb") as f:
        img_data = f.read()

def convertImageWithSamplingFactor(input_image, output_image, sampling_factor):
    """
    Converts the JPEG image provided as input to a Y, Cr, Cb colour channel.
    """
    command = [
        "convert",
        input_image,
        "-sampling-factor",
        sampling_factor,
        output_image
    ]

    try:
        subprocess.run(command, check=True)
        print("Image converted with sampling factor 4:4:4.")
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}")


def PrintMatrix(m):
    """
    A convenience function for printing matrices
    """
    for j in range(8):
        print("|", end="")
        for i in range(8):
            print("%d  |" % m[i + j * 8], end="\t")
        print()
    print()


def Clamp(col):
    """
    Makes sure col is between 0 and 255.
    """
    col = 255 if col > 255 else col
    col = 0 if col < 0 else col
    return int(col)


def ColorConversion(Y, Cr, Cb):
    """
    Converts Y, Cr and Cb to RGB color space
    """
    R = Cr * (2 - 2 * 0.299) + Y
    B = Cb * (2 - 2 * 0.114) + Y
    G = (Y - 0.114 * B - 0.299 * R) / 0.587
    return (Clamp(B + 128), Clamp(G + 128), Clamp(R + 128))


def WriteMatrix(x, y, matL, matCb, matCr, output, scaling_factor):
    """
    Loops over a single 8x8 MCU and writes the decoded value on a 2d array representing the output image.
    """
    for yy in range(8):
        for xx in range(8):
            x1, y1 = (x * 8 + xx) * scaling_factor, (y * 8 + yy) * scaling_factor

            # colour the entire block T\
            for i in range(scaling_factor):
                for j in range(scaling_factor):
                    output[y1+i][x1+j] = ColorConversion(
                matL[yy][xx], matCb[yy][xx], matCr[yy][xx]
            )


def WriteCompressedMatrix(x, y, comp_image, output, scaling_factor):
    """
    Loops over a single 8x8 MCU and copies it on a 2d array representing the output image.
    """
    comp_image = Image.open(BytesIO(comp_image))
    for yy in range(8):
        for xx in range(8):
            x1, y1 = (x * 8 + xx) * scaling_factor, (y * 8 + yy) * scaling_factor
            for i in range(scaling_factor):
                for j in range(scaling_factor):
                    output[y1+i][x1+j] = comp_image.getpixel((x, y))
    return


def RemoveFF00(data):
    """
    Removes 0x00 after 0xff in the image scan section of JPEG
    """
    datapro = []
    i = 0
    while True:
        b, bnext = unpack("BB", data[i: i + 2])
        if b == 0xFF:
            if bnext != 0:
                break
            datapro.append(data[i])
            i += 2
        else:
            datapro.append(data[i])
            i += 1
    return datapro, i


def GetArray(type, l, length):
    """
    A convenience function for unpacking an array from bitstream
    """
    s = ""
    for i in range(length):
        s = s + type
    return list(unpack(s, l[:length]))


def DecodeNumber(code, bits):
    """
    code:
    bits:
    extract the delta encoded DC coefficient
    """
    l = 2 ** (code - 1)
    if bits >= l:
        return bits
    else:
        return bits - (2 * l - 1)


def DefineQuantizationTables(data):
    """
    This function first parses the header of a Quantization Table section. Subsequently, it stores the quantization data in a dictionary, utilizing the header value as the respective key. For luminance, the header value is set to 0, while for chrominance, it is set to 1.
    """
    global quant
    while (len(data) > 0):
        (hdr,) = unpack("B", data[0:1])
        quant[hdr] = GetArray("B", data[1: 1 + 64], 64)
        data = data[65:]

def BuildMatrix(idx, quant, olddccoeff):
    """
    quant : quantization table
    creates an inverse discreate cosine transformation matrix and Y, Cr, and Cb matrices.
    """
    global huffman_tables
    idct.initialize()

    code = huffmanTable.GetRoot(huffman_tables[0 + idx][0])
    bits = stream.GetBitN(code)
    dccoeff = DecodeNumber(code, bits) + olddccoeff

    idct.base[0] = (dccoeff) * quant[0]
    l = 1
    while l < 64:
        code = huffmanTable.GetRoot(huffman_tables[16 + idx][0])
        if code == 0:
            break

        # The first part of the AC key_len
        # is the number of leading zeros
        if code > 15:
            l += code >> 4
            code = code & 0x0F

        bits = stream.GetBitN(code)

        if l < 64:
            coeff = DecodeNumber(code, bits)
            idct.base[l] = coeff * quant[l]
            l += 1

    idct.rearrange_using_zigzag()
    idct.perform_IDCT()

    return idct.base, dccoeff

def StartOfScan(data, hdrlen):
    """
    This is the most involved step. All the decoded information so far serves as a map that helps us in navigating and decoding the actual image data which happens over here..
    """
    global height, width, quantMapping, quant, img_data, output, scaling_factor
    data, lenchunk = RemoveFF00(data[hdrlen:])

    stream.initialize(data)
    print(id(data))
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
            if x == blockCoordinate[0] and y == blockCoordinate[1]:
                # continue
                WriteCompressedMatrix(x, y, img_data, output, scaling_factor)
            WriteMatrix(x, y, matL_base, matCb_base, matCr_base, output, scaling_factor)
    return lenchunk + hdrlen

def BaselineDCT(data):
    """
    The BaselineDCT method extracts essential data from the Start of Frame (SOF) section. It then compiles the quantization table numbers for each component, storing them in the quantMapping list.
    """
    global height, width, quantMapping
    hdr, height, width, components = unpack(">BHHB", data[0:6])
    # print("size %ix%i" % (self.width,  self.height))

    for i in range(components):
        id, samp, QtbId = unpack("BBB", data[6 + i * 3: 9 + i * 3])
        quantMapping.append(QtbId)

def decodeHuffman(data):
    """
    When the huffman marker, 0xFFC4, is reached, it gets the huffman tables from the image data and stores it in the huffman_tables list
    """
    global huffman_tables
    while (len(data) > 0):
        offset = 0
        (header,) = unpack("B", data[offset: offset + 1])
        # print(header, header & 0x0F, (header >> 4) & 0x0F)
        offset += 1

        lengths = GetArray("B", data[offset: offset + 16], 16)
        offset += 16

        elements = []
        for i in lengths:
            elements += GetArray("B", data[offset: offset + i], i)
            offset += i

        huffmanTable.initialize()
        huffmanTable.GetHuffmanBits(lengths, elements)
        huffman_tables[header] = huffmanTable.hfTables[-1]
        data = data[offset:]

def decode():
    """
    Entry point to the decoder. It takes in the image data, loops through all the markers and decodes the Image.
    """
    data = img_data
    while len(data) != 0:
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

