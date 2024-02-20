
"""
A bit stream class with convenience methods
"""
data = []
pos = 0

def initialize(data_param):
    """
    Initializes the required global variables.
    """
    global data
    data = data_param
    print(id(data))

def GetBit():
    """
    returns the bit in position pos
    """
    global pos
    b = data[pos >> 3]
    s = 7 - (pos & 0x7)
    pos += 1
    return (b >> s) & 1

def GetBitN(l):
    """
    returns l number of bits from position pos
    """
    val = 0
    for _ in range(l):
        val = (val << 1) + GetBit()
    return val
