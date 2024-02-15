
"""
A Huffman Table class
"""
hfTables = []

def initialize():
    global hfTables
    root = []
    elements = []
    hfTables.append([root, elements])

def BitsFromLengths(root, element, pos):
    if isinstance(root, list):
        if pos == 0:
            if len(root) < 2:
                root.append(element)
                return True
            return False
        for i in [0, 1]:
            if len(root) == i:
                root.append([])
            if BitsFromLengths(root[i], element, pos - 1) == True:
                return True
    return False

def GetHuffmanBits(lengths, elements):
    global hfTables
    hfTables[-1][1] = elements
    ii = 0
    for i in range(len(lengths)):
        for j in range(lengths[i]):
            BitsFromLengths(hfTables[-1][0], elements[ii], i)
            ii += 1

def Find(st, root, elements):
    r = root
    while isinstance(r, list):
        r = r[st.GetBit()]
    return r

def GetCode(st, root, elements):
    while True:
        res = Find(st, root, elements)
        if res == 0:
            return 0
        elif res != -1:
            return res