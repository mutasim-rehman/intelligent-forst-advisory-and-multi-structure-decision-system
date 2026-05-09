#ifndef IFAMDS_LINKED_LIST_H
#define IFAMDS_LINKED_LIST_H

#include <iostream>

namespace ifamds {

template<typename T>
class SinglyLinkedList {
public:
    struct Node {
        T data;
        Node* next;
        Node(T d) : data(d), next(nullptr) {}
    };

private:
    Node* head;
    Node* tail;
    int current_size;

public:
    SinglyLinkedList() {
        head = nullptr;
        tail = nullptr;
        current_size = 0;
    }

    ~SinglyLinkedList() { clear(); }

    void pushBack(T value) {
        Node* newNode = new Node(value);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        current_size++;
    }

    Node* getHead() const { return head; }
    int size() const { return current_size; }
    bool empty() const { return current_size == 0; }

    void clear() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        current_size = 0;
    }
};

template<typename T>
class DoublyLinkedList {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        Node(T d) : data(d), prev(nullptr), next(nullptr) {}
    };

private:
    Node* head;
    Node* tail;
    int current_size;

public:
    DoublyLinkedList() {
        head = nullptr;
        tail = nullptr;
        current_size = 0;
    }

    ~DoublyLinkedList() { clear(); }

    void pushBack(T value) {
        Node* newNode = new Node(value);
        if (tail == nullptr) {
            head = tail = newNode;
        } else {
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }
        current_size++;
    }

    bool popBack(T& outValue) {
        if (tail == nullptr) return false;
        Node* temp = tail;
        outValue = temp->data;
        tail = tail->prev;
        if (tail != nullptr) {
            tail->next = nullptr;
        } else {
            head = nullptr;
        }
        delete temp;
        current_size--;
        return true;
    }

    Node* getHead() const { return head; }
    Node* getTail() const { return tail; }
    int size() const { return current_size; }
    bool empty() const { return current_size == 0; }

    void clear() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        current_size = 0;
    }
};

template<typename T>
class CircularLinkedList {
public:
    struct Node {
        T data;
        Node* next;
        Node(T d) : data(d), next(nullptr) {}
    };

private:
    Node* tail;
    int current_size;

public:
    CircularLinkedList() {
        tail = nullptr;
        current_size = 0;
    }

    ~CircularLinkedList() { clear(); }

    void insert(T value) {
        Node* newNode = new Node(value);
        if (tail == nullptr) {
            newNode->next = newNode;
            tail = newNode;
        } else {
            newNode->next = tail->next;
            tail->next = newNode;
            tail = newNode;
        }
        current_size++;
    }

    Node* getHead() const { 
        if (tail == nullptr) return nullptr;
        return tail->next; 
    }

    int size() const { return current_size; }
    bool empty() const { return current_size == 0; }

    void clear() {
        if (tail == nullptr) return;
        Node* current = tail->next;
        for (int i = 0; i < current_size; ++i) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        tail = nullptr;
        current_size = 0;
    }
};

} // namespace ifamds

#endif // IFAMDS_LINKED_LIST_H
