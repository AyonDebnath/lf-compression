//
// Created by Ayon Debnath on 2024-03-12.
//
#include <cstdlib>
#include <string>
#include <vector>

#ifndef DECODER_C_HUFFMANTABLE_H
#define DECODER_C_HUFFMANTABLE_H

struct TreeNode;
using NodeElement = std::variant<int, TreeNode*>;

struct TreeNode {
    std::vector<NodeElement> elements;

    TreeNode(const std::vector<NodeElement>& vals = {}) : elements(vals) {}
};

struct Tree {
    TreeNode *root;

    // Constructor for an empty tree
    Tree() : root(nullptr) {}

    Tree(const std::vector<NodeElement> &rootElements) {
        root = new TreeNode(rootElements);
    }

    void addChild(TreeNode& parent, TreeNode *child) {
        if(&parent == nullptr) {
            root = child;
            parent = *child;
        }else{
            parent.elements.push_back(child);
        }
    }

    void addChild(TreeNode& node, int newData) {
        if(&node == nullptr){
            NodeElement data = newData;
            std::vector<NodeElement> vectorData = {};
            vectorData.push_back(data);
            node = TreeNode(vectorData);
            root = &node;

        }else{
            node.elements.push_back(newData);
        }

    }

    int countElements(TreeNode& node) {
        if(&node == nullptr){
            return 0;
        }
        return node.elements.size();
    }

    TreeNode* getRoot() const {
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


bool BitsFromLengths(Tree& tree, int element, int pos, TreeNode& currentRoot);
void GetHuffmanBits(std::vector<uint8_t>& lengths, std::vector<uint8_t>& elements, std::vector<std::pair<Tree, std::vector<uint8_t>>>& hfTables);
int GetRoot(Tree tree, std::vector<unsigned char>& data, int& pos);
#endif //DECODER_C_HUFFMANTABLE_H
