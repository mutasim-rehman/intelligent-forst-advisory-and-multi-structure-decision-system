#ifndef IFAMDS_CUSTOM_VECTOR_H
#define IFAMDS_CUSTOM_VECTOR_H

namespace ifamds {

template<typename T>
class CustomVector {
private:
    T* data;
    int current_size;
    int current_capacity;

public:
    CustomVector() {
        current_capacity = 10;
        current_size = 0;
        data = new T[current_capacity];
    }

    CustomVector(int count, T value = T()) {
        current_capacity = count > 10 ? count * 2 : 10;
        current_size = count;
        data = new T[current_capacity];
        for(int i = 0; i < current_size; i++) {
            data[i] = value;
        }
    }

    CustomVector(const CustomVector& other) {
        current_capacity = other.current_capacity;
        current_size = other.current_size;
        data = new T[current_capacity];
        for(int i = 0; i < current_size; i++) {
            data[i] = other.data[i];
        }
    }

    CustomVector& operator=(const CustomVector& other) {
        if (this != &other) {
            delete[] data;
            current_capacity = other.current_capacity;
            current_size = other.current_size;
            data = new T[current_capacity];
            for(int i = 0; i < current_size; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    ~CustomVector() {
        delete[] data;
    }

    void push_back(T value) {
        if (current_size == current_capacity) {
            current_capacity *= 2;
            T* temp = new T[current_capacity];
            for (int i = 0; i < current_size; i++) {
                temp[i] = data[i];
            }
            delete[] data;
            data = temp;
        }
        data[current_size] = value;
        current_size++;
    }

    void pop_back() {
        if (current_size > 0) {
            current_size--;
        }
    }

    T& operator[](int index) {
        return data[index];
    }

    const T& operator[](int index) const {
        return data[index];
    }

    int size() const {
        return current_size;
    }

    bool empty() const {
        return current_size == 0;
    }

    void clear() {
        current_size = 0;
    }
    
    T* begin() { return data; }
    const T* begin() const { return data; }
    T* end() { return data + current_size; }
    const T* end() const { return data + current_size; }
    
    void resize(int new_size, T default_val = T()) {
        if (new_size > current_capacity) {
            int new_cap = new_size;
            T* temp = new T[new_cap];
            for (int i = 0; i < current_size; i++) {
                temp[i] = data[i];
            }
            delete[] data;
            data = temp;
            current_capacity = new_cap;
        }
        for (int i = current_size; i < new_size; i++) {
            data[i] = default_val;
        }
        current_size = new_size;
    }
};

} // namespace ifamds

#endif // IFAMDS_CUSTOM_VECTOR_H
