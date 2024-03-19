//
// Created by Ayon Debnath on 2024-03-09.
//

#include <iostream>

#include <vector>
#include <variant>
#include "stream.h"
#include "huffmanTable.h"



bool BitsFromLengths(Tree& tree, int element, int pos, TreeNode& currentRoot) {
    if (pos == 0) {
        if (tree.countElements(currentRoot) < 2) {
            tree.addChild(currentRoot, element);
            return true;
        }
        return false;
    }
    for (int i = 0; i < 2; ++i) {

        if (tree.countElements(currentRoot) == i) {
            tree.addChild(currentRoot, new TreeNode());
        }

        std::vector<NodeElement> nodeElements = currentRoot.elements;


        for (NodeElement& nodeElement :nodeElements) {
            std::variant<int, TreeNode*> nodeElementValues = nodeElement;
            if (std::holds_alternative<int>(nodeElementValues)) {
                continue;
            }
            TreeNode* subtreeRoot = new TreeNode();

            // change the dataType of nodeElementValues from std::variant<int, TreeNode*> to TreeNode
            TreeNode* treeNodePtr = new TreeNode();
            nodeElementValues = treeNodePtr;

            TreeNode* treeNode = nullptr;
            if (auto ptr = std::get_if<TreeNode*>(&nodeElementValues)) {
                treeNode = *ptr; // Assigning the TreeNode* directly
            } else {
                std::cerr << "Variant does not contain a TreeNode*" << std::endl;
                // Handle the case where the variant doesn't hold a TreeNode* ///Should ever be printed
            }

            if(BitsFromLengths(tree, element, pos - 1, *treeNode)){
                return true;
            }
        }
    }
    return false;
}

void GetHuffmanBits(std::vector<uint8_t>& lengths, std::vector<uint8_t>& elements, std::vector<std::pair<Tree, std::vector<uint8_t>>>& hfTables) {
    hfTables.back().second = elements;
    int ii = 0;
    for (size_t i = 0; i < lengths.size(); ++i) {
        for (int j = 0; j < lengths[i]; ++j) {
            TreeNode * root = hfTables.back().first.getRoot();
            BitsFromLengths(hfTables.back().first, elements[ii], i, *root);
            ii++;
        }
    }
}

int GetRoot(Tree tree, std::vector<unsigned char>& data, int& pos) {
    NodeElement rootNodeElement;
    if(tree.getRoot() == nullptr){
        std::cout << "Root is null" <<std::endl;
        return 0;
    }
    while (true) {
        std::cout << "Root is NOT null" <<std::endl;
        rootNodeElement = (tree.getRoot()->elements[GetBit(data, pos)]);
        if(std::holds_alternative<int>(rootNodeElement)){
            int root = std::get<int>(rootNodeElement);
            return root;
        }

        std::cout<< "GetRoot might enter infinite loop" <<std::endl;
    }
}