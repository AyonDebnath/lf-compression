import stream
hfTables = []

def initialize():
    """
    Initializes the required variables.
    """
    global hfTables
    root = []

    elements = []
    hfTables.append([root, elements])

def BitsFromLengths(root, element, pos):
    """

    The root list contains nested lists and represents a binary tree of the elements. Each element of the huffman table is passed in as a parameter from the GetHuffmanBits() function. This function is called for every element in the huffman code. The elements are grouped together in lists of 2 elements to
    represent a binary tree.
    """
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
    """
    lengths: list of lengths of each element in the huffman code
    elements: the elements in the huffman code
    The GetHuffmanBits takes in the lengths and elements, iterates over all the elements and puts them in a root list.
    """
    global hfTables
    hfTables[-1][1] = elements
    ii = 0
    for i in range(len(lengths)):
        for j in range(lengths[i]):
            BitsFromLengths(hfTables[-1][0], elements[ii], i)
            ii += 1

def Find(root):
    r = root
    while isinstance(r, list):
        r = r[stream.GetBit()]
    return r

def GetCode(root):
    """
    traverses the tree/root and returns the decoded bits using the Huffman table.
    """
    while True:
        res = Find(root)
        if res == 0:
            return 0
        elif res != -1:
            return res