#ifndef IFAMDS_CUSTOM_VECTOR_H
#define IFAMDS_CUSTOM_VECTOR_H

#include <cstddef>
#include <stdexcept>
#include <iostream>

namespace ifamds {

template<typename T>
class CustomVector {
public:
    using value_type = T;
    using size_type = std::size_t;
    using reference = T&;
    using const_reference = const T&;
    using iterator = T*;
    using const_iterator = const T*;

    CustomVector() : data_(nullptr), size_(0), capacity_(0) {}

    explicit CustomVector(size_type count, const T& value = T()) : data_(nullptr), size_(0), capacity_(0) {
        if (count > 0) {
            reserve(count);
            for (size_type i = 0; i < count; ++i) {
                data_[i] = value;
            }
            size_ = count;
        }
    }

    CustomVector(const CustomVector& other) : data_(nullptr), size_(0), capacity_(0) {
        if (other.size_ > 0) {
            reserve(other.size_);
            for (size_type i = 0; i < other.size_; ++i) {
                data_[i] = other.data_[i];
            }
            size_ = other.size_;
        }
    }

    CustomVector& operator=(const CustomVector& other) {
        if (this != &other) {
            clear();
            if (other.size_ > 0) {
                reserve(other.size_);
                for (size_type i = 0; i < other.size_; ++i) {
                    data_[i] = other.data_[i];
                }
                size_ = other.size_;
            }
        }
        return *this;
    }

    ~CustomVector() {
        delete[] data_;
    }

    void reserve(size_type new_cap) {
        if (new_cap > capacity_) {
            T* new_data = new T[new_cap];
            for (size_type i = 0; i < size_; ++i) {
                new_data[i] = data_[i];
            }
            delete[] data_;
            data_ = new_data;
            capacity_ = new_cap;
        }
    }

    void push_back(const T& value) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 8 : capacity_ * 2);
        }
        data_[size_++] = value;
    }

    void pop_back() {
        if (size_ > 0) {
            size_--;
        }
    }

    void clear() {
        size_ = 0;
    }

    void resize(size_type new_size, const T& value = T()) {
        if (new_size > size_) {
            reserve(new_size);
            for (size_type i = size_; i < new_size; ++i) {
                data_[i] = value;
            }
        }
        size_ = new_size;
    }

    reference operator[](size_type pos) {
        return data_[pos];
    }

    const_reference operator[](size_type pos) const {
        return data_[pos];
    }

    size_type size() const { return size_; }
    bool empty() const { return size_ == 0; }
    size_type capacity() const { return capacity_; }

    iterator begin() { return data_; }
    const_iterator begin() const { return data_; }
    iterator end() { return data_ + size_; }
    const_iterator end() const { return data_ + size_; }

    reference front() { return data_[0]; }
    const_reference front() const { return data_[0]; }
    reference back() { return data_[size_ - 1]; }
    const_reference back() const { return data_[size_ - 1]; }

private:
    T* data_;
    size_type size_;
    size_type capacity_;
};

} // namespace ifamds

#endif // IFAMDS_CUSTOM_VECTOR_H
