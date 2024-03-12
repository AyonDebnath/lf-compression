//
// Created by Ayon Debnath on 2024-03-09.
//

#include <iostream>

#include <vector>
#include <variant>
#include "stream.h"

std::vector<std::vector<int> > hfTables;

struct TreeNode;
using NodeElement = std::variant<int, TreeNode*>;

struct TreeNode {
    std::vector<NodeElement> elements;

    TreeNode(const std::vector<NodeElement>& vals = {}) : elements(vals) {}
};

struct Tree {
    TreeNode *root;

    Tree(const std::vector<NodeElement> &rootElements) {
        root = new TreeNode(rootElements);
    }

    void addChild(TreeNode *parent, TreeNode *child) {
        parent->elements.push_back(child);
    }

    void addChild(TreeNode *node, int newData) {
        node->elements.push_back(newData);
    }

    int countElements(TreeNode *node) {
        return node->elements.size();
    }

    TreeNode *getRoot() const {
        return root;
    }

    void print(TreeNode *node, int level = 0) {
        if (node == nullptr)
            return;

        for (int i = 0; i < level; ++i)
            std::cout << "  ";

        std::cout << "Node elements: ";
        for (const auto &element: node->elements) {
            if (std::holds_alternative<int>(element))
                std::cout << std::get<int>(element) << " ";
            else if (std::holds_alternative<TreeNode *>(element))
                std::cout << "TreeNode ";
        }
        std::cout << std::endl;

        for (const auto &element: node->elements) {
            if (std::holds_alternative<TreeNode *>(element))
                print(std::get<TreeNode *>(element), level + 1);
        }
    }
};

bool BitsFromLengths(Tree& tree, int element, int pos, TreeNode* currentRoot) {
    if (pos == 0) {
        if (tree.countElements(currentRoot) < 2) {
            tree.addChild(currentRoot, element);
            return true;
        }
        return false;
    }
    for (int i = 0; i < 2; ++i) {
        if (tree.countElements(tree.getRoot()) == i) {
            tree.addChild( tree.getRoot(), new TreeNode());
        }

        std::vector<NodeElement> nodeElements = currentRoot->elements;


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

            if(BitsFromLengths(tree, element, pos - 1, treeNode)){
                return true;
            }
        }
    }
    return false;
}

void GetHuffmanBits(std::vector<int>& lengths, std::vector<int>& elements) {
    std::vector<int> tree;
    hfTables.push_back({tree, elements});
    int ii = 0;
    for (size_t i = 0; i < lengths.size(); ++i) {
        for (int j = 0; j < lengths[i]; ++j) {
            BitsFromLengths(hfTables.back()[0], elements[ii], i);
            ii++;
        }
    }
}

int GetRoot(std::vector<int>& tree, std::vector<unsigned char>& data, int& pos) {
    int root = 0;
    while (!tree.empty() && tree[0] == -1) {
        root = tree[root + 1 + GetBit(data, pos)];
    }
    if (root == 0) {
        return 0;
    } else if (root != -1) {
        return root;
    }
    return 0; // Adjust return value according to your logic
}