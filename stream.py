
"""
A bit stream class with convenience methods
"""
data = []
pos = 0

def initialize(data_param):
    global data
    data = data_param

def GetBit():
    global pos
    b = data[pos >> 3]
    s = 7 - (pos & 0x7)
    pos += 1
    return (b >> s) & 1

def GetBitN(l):
    val = 0
    for _ in range(l):
        val = val * 2 + GetBit()
    return val

def len(self):
    return len(self.data)