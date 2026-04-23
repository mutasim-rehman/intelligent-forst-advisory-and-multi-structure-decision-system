#ifndef IFAMDS_LINKED_LIST_H
#define IFAMDS_LINKED_LIST_H

#include <cstddef>
#include <iostream>
#include <functional>

namespace ifamds {

// ============================================================
// Singly Linked List (L1-L3: Raw/Verified/Anomaly event streams)
// Insert at tail: O(1) with tail pointer
// Traversal: O(n)
// ============================================================
template<typename T>
class SinglyLinkedList {
public:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };

    SinglyLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

    ~SinglyLinkedList() { clear(); }

    // Insert at the end of the list - O(1)
    void pushBack(const T& value) {
        Node* newNode = new Node(value);
        if (!head_) {
            head_ = tail_ = newNode;
        } else {
            tail_->next = newNode;
            tail_ = newNode;
        }
        size_++;
    }

    // Insert at the front of the list - O(1)
    void pushFront(const T& value) {
        Node* newNode = new Node(value);
        newNode->next = head_;
        head_ = newNode;
        if (!tail_) tail_ = newNode;
        size_++;
    }

    // Remove from the front - O(1)
    bool popFront(T& outValue) {
        if (!head_) return false;
        Node* temp = head_;
        outValue = temp->data;
        head_ = head_->next;
        if (!head_) tail_ = nullptr;
        delete temp;
        size_--;
        return true;
    }

    // Traverse forward and apply a function to each element - O(n)
    void traverseForward(std::function<void(const T&)> fn) const {
        Node* current = head_;
        while (current) {
            fn(current->data);
            current = current->next;
        }
    }

    // Get the last element - O(1) with tail pointer
    bool back(T& outValue) const {
        if (!tail_) return false;
        outValue = tail_->data;
        return true;
    }

    // Get the first element - O(1)
    bool front(T& outValue) const {
        if (!head_) return false;
        outValue = head_->data;
        return true;
    }

    Node* getHead() const { return head_; }
    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
        while (head_) {
            Node* temp = head_;
            head_ = head_->next;
            delete temp;
        }
        tail_ = nullptr;
        size_ = 0;
    }

private:
    Node* head_;
    Node* tail_;
    std::size_t size_;
};

// ============================================================
// Doubly Linked List (L4-L6: Forward/Backward correction chains)
// Insert at tail: O(1)
// Forward/backward traversal: O(n)
// Remove from tail (for rollback): O(1)
// ============================================================
template<typename T>
class DoublyLinkedList {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        Node(const T& d) : data(d), prev(nullptr), next(nullptr) {}
    };

    DoublyLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

    ~DoublyLinkedList() { clear(); }

    // Insert at the end - O(1)
    void pushBack(const T& value) {
        Node* newNode = new Node(value);
        if (!tail_) {
            head_ = tail_ = newNode;
        } else {
            newNode->prev = tail_;
            tail_->next = newNode;
            tail_ = newNode;
        }
        size_++;
    }

    // Remove from the end (rollback) - O(1)
    bool popBack(T& outValue) {
        if (!tail_) return false;
        Node* temp = tail_;
        outValue = temp->data;
        tail_ = tail_->prev;
        if (tail_) tail_->next = nullptr;
        else head_ = nullptr;
        delete temp;
        size_--;
        return true;
    }

    // Traverse forward - O(n)
    void traverseForward(std::function<void(const T&)> fn) const {
        Node* current = head_;
        while (current) {
            fn(current->data);
            current = current->next;
        }
    }

    // Traverse backward - O(n)
    void traverseBackward(std::function<void(const T&)> fn) const {
        Node* current = tail_;
        while (current) {
            fn(current->data);
            current = current->prev;
        }
    }

    // Get the last element - O(1)
    bool back(T& outValue) const {
        if (!tail_) return false;
        outValue = tail_->data;
        return true;
    }

    Node* getHead() const { return head_; }
    Node* getTail() const { return tail_; }
    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
        while (head_) {
            Node* temp = head_;
            head_ = head_->next;
            delete temp;
        }
        tail_ = nullptr;
        size_ = 0;
    }

private:
    Node* head_;
    Node* tail_;
    std::size_t size_;
};

// ============================================================
// Circular Linked List (L7-L10: Monitoring loops)
// Insert: O(1) at tail (tail->next always = head)
// Traverse N cycles: O(n * cycles)
// ============================================================
template<typename T>
class CircularLinkedList {
public:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };

    CircularLinkedList() : tail_(nullptr), size_(0) {}

    ~CircularLinkedList() { clear(); }

    // Insert at end, maintain circular property - O(1)
    void insert(const T& value) {
        Node* newNode = new Node(value);
        if (!tail_) {
            newNode->next = newNode;  // points to itself
            tail_ = newNode;
        } else {
            newNode->next = tail_->next;  // new node points to head
            tail_->next = newNode;        // old tail points to new node
            tail_ = newNode;              // update tail
        }
        size_++;
    }

    // Traverse the list for a given number of cycles, calling fn on each node - O(n * cycles)
    void traverse(int cycles, std::function<void(const T&, int)> fn) const {
        if (!tail_ || cycles <= 0) return;
        Node* current = tail_->next;  // start at head
        int totalVisits = static_cast<int>(size_) * cycles;
        for (int i = 0; i < totalVisits; ++i) {
            fn(current->data, i);
            current = current->next;
        }
    }

    // Get head node - O(1)
    Node* getHead() const { return tail_ ? tail_->next : nullptr; }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
        if (!tail_) return;
        Node* current = tail_->next;  // head
        for (std::size_t i = 0; i < size_; ++i) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        tail_ = nullptr;
        size_ = 0;
    }

private:
    Node* tail_;
    std::size_t size_;
};

}  // namespace ifamds

#endif
