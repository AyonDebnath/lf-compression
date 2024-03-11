#include <iostream>
#include <vector>

// Define a struct for the node of the linked list
struct Node {
    // Union to store either an integer or a vector of integers
    union Data {
        int intValue;
        std::vector<int> vectorValue;

        Data() {} // Default constructor to initialize the union
        ~Data() {} // Destructor to manage memory for the vector
    };

    Data data; // Data stored in the node
    bool isVector; // Flag to indicate whether the data is an integer or a vector
    Node* next; // Pointer to the next node in the list
};

// Function to create a new node with an integer value
Node* createNode(int value) {
    Node* newNode = new Node;
    newNode->data.intValue = value;
    newNode->isVector = false;
    newNode->next = nullptr;
    return newNode;
}

// Function to create a new node with a vector value
Node* createNode(const std::vector<int>& value) {
    Node* newNode = new Node;
    newNode->data.vectorValue = value;
    newNode->isVector = true;
    newNode->next = nullptr;
    return newNode;
}

// Function to insert a new node at the end of the list
void insert(Node*& head, int value) {
    Node* newNode = createNode(value);
    if (head == nullptr) {
        head = newNode;
    } else {
        Node* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Function to insert a new node at the end of the list
void insert(Node*& head, const std::vector<int>& value) {
    Node* newNode = createNode(value);
    if (head == nullptr) {
        head = newNode;
    } else {
        Node* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Function to display the contents of the list
void display(Node* head) {
    Node* current = head;
    while (current != nullptr) {
        if (current->isVector) {
            std::cout << "Vector: ";
            for (int val : current->data.vectorValue) {
                std::cout << val << " ";
            }
        } else {
            std::cout << "Integer: " << current->data.intValue;
        }
        std::cout << std::endl;
        current = current->next;
    }
}

// Function to delete the entire linked list
void deleteList(Node*& head) {
    while (head != nullptr) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

int main() {
    Node* head = nullptr;

    // Insert some integer values
    insert(head, 5);
    insert(head, 10);
    insert(head, 15);

    // Insert some vector values
    std::vector<int> vec1 = {1, 2, 3};
    std::vector<int> vec2 = {4, 5, 6};
    insert(head, vec1);
    insert(head, vec2);

    // Display the contents of the list
    display(head);

    // Delete the list to free memory
    deleteList(head);

    return 0;
}
