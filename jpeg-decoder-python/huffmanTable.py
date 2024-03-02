import stream

hfTables = []


def initialize():
    """
    Initializes the required variables.
    """
    global hfTables
    tree = []

    elements = []
    hfTables.append([tree, elements])


def BitsFromLengths(tree, element, pos):
    """

    The root list contains nested lists and represents a binary tree of the elements. Each element of the huffman table is passed in as a parameter from the GetHuffmanBits() function. This function is called for every element in the huffman code. The elements are grouped together in lists of 2 elements to
    represent a binary tree.
    """
    if isinstance(tree, list):
        if pos == 0:
            if len(tree) < 2:
                tree.append(element)
                return True
            return False
        for i in [0, 1]:
            if len(tree) == i:
                tree.append([])
            if BitsFromLengths(tree[i], element, pos - 1) == True:
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


def GetRoot(tree):
    """
    traverses the tree/root and returns the encoded root node value of the tree
    """
    root = tree
    while isinstance(root, list):
        root = root[stream.GetBit()]
    if root == 0:
        return 0
    elif root != -1:
        return root
