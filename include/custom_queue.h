#ifndef IFAMDS_CUSTOM_QUEUE_H
#define IFAMDS_CUSTOM_QUEUE_H

namespace ifamds {

// Basic FIFO Queue using linked list
template<typename T>
class CustomQueue {
private:
    struct Node {
        T data;
        Node* next;
        Node(T d) : data(d), next(nullptr) {}
    };
    Node* front_node;
    Node* rear_node;
    int current_size;

public:
    CustomQueue() {
        front_node = nullptr;
        rear_node = nullptr;
        current_size = 0;
    }

    ~CustomQueue() { clear(); }

    void enqueue(T value) {
        Node* newNode = new Node(value);
        if (rear_node == nullptr) {
            front_node = rear_node = newNode;
        } else {
            rear_node->next = newNode;
            rear_node = newNode;
        }
        current_size++;
    }

    bool dequeue(T& outValue) {
        if (front_node == nullptr) return false;
        Node* temp = front_node;
        outValue = temp->data;
        front_node = front_node->next;
        if (front_node == nullptr) {
            rear_node = nullptr;
        }
        delete temp;
        current_size--;
        return true;
    }

    bool empty() const { return current_size == 0; }
    int size() const { return current_size; }

    void clear() {
        while (front_node != nullptr) {
            Node* temp = front_node;
            front_node = front_node->next;
            delete temp;
        }
        rear_node = nullptr;
        current_size = 0;
    }
};

// Basic Priority Queue using array-based max-heap
template<typename T>
class CustomPriorityQueue {
private:
    T* data;
    int current_size;
    int capacity;

    void grow() {
        int newCap = (capacity == 0) ? 10 : capacity * 2;
        T* newData = new T[newCap];
        for (int i = 0; i < current_size; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCap;
    }

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (data[index] > data[parent]) { // Using simple '>'
                T temp = data[parent];
                data[parent] = data[index];
                data[index] = temp;
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int index) {
        while (true) {
            int largest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            
            if (left < current_size && data[left] > data[largest]) {
                largest = left;
            }
            if (right < current_size && data[right] > data[largest]) {
                largest = right;
            }
            
            if (largest != index) {
                T temp = data[index];
                data[index] = data[largest];
                data[largest] = temp;
                index = largest;
            } else {
                break;
            }
        }
    }

public:
    CustomPriorityQueue() {
        data = nullptr;
        current_size = 0;
        capacity = 0;
    }

    ~CustomPriorityQueue() { delete[] data; }

    void push(T value) {
        if (current_size >= capacity) {
            grow();
        }
        data[current_size] = value;
        heapifyUp(current_size);
        current_size++;
    }

    bool pop(T& outValue) {
        if (current_size == 0) return false;
        outValue = data[0];
        current_size--;
        if (current_size > 0) {
            data[0] = data[current_size];
            heapifyDown(0);
        }
        return true;
    }

    bool empty() const { return current_size == 0; }
    int size() const { return current_size; }
};

} // namespace ifamds

#endif // IFAMDS_CUSTOM_QUEUE_H
