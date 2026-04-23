#ifndef IFAMDS_CUSTOM_QUEUE_H
#define IFAMDS_CUSTOM_QUEUE_H

#include <cstddef>
#include <functional>

namespace ifamds {

// ============================================================
// Custom FIFO Queue (linked-list-based)
// Used for Q1: Routine, Q2: Surveillance, Q4: Multi-factor
// Enqueue: O(1), Dequeue: O(1)
// ============================================================
template<typename T>
class CustomQueue {
public:
    CustomQueue() : front_(nullptr), rear_(nullptr), size_(0) {}

    ~CustomQueue() { clear(); }

    // Enqueue at the rear - O(1)
    void enqueue(const T& value) {
        Node* newNode = new Node(value);
        if (!rear_) {
            front_ = rear_ = newNode;
        } else {
            rear_->next = newNode;
            rear_ = newNode;
        }
        size_++;
    }

    // Dequeue from the front - O(1)
    bool dequeue(T& outValue) {
        if (!front_) return false;
        Node* temp = front_;
        outValue = temp->data;
        front_ = front_->next;
        if (!front_) rear_ = nullptr;
        delete temp;
        size_--;
        return true;
    }

    // Peek at the front - O(1)
    bool peek(T& outValue) const {
        if (!front_) return false;
        outValue = front_->data;
        return true;
    }

    bool empty() const { return size_ == 0; }
    std::size_t size() const { return size_; }

    void clear() {
        while (front_) {
            Node* temp = front_;
            front_ = front_->next;
            delete temp;
        }
        rear_ = nullptr;
        size_ = 0;
    }

private:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };
    Node* front_;
    Node* rear_;
    std::size_t size_;
};

// ============================================================
// Custom Priority Queue (array-based max-heap)
// Used for Q3: Emergency response queue
// Push: O(log n), Pop: O(log n), Top: O(1)
// ============================================================
template<typename T, typename Compare = std::less<T>>
class CustomPriorityQueue {
public:
    CustomPriorityQueue() : data_(nullptr), size_(0), capacity_(0) {}

    ~CustomPriorityQueue() { delete[] data_; }

    // Push a value and heapify up - O(log n)
    void push(const T& value) {
        if (size_ >= capacity_) {
            grow();
        }
        data_[size_] = value;
        heapifyUp(size_);
        size_++;
    }

    // Pop the highest-priority element and heapify down - O(log n)
    bool pop(T& outValue) {
        if (size_ == 0) return false;
        outValue = data_[0];
        size_--;
        if (size_ > 0) {
            data_[0] = data_[size_];
            heapifyDown(0);
        }
        return true;
    }

    // Peek at the highest-priority element - O(1)
    bool top(T& outValue) const {
        if (size_ == 0) return false;
        outValue = data_[0];
        return true;
    }

    bool empty() const { return size_ == 0; }
    std::size_t size() const { return size_; }

private:
    // Grow array capacity when full - amortized O(1)
    void grow() {
        std::size_t newCap = (capacity_ == 0) ? 8 : capacity_ * 2;
        T* newData = new T[newCap];
        for (std::size_t i = 0; i < size_; ++i) {
            newData[i] = data_[i];
        }
        delete[] data_;
        data_ = newData;
        capacity_ = newCap;
    }

    // Restore heap property upward - O(log n)
    void heapifyUp(std::size_t index) {
        while (index > 0) {
            std::size_t parent = (index - 1) / 2;
            if (comp_(data_[parent], data_[index])) {
                T temp = data_[parent];
                data_[parent] = data_[index];
                data_[index] = temp;
                index = parent;
            } else {
                break;
            }
        }
    }

    // Restore heap property downward - O(log n)
    void heapifyDown(std::size_t index) {
        while (true) {
            std::size_t largest = index;
            std::size_t left = 2 * index + 1;
            std::size_t right = 2 * index + 2;
            if (left < size_ && comp_(data_[largest], data_[left])) {
                largest = left;
            }
            if (right < size_ && comp_(data_[largest], data_[right])) {
                largest = right;
            }
            if (largest != index) {
                T temp = data_[index];
                data_[index] = data_[largest];
                data_[largest] = temp;
                index = largest;
            } else {
                break;
            }
        }
    }

    T* data_;
    std::size_t size_;
    std::size_t capacity_;
    Compare comp_;
};

}  // namespace ifamds

#endif
